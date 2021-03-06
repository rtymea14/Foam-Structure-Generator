/*******************************************************************************
 * photons.cpp
 *
 * This module implements Photon Mapping.
 *
 * Author: Nathan Kopp
 *
 * ---------------------------------------------------------------------------
 * Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
 * Copyright 1991-2013 Persistence of Vision Raytracer Pty. Ltd.
 *
 * POV-Ray is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * POV-Ray is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * ---------------------------------------------------------------------------
 * POV-Ray is based on the popular DKB raytracer version 2.12.
 * DKBTrace was originally written by David K. Buck.
 * DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
 * ---------------------------------------------------------------------------
 * $File: //depot/public/povray/3.x/source/backend/lighting/photons.cpp $
 * $Revision: #1 $
 * $Change: 6069 $
 * $DateTime: 2013/11/06 11:59:40 $
 * $Author: chrisc $
 *******************************************************************************/

// frame.h must always be the first POV file included (pulls in platform config)
#include "backend/frame.h"
#include "base/povms.h"
#include "base/povmsgid.h"
#include "backend/math/vector.h"
#include "backend/math/matrices.h"
#include "backend/scene/objects.h"
#include "backend/shape/csg.h"
#include "backend/support/octree.h"
#include "backend/bounding/bbox.h"
#include "backend/scene/threaddata.h"
#include "backend/scene/scene.h"
#include "backend/scene/view.h"
#include "backend/support/msgutil.h"
#include "backend/lighting/point.h"
#include "backend/lighting/photons.h"
#include "backend/texture/normal.h"
#include "backend/texture/pigment.h"
#include "backend/texture/texture.h"
#include "backend/colour/colour.h"
#include "lightgrp.h"
#include "backend/lighting/photonshootingstrategy.h"

#include <algorithm>

// this must be the last file included
#include "base/povdebug.h"

// behavior of pass_through objects
#define PT_FILTER_BEFORE_TARGET     // pass_through objects encountered before target do affect photons color
//#define PT_AMPLIFY_BUG              // replicate bogus 3.6 "photon amplifier" effect

namespace pov
{

/* ------------------------------------------------------ */
/* global variables */
/* ------------------------------------------------------ */
SinCosOptimizations sinCosData;

// statistics helpers
//int gPhotonStat_i = 0;
//int gPhotonStat_x_samples = 0;
//int gPhotonStat_y_samples = 0;
//int gPhotonStat_end = 0;

/* ------------------------------------------------------ */
/* external variables */
/* ------------------------------------------------------ */
//extern int Trace_Level;
//extern int disp_elem;
//extern int disp_nelems;

/* ------------------------------------------------------ */
/* static functions */
/* ------------------------------------------------------ */
static void FreePhotonMemory();
static void InitPhotonMemory();
static int savePhotonMap(void);
static int loadPhotonMap(void);

/* ------------------------------------------------------ */
/* static variables */
/* ------------------------------------------------------ */

const int PHOTON_BLOCK_POWER = 14;
// PHOTON_BLOCK_SIZE must be equal to 2 raised to the power PHOTON_BLOCK_POWER
const int PHOTON_BLOCK_SIZE = (16384);
const int PHOTON_BLOCK_MASK = (PHOTON_BLOCK_SIZE-1);
const int INITIAL_BASE_ARRAY_SIZE = 100;


PhotonTrace::PhotonTrace(shared_ptr<SceneData> sd, TraceThreadData *td, unsigned int mtl, DBL adcb, unsigned int qf, Trace::CooperateFunctor& cf) :
	Trace(sd, td, qf, cf, mediaPhotons, noRadiosity),
	mediaPhotons(sd, td, this, new PhotonGatherer(&sd->mediaPhotonMap, sd->photonSettings))
{
}

PhotonTrace::~PhotonTrace()
{
}

DBL PhotonTrace::TraceRay(const Ray& ray, Colour& colour, COLC weight, Trace::TraceTicket& ticket, bool continuedRay, DBL maxDepth)
{
	Intersection bestisect;
	bool found;
	NoSomethingFlagRayObjectCondition precond;
	TrueRayObjectCondition postcond;

	POV_ULONG nrays = threadData->Stats()[Number_Of_Rays]++;
	if(((unsigned char) nrays & 0x0f) == 0x00)
		cooperate();

	// Check for max. trace level or ADC bailout.
	if((ticket.traceLevel >= ticket.maxAllowedTraceLevel) || (weight < ticket.adcBailout))
	{
		if(weight < ticket.adcBailout)
			threadData->Stats()[ADC_Saves]++;
		colour.clear();
		return BOUND_HUGE;
	}

	// Set highest level traced.

	ticket.traceLevel++;
	ticket.maxFoundTraceLevel = max(ticket.maxFoundTraceLevel, ticket.traceLevel);

	if (maxDepth >= EPSILON)
		bestisect.Depth = maxDepth;

	found = FindIntersection(bestisect, ray, precond, postcond);
	if(found)
	{
		// NK phmap
		int oldptflag = threadData->passThruPrev;
		threadData->passThruPrev = threadData->passThruThis;

		// if this is the photonPass and we're shooting at a target
		if(threadData->photonTargetObject)
		{
			// start with this assumption
			threadData->passThruThis = false;

			// if either this is the first ray or the last thing we hit was a pass-through object
			if((ticket.traceLevel==1) || threadData->passThruPrev)
			{
				if (Test_Flag(bestisect.Object, PH_TARGET_FLAG))
				{
					// We hit *some* photon target object

					if (!IsObjectInCSG(bestisect.Object,threadData->photonTargetObject))
					{
						// We did *not* hit *the* photon target

						if ( Test_Flag(bestisect.Object, PH_PASSTHRU_FLAG) //||
						//  ( Check_No_Shadow_Group(Best_Intersection.Object, photonOptions.Light) &&
						//  !Check_Light_Group(Best_Intersection.Object, photonOptions.Light) )
						)
							// Hit some passthrough object; just carry on tracing.
						{
							threadData->passThruThis = true;
						}
						else
						{
							// Hit some non-passthrough object other than our target; do *not* follow the ray any further.

							threadData->passThruThis = threadData->passThruPrev;
							threadData->passThruPrev = oldptflag;
							ticket.traceLevel--;
							return (BOUND_HUGE);
						}
					}
					else
					{
						threadData->hitObject = true;  // we need to know that we hit it
					}
				}
				else
				{
					// We did *not* hit a photon target

					if ( Test_Flag(bestisect.Object, PH_PASSTHRU_FLAG) //||
					//  ( Check_No_Shadow_Group(Best_Intersection.Object, photonOptions.Light) &&
					//  !Check_Light_Group(Best_Intersection.Object, photonOptions.Light) )
					)
						threadData->passThruThis = true;
					else
					{
						// Hit some non-passthrough non-target object; do *not* follow the ray any further.

						threadData->passThruThis = threadData->passThruPrev;
						threadData->passThruPrev = oldptflag;
						ticket.traceLevel--;
						return (BOUND_HUGE);
					}
				}
			}
		}

		ComputeTextureColour(bestisect, colour, ray, weight, true, ticket);

		// NK phmap
		threadData->passThruThis = threadData->passThruPrev;
		threadData->passThruPrev = oldptflag;
	}

	ticket.traceLevel--;

	if(found == false)
		return HUGE_VAL;
	else
		return bestisect.Depth;
}

void PhotonTrace::ComputeLightedTexture(Colour& LightCol, const TEXTURE *Texture, vector<const TEXTURE *>& warps, const Vector3d& ipoint, const Vector3d& rawnormal,
                                        const Ray& ray, COLC weight, Intersection& isect, Trace::TraceTicket& ticket)
{
	int i;
	int layer_number;
	int one_colour_found, colour_found;
	DBL w1;
	DBL New_Weight, TempWeight;
	DBL Att, Trans;
	Vector3d LayNormal, TopNormal;
	Colour LayCol;
	RGBColour FilCol;
	RGBColour AttCol, ResCol;
	Colour TmpCol;
	Interior *interior;
	const TEXTURE *Layer;
	Ray NewRay(ray);
	int doReflection, doDiffuse, doRefraction;
	DBL reflectionWeight, refractionWeight, diffuseWeight, dieWeight, totalWeight;
	DBL choice;
	DBL Cos_Angle_Incidence;
	int TIR_occured;

	WNRXVector listWNRX(wnrxPool);
	assert(listWNRX->empty()); // verify that the WNRXVector pulled from the pool is in a cleaned-up condition

	// LightCol is the color of the light beam.

	// result color for doing diffuse
	ResCol.clear();

	// NK 1998 - switched transmit component to zero
	FilCol = RGBColour(1.0);

	Trans = 1.0;

	// initialize the new ray... we will probably end up using it
	Assign_Vector(NewRay.Origin, isect.IPoint);

	// In the future, we could enhance this so that users can determine
	// how and when photons are deposited into different media.

	// TODO FIXME - [CLi] for the sake of performance, this should be handled in the calling function, in case we're dealing with averaged textures!
	// Calculate participating media effects, and deposit photons in media as we go.
	if((qualityFlags & Q_VOLUME) && (!ray.GetInteriors().empty()) && (ray.IsHollowRay() == true))
	{
		// Calculate effects of all media we're currently in.

			MediaVector medialist;

			for(RayInteriorVector::const_iterator i(ray.GetInteriors().begin()); i != ray.GetInteriors().end(); i++)
			{
				for(vector<Media>::iterator im((*i)->media.begin()); im != (*i)->media.end(); im++)
					medialist.push_back(&(*im));
			}

/*  TODO FIXME lightgroups
			if ((Trace_Level > 1) &&
			    !threadData->passThruPrev && sceneData->photonSettings.maxMediaSteps>0 &&
			    !Test_Flag(isect.Object,PH_IGNORE_PHOTONS_FLAG) &&
			    Check_Light_Group(isect.Object,photonOptions.Light))
*/
			if(!medialist.empty())
			{
				if((ticket.traceLevel > 1) && !threadData->passThruPrev && (sceneData->photonSettings.maxMediaSteps > 0))
					mediaPhotons.ComputeMediaAndDepositPhotons(medialist, ray, isect, LightCol, ticket);
				else
					// compute media WITHOUT depositing photons
					mediaPhotons.ComputeMedia(medialist, ray, isect, LightCol, ticket);
			}
	}

	// Get distance based attenuation.
	interior = isect.Object->interior;
	AttCol = RGBColour(interior->Old_Refract);

	if (interior != NULL)
	{
		if (ray.IsInterior(interior) == true)
		{
			if ((interior->Fade_Power > 0.0) && (fabs(interior->Fade_Distance) > EPSILON))
			{
				// NK attenuate
				if (interior->Fade_Power>=1000)
				{
					AttCol *= exp( -(RGBColour(1.0)-interior->Fade_Colour) * isect.Depth/interior->Fade_Distance );
				}
				else
				{
					Att = 1.0 + pow(isect.Depth / interior->Fade_Distance, (DBL) interior->Fade_Power);
					AttCol *= (interior->Fade_Colour + (RGBColour(1.0) - interior->Fade_Colour) / Att);
				}
			}
		}
	}
	LightCol.red()   *= AttCol.red();
	LightCol.green() *= AttCol.green();
	LightCol.blue()  *= AttCol.blue();

	// set size here
	threadData->photonDepth += isect.Depth;


	// First, we should save this photon!

	if ((ticket.traceLevel > 1) && !threadData->passThruPrev &&
	    !Test_Flag(isect.Object,PH_IGNORE_PHOTONS_FLAG) &&
	    Check_Photon_Light_Group(isect.Object))
	{
		addSurfacePhoton(isect.IPoint, ray.Origin, RGBColour(LightCol));
	}

#ifndef PT_FILTER_BEFORE_TARGET
	if (threadData->passThruThis)
	{
		Ray NRay(ray);

		Assign_Vector(NRay.Origin, isect.IPoint); // [CLi] this erroneously used *ipoint before
		Assign_Vector(NRay.Direction, ray.Direction);

#ifndef PT_AMPLIFY_BUG
		// Make sure we get insideness right
		if (interior != NULL)
		{
			if (!NRay.RemoveInterior(interior))
				NRay.AppendInterior(interior);
		}
#endif // PT_AMPLIFY_BUG

		TraceRay(NRay, LightCol, weight, ticket, true);

#ifndef PT_AMPLIFY_BUG
		return;
#endif // PT_AMPLIFY_BUG
	}
#endif // PT_FILTER_BEFORE_TARGET

	// Loop through the layers and compute the ambient, diffuse,
	// phong and specular for these textures.
	one_colour_found = false;
	for (layer_number = 0, Layer = Texture;
	     (Layer != NULL) && (Trans > ticket.adcBailout);
	     layer_number++, Layer = (TEXTURE *)Layer->Next)
	{
		// Get perturbed surface normal.
		LayNormal = rawnormal;

		if ((qualityFlags & Q_NORMAL) && (Layer->Tnormal != NULL))
		{
			for(vector<const TEXTURE *>::iterator i(warps.begin()); i != warps.end(); i++)
				Warp_Normal(*LayNormal, *LayNormal, reinterpret_cast<const TPATTERN *>(*i), Test_Flag((*i), DONT_SCALE_BUMPS_FLAG));

			Perturb_Normal(*LayNormal, Layer->Tnormal, *ipoint, &isect, &ray, threadData);

			if((Test_Flag(Layer->Tnormal, DONT_SCALE_BUMPS_FLAG)))
				LayNormal.normalize();

			for(vector<const TEXTURE *>::reverse_iterator i(warps.rbegin()); i != warps.rend(); i++)
				UnWarp_Normal(*LayNormal, *LayNormal, reinterpret_cast<const TPATTERN *>(*i), Test_Flag((*i), DONT_SCALE_BUMPS_FLAG));
		}

		// Store top layer normal.
		if (!layer_number)
		{
			TopNormal = LayNormal;
		}

		// Get surface colour.
		New_Weight = weight * Trans;
		colour_found = Compute_Pigment(LayCol, Layer->Pigment, *ipoint, &isect, &ray, threadData);

		Att = Trans * (1.0 - min(1.0, (DBL)(LayCol[pFILTER] + LayCol[pTRANSM])));

		LayCol.red()   *= FilCol.red();
		LayCol.green() *= FilCol.green();
		LayCol.blue()  *= FilCol.blue();

		ResCol += RGBColour(LayCol) * Att;

		// If a valid color was returned set one_colour_found to true.
		// An invalid color is returned if a surface point is outside
		// an image map used just once.
		if (colour_found)
		{
			one_colour_found = true;
		}

		listWNRX->push_back(WNRX(New_Weight, LayNormal, RGBColour(), Layer->Finish->Reflect_Exp));

		// angle-dependent reflectivity
		VDot(Cos_Angle_Incidence, ray.Direction, *LayNormal);
		Cos_Angle_Incidence *= -1.0;

		if ((isect.Object->interior != NULL) ||
		    (Layer->Finish->Reflection_Type != 1))
		{
			ComputeReflectivity (listWNRX->back().weight, listWNRX->back().reflec,
			                     Layer->Finish->Reflection_Max, Layer->Finish->Reflection_Min,
			                     Layer->Finish->Reflection_Type, Layer->Finish->Reflection_Falloff,
			                     Cos_Angle_Incidence, ray, isect.Object->interior);
		}
		else
		{
			throw POV_EXCEPTION_STRING("Reflection_Type 1 used with no interior."); // TODO FIXME - wrong place to report this [trf]
		}

		// Added by MBP for metallic reflection
		if (Layer->Finish->Reflect_Metallic != 0.0)
		{
			DBL R_M=Layer->Finish->Reflect_Metallic;

			DBL x = fabs(acos(Cos_Angle_Incidence)) / M_PI_2;
			DBL F = 0.014567225 / Sqr(x - 1.12) - 0.011612903;
			F=min(1.0,max(0.0,F));

			listWNRX->back().reflec[pRED]   *= (1.0 + R_M * (1.0 - F) * (LayCol[pRED]   - 1.0));
			listWNRX->back().reflec[pGREEN] *= (1.0 + R_M * (1.0 - F) * (LayCol[pGREEN] - 1.0));
			listWNRX->back().reflec[pBLUE]  *= (1.0 + R_M * (1.0 - F) * (LayCol[pBLUE]  - 1.0));
		}

		// NK - I think we SHOULD do something like this: (to apply the layer's color)
		//listWNRX->back().reflec.red()*=FilCol.red();
		//listWNRX->back().reflec.green()*=FilCol.green();
		//listWNRX->back().reflec.blue()*=FilCol.blue();

		// Get new filter color.
		if (colour_found)
		{
			FilCol *= LayCol.rgbTransm();
			if(Layer->Finish->Conserve_Energy)
			{
				// adjust filcol based on reflection
				// this would work so much better with r,g,b,rt,gt,bt
				FilCol.red()   *= min(1.0,1.0-listWNRX->back().reflec.red());
				FilCol.green() *= min(1.0,1.0-listWNRX->back().reflec.green());
				FilCol.blue()  *= min(1.0,1.0-listWNRX->back().reflec.blue());
			}
		}

		// Get new remaining translucency.
		Trans = min(1.0f, std::fabs(FilCol.greyscale()));
	}

	//******************
	// now that we have color info, we can determine what we want to do next
	//*******************

	if (threadData->photonTargetObject)
	{
		// if photon is for caustic map, then do
		//   reflection/refraction always
		//   diffuse never
		doReflection = 1;
		doRefraction = 1;
		doDiffuse = 0;
	}
	else
	{
		// if photon is for global map, then decide which we want to do
		diffuseWeight = max(0.0f, std::fabs(ResCol.greyscale()));
		// use top-layer finish only
		if(Texture->Finish)
			diffuseWeight*=Texture->Finish->Diffuse;
		refractionWeight = Trans;
		// reflection only for top layer!!!!!!
		// TODO is "rend()" the top layer or the bottom layer???
		reflectionWeight = max(0.0f, std::fabs(listWNRX->rend()->reflec.greyscale()));
		dieWeight = max(0.0,(1.0-diffuseWeight));

		// normalize weights: make sum be 1.0
		totalWeight = reflectionWeight + refractionWeight + diffuseWeight + dieWeight;
		if ((reflectionWeight + refractionWeight + diffuseWeight) > ticket.adcBailout)
		{
			diffuseWeight /= totalWeight;
			refractionWeight /= totalWeight;
			reflectionWeight /= totalWeight;
			dieWeight /= totalWeight;

			// now, determine which we want to use
			choice = randomNumberGenerator();
			if (choice<diffuseWeight)
			{
				// do diffuse
				ResCol /= diffuseWeight;
				// debugging code left here for future use when global photons are fully implemented...
				//if (diffuseWeight<.999)
				//{
				//  ResCol *= 20.0;
				//}
				//else
				//{
				//  ResCol *= 2.0;
				//}
				doReflection = 0;
				doRefraction = 0;
				doDiffuse = 1;
				threadData->passThruPrev = false;
			}
			else if (choice<diffuseWeight+refractionWeight)
			{
				// do refraction
				FilCol /= refractionWeight;
				doReflection = 0;
				doRefraction = 1;
				doDiffuse = 0;
				threadData->passThruPrev = true;
			}
			else if (choice<diffuseWeight+refractionWeight+reflectionWeight)
			{
				// do reflection
				// TODO again, is "rend()" the top layer?
				listWNRX->rend()->reflec /= reflectionWeight;
				doReflection = 1;
				doRefraction = 0;
				doDiffuse = 0;
				threadData->passThruPrev = true;
			}
			// else die
		}
		else
		{
			doReflection = 0;
			doRefraction = 0;
			doDiffuse = 0;
		}

	}

#ifdef PT_FILTER_BEFORE_TARGET
	if (threadData->passThruThis)
	{
		w1 = max3(FilCol.red(), FilCol.green(), FilCol.blue());

		New_Weight = weight * max(w1, 0.0); // TODO FIXME - check if the max() is necessary

		// Trace refracted ray.
		threadData->GFilCol = FilCol;
		Vector3d tmpIPoint(isect.IPoint);

		Ray NRay(ray);

		Assign_Vector(NRay.Origin, isect.IPoint);
		Assign_Vector(NRay.Direction, ray.Direction);

		Colour CurLightCol;
		RGBColour GFilCol = threadData->GFilCol;
		CurLightCol.red()   = LightCol.red()   * GFilCol.red();
		CurLightCol.green() = LightCol.green() * GFilCol.green();
		CurLightCol.blue()  = LightCol.blue()  * GFilCol.blue();

#ifndef PT_AMPLIFY_BUG
		// Make sure we get insideness right
		if (interior != NULL)
		{
			if (!NRay.RemoveInterior(interior))
				NRay.AppendInterior(interior);
		}
#endif // PT_AMPLIFY_BUG

		TraceRay(NRay, CurLightCol, (float)New_Weight, ticket, true);

#ifndef PT_AMPLIFY_BUG
		return;
#endif // PT_AMPLIFY_BUG
	}
#endif // PT_FILTER_BEFORE_TARGET

	// Shoot diffusely scattered photons.

	if (doDiffuse)
	{
		//ChooseRay(Ray &NewRay, VECTOR Normal, Ray &ray, VECTOR Raw_Normal, int WhichRay)
		ChooseRay(NewRay, *LayNormal, ray, *rawnormal, rand()%400);

		Colour CurLightCol;
		CurLightCol.red() = LightCol.red()*ResCol.red();
		CurLightCol.green() = LightCol.green()*ResCol.green();
		CurLightCol.blue() = LightCol.blue()*ResCol.blue();

		// don't trace if < EPSILON

		// now trace it
		TraceRay(NewRay, CurLightCol, 1.0, ticket, false);
	}

	// Shoot refracted photons.

	TIR_occured = false;

	// only really do refraction if...
	//  (1) We have already reached the target, -and-
	//  (2) (a) photon refraction is turned on for the object, and not explicitly turned off for the light, -or-
	//      (b) photon refraction is turned on for the light, and not explicitly turned off for the object
	doRefraction = doRefraction &&
	               ( ( Test_Flag(isect.Object, PH_RFR_ON_FLAG) && !(threadData->photonSourceLight->Flags & PH_RFR_OFF_FLAG) ) ||
	                 ( !Test_Flag(isect.Object, PH_RFR_OFF_FLAG) && (threadData->photonSourceLight->Flags & PH_RFR_ON_FLAG) ) ||
	                 threadData->passThruThis );

	// If the surface is translucent a transmitted ray is traced
	// and its illunination is filtered by FilCol.
	if (doRefraction && ((interior = isect.Object->interior) != NULL) && (Trans > ticket.adcBailout) && (qualityFlags & Q_REFRACT))
	{
		w1 = max3(FilCol.red(), FilCol.green(), FilCol.blue());

		New_Weight = weight * max(w1, 0.0); // TODO FIXME - check if the max() is necessary

		// Trace refracted ray.
		threadData->GFilCol = FilCol;
		Vector3d tmpIPoint(isect.IPoint);

		TIR_occured = ComputeRefractionForPhotons(Texture->Finish, interior, tmpIPoint, ray, TopNormal, rawnormal, LightCol, New_Weight, ticket);
	}

	// Shoot reflected photons.

	// If total internal reflection occured all reflections using
	// TopNormal are skipped.

	// only really do reflection if...
	//  (1) We have already reached the target, -and-
	//  (2) (a) photon reflection is turned on for the object, and not explicitly turned off for the light, -or-
	//      (b) photon reflection is turned on for the light, and not explicitly turned off for the object
	doReflection = doReflection &&
	               ( ( ( Test_Flag(isect.Object, PH_RFL_ON_FLAG) && !(threadData->photonSourceLight->Flags & PH_RFL_OFF_FLAG) ) ||
	                   ( !Test_Flag(isect.Object, PH_RFL_OFF_FLAG) && (threadData->photonSourceLight->Flags & PH_RFL_ON_FLAG) ) ) &&
	                 !threadData->passThruThis );

	if(doReflection && (qualityFlags & Q_REFLECT))
	{
		Layer = Texture;
		for (i = 0; i < layer_number; i++)
		{
			if ((!TIR_occured) ||
			    (fabs(TopNormal[X]-(*listWNRX)[i].normal[X]) > EPSILON) ||
			    (fabs(TopNormal[Y]-(*listWNRX)[i].normal[Y]) > EPSILON) ||
			    (fabs(TopNormal[Z]-(*listWNRX)[i].normal[Z]) > EPSILON))
			{
				if (!(*listWNRX)[i].reflec.isZero())
				{
					// Added by MBP for metallic reflection
					TmpCol.red()   = LightCol.red();
					TmpCol.green() = LightCol.green();
					TmpCol.blue()  = LightCol.blue();

					if ((*listWNRX)[i].reflex != 1.0)
					{
						TmpCol.red()   = (*listWNRX)[i].reflec.red()   * pow(TmpCol.red(),   (*listWNRX)[i].reflex);
						TmpCol.green() = (*listWNRX)[i].reflec.green() * pow(TmpCol.green(), (*listWNRX)[i].reflex);
						TmpCol.blue()  = (*listWNRX)[i].reflec.blue()  * pow(TmpCol.blue(),  (*listWNRX)[i].reflex);
					}
					else
					{
						TmpCol.red()   = (*listWNRX)[i].reflec.red()   * TmpCol.red();
						TmpCol.green() = (*listWNRX)[i].reflec.green() * TmpCol.green();
						TmpCol.blue()  = (*listWNRX)[i].reflec.blue()  * TmpCol.blue();
					}

					TempWeight = (*listWNRX)[i].weight * max3((*listWNRX)[i].reflec.red(), (*listWNRX)[i].reflec.green(), (*listWNRX)[i].reflec.blue());

					Vector3d tmpIPoint(isect.IPoint);

					ComputeReflection(Layer->Finish, tmpIPoint, ray, LayNormal, rawnormal, TmpCol, TempWeight, ticket);
				}
			}

			// if global photons, the stop after first layer
			if (threadData->photonTargetObject==NULL)
				break;

			Layer = (TEXTURE *)Layer->Next;
		}
	}

	// now reset the depth!
	threadData->photonDepth -= isect.Depth;
}


bool PhotonTrace::ComputeRefractionForPhotons(const FINISH* finish, Interior *interior, const Vector3d& ipoint, const Ray& ray, const Vector3d& normal, const Vector3d& rawnormal, Colour& colour, COLC weight, Trace::TraceTicket& ticket)
{
	Ray nray(ray);
	Vector3d localnormal;
	DBL n, ior, dispersion;
	unsigned int dispersionelements = interior->Disp_NElems;
	bool havedispersion = (dispersionelements > 0);

	nray.SetFlags(Ray::RefractionRay, ray);

	// Set up new ray.
	Assign_Vector(nray.Origin, *ipoint);

	// Get ratio of iors depending on the interiors the ray is traversing.

	// Note:
	// For the purpose of reflection, the space occupied by "nested" objects is considered to be "outside" the containing objects,
	// i.e. when encountering (A (B B) A) we pretend that it's (A A|B B|A A).
	// (Here "(X" and "X)" denote the entering and leaving of object X, and "X|Y" denotes an interface between objects X and Y.)
	// In case of overlapping objects, the intersecting region is considered to be part of whatever object is encountered last,
	// i.e. when encountering (A (B A) B) we pretend that it's (A A|B B|B B).

	if(ray.GetInteriors().empty())
	{
		// The ray is entering from the atmosphere.
		nray.AppendInterior(interior);

		ior = sceneData->atmosphereIOR / interior->IOR;
		if(havedispersion == true)
			dispersion = sceneData->atmosphereDispersion / interior->Dispersion;
	}
	else
	{
		// The ray is currently inside an object.
		if(interior == nray.GetInteriors().back()) // The ray is leaving the "innermost" object
		{
			nray.RemoveInterior(interior);
			if(nray.GetInteriors().empty())
			{
				// The ray is leaving into the atmosphere
				ior = interior->IOR / sceneData->atmosphereIOR;
				if(havedispersion == true)
					dispersion = interior->Dispersion / sceneData->atmosphereDispersion;
			}
			else
			{
				// The ray is leaving into another object, i.e. (A (B B) ...
				// For the purpose of refraction, pretend that we weren't inside that other object,
				// i.e. pretend that we didn't encounter (A (B B) ... but (A A|B B|A ...
				ior = interior->IOR / nray.GetInteriors().back()->IOR;
				if(havedispersion == true)
				{
					dispersion = interior->Dispersion / nray.GetInteriors().back()->Dispersion;
					dispersionelements = max(dispersionelements, (unsigned int)(nray.GetInteriors().back()->Disp_NElems));
				}
			}
		}
		else if(nray.RemoveInterior(interior) == true) // The ray is leaving the intersection of overlapping objects, i.e. (A (B A) ...
		{
			// For the purpose of refraction, pretend that we had already left the other member of the intersection when we entered the overlap,
			// i.e. pretend that we didn't encounter (A (B A) ... but (A A|B B|B ...
			ior = 1.0;
			dispersion = 1.0;
		}
		else
		{
			// The ray is entering a new object.
			// For the purpose of refraction, pretend that we're leaving any containing objects,
			// i.e. pretend that we didn't encounter (A (B ... but (A A|B ...
			ior = nray.GetInteriors().back()->IOR / interior->IOR;
			if(havedispersion == true)
				dispersion = nray.GetInteriors().back()->Dispersion / interior->Dispersion;

			nray.AppendInterior(interior);
		}
	}

	// Do the two mediums traversed have the same indices of refraction?
	if((fabs(ior - 1.0) < EPSILON) && (fabs(dispersion - 1.0) < EPSILON))
	{
		// Only transmit the ray.
		Assign_Vector(nray.Direction, ray.Direction);
		// Trace a transmitted ray.
		threadData->Stats()[Transmitted_Rays_Traced]++;

		// photon:
		//  added this block
		//  changed 2nd variable in next line from color to lc
		//  added return false
		Colour lc;
		RGBColour GFilCol = threadData->GFilCol;
		lc.red()   = colour.red()   * GFilCol.red();
		lc.green() = colour.green() * GFilCol.green();
		lc.blue()  = colour.blue()  * GFilCol.blue();

		TraceRay(nray, lc, weight, ticket, true);
		return false;
	}
	else
	{
		// Refract the ray.
		VDot(n, ray.Direction, *normal);

		if(n <= 0.0)
		{
			localnormal = normal;
			n = -n;
		}
		else
			localnormal = -normal;

		if(fabs (dispersion - 1.0) < EPSILON)
			return TraceRefractionRayForPhotons(finish, ipoint, ray, nray, ior, n, normal, rawnormal, localnormal, colour, weight, ticket);
		else if(ray.IsMonochromaticRay() == true)
			return TraceRefractionRayForPhotons(finish, ipoint, ray, nray, ray.GetSpectralBand().GetDispersionIOR(ior, dispersion), n, normal, rawnormal, localnormal, colour, weight, ticket);
		else
		{
			for(unsigned int i = 0; i < dispersionelements; i++)
			{
				SpectralBand spectralBand(i, dispersionelements);
				Colour tempcolour;
				tempcolour = Colour(RGBColour(colour) * spectralBand.GetHue() / DBL(dispersionelements));

				// NB setting the dispersion factor also causes the MonochromaticRay flag to be set
				nray.SetSpectralBand(spectralBand);

				(void)TraceRefractionRayForPhotons(finish, ipoint, ray, nray, spectralBand.GetDispersionIOR(ior, dispersion), n, normal, rawnormal, localnormal, tempcolour, weight, ticket);
			}
		}
	}

	return false;
}

bool PhotonTrace::TraceRefractionRayForPhotons(const FINISH* finish, const Vector3d& ipoint, const Ray& ray, Ray& nray, DBL ior, DBL n, const Vector3d& normal, const Vector3d& rawnormal, const Vector3d& localnormal, Colour& colour, COLC weight, Trace::TraceTicket& ticket)
{
	// Compute refrated ray direction using Heckbert's method.
	DBL t = 1.0 + Sqr(ior) * (Sqr(n) - 1.0);

	if(t < 0.0)
	{
		// Total internal reflection occures.
		threadData->Stats()[Internal_Reflected_Rays_Traced]++;
		ComputeReflection(finish, ipoint, ray, normal, rawnormal, colour, weight, ticket);

		return true;
	}

	t = ior * n - sqrt(t);

	VLinComb2(nray.Direction, ior, ray.Direction, t, *localnormal);

	// Trace a refracted ray.
	threadData->Stats()[Refracted_Rays_Traced]++;

	Colour lc;
	RGBColour GFilCol = threadData->GFilCol;
	lc.red()   = colour.red()   * GFilCol.red();
	lc.green() = colour.green() * GFilCol.green();
	lc.blue()  = colour.blue()  * GFilCol.blue();

	TraceRay(nray, lc, weight, ticket, false);

	return false;
}



/*****************************************************************************

  FUNCTION

  addSurfacePhoton()

  Adds a photon to the array of photons.

  Preconditions:
    InitBacktraceEverything() was called
    'Point' is the intersection point to store the photon
    'Origin' is the origin of the light ray
    'LightCol' is the color of the light propogated through the scene
    'RawNorm' is the raw normal of the surface intersected

  Postconditions:
    Another photon is allocated (by AllocatePhoton())
    The information passed in (as well as renderer->sceneData->photonSettings.photonDepth)
      is stored in the photon data structure.

******************************************************************************/

void PhotonTrace::addSurfacePhoton(const VECTOR Point, const VECTOR Origin, const RGBColour& LightCol)
{
	// TODO FIXME - this seems to have a lot in common with addMediaPhoton()
	Photon *Photon;
	RGBColour LightCol2;
	DBL Attenuation;
	VECTOR d;
	DBL d_len, phi, theta;
	PhotonMap *map;

	// first, compensate for POV's weird light attenuation
	LightSource *photonLight = threadData->photonSourceLight;
	if ((photonLight->Fade_Power > 0.0) && (fabs(photonLight->Fade_Distance) > EPSILON))
	{
		Attenuation = 2.0 / (1.0 + pow(threadData->photonDepth / photonLight->Fade_Distance, photonLight->Fade_Power));
	}
	else
		Attenuation = 1;

	LightCol2 = LightCol * (Attenuation * threadData->photonSpread*threadData->photonSpread);
	if(!photonLight->Parallel)
		LightCol2 *= (threadData->photonDepth*threadData->photonDepth);

	// if too dark, maybe we should stop here

#ifdef GLOBAL_PHOTONS
	if(threadData->photonObject==NULL)
	{
		map = &globalPhotonMap;
		threadData->Stats()[Number_Of_Global_Photons_Stored]++;
	}
	else
#endif
	{
		map = (threadData->surfacePhotonMap);
		threadData->Stats()[Number_Of_Photons_Stored]++;
	}


	// allocate the photon
	Photon = map->AllocatePhoton();

	// convert photon from three floats to 4 bytes
	colour2photonRgbe(Photon->colour, LightCol2);

	// store the location
	Assign_Vector(Photon->Loc, Point);

	// now determine rotation angles
	VSub(d,Origin, Point);
	VNormalizeEq(d);
	d_len = sqrt(d[X]*d[X]+d[Z]*d[Z]);

	phi = acos(d[X]/d_len);
	if (d[Z]<0) phi = -phi;

	theta = acos(d_len);
	if (d[Y]<0) theta = -theta;

	// cram these rotation angles into two signed bytes
	Photon->theta=(signed char)(theta*127.0/M_PI);
	Photon->phi=(signed char)(phi*127.0/M_PI);

}

PhotonMediaFunction::PhotonMediaFunction(shared_ptr<SceneData> sd, TraceThreadData *td, Trace *t, PhotonGatherer *pg) :
	MediaFunction(td, t, pg),
	sceneData(sd)
{
}

/*****************************************************************************

  FUNCTION

  addMediaPhoton()

  Adds a photon to the array of photons.

  Preconditions:
    InitBacktraceEverything() was called
    'Point' is the intersection point to store the photon
    'Origin' is the origin of the light ray
    'LightCol' is the color of the light propogated through the scene

  Postconditions:
    Another photon is allocated (by AllocatePhoton())
    The information passed in (as well as renderer->sceneData->photonSettings.photonDepth)
      is stored in the photon data structure.

******************************************************************************/

void PhotonMediaFunction::addMediaPhoton(const VECTOR Point, const VECTOR Origin, const RGBColour& LightCol, DBL depthDiff)
{
	// TODO FIXME - this seems to have a lot in common with addSurfacePhoton()
	Photon *Photon;
	RGBColour LightCol2;
	DBL Attenuation;
	VECTOR d;
	DBL d_len, phi, theta;

	// first, compensate for POV's weird light attenuation
	// TODO CLARIFY - what *exactly* does this compensate for (and how)??
	LightSource *photonLight = threadData->photonSourceLight;
	if ((photonLight->Fade_Power > 0.0) && (fabs(photonLight->Fade_Distance) > EPSILON))
		Attenuation = 2.0 / (1.0 + pow((threadData->photonDepth+depthDiff) / photonLight->Fade_Distance, photonLight->Fade_Power));
	else
		Attenuation = 1;

#if 0
	LightCol2 = LightCol * (sceneData->photonSettings.photonSpread*sceneData->photonSettings.photonSpread);
	if(!photonLight->Parallel)
		LightCol2 *= ( (sceneData->photonSettings.photonDepth+depthDiff)*(sceneData->photonSettings.photonDepth+depthDiff) * Attenuation );
#else
	LightCol2 = LightCol * (Attenuation * threadData->photonSpread*threadData->photonSpread);
	if(!photonLight->Parallel)
		LightCol2 *= ( (threadData->photonDepth+depthDiff) * (threadData->photonDepth+depthDiff) );
#endif

	// if too dark, maybe we should stop here


	// allocate the photon
	if(threadData->photonTargetObject==NULL) return;

	threadData->Stats()[Number_Of_Media_Photons_Stored]++;

	Photon = threadData->mediaPhotonMap->AllocatePhoton();

	// convert photon from three floats to 4 bytes
	colour2photonRgbe(Photon->colour, LightCol2);

	// store the location
	Assign_Vector(Photon->Loc, Point);

	// now determine rotation angles
	VSub(d,Origin, Point);
	VNormalizeEq(d);
	d_len = sqrt(d[X]*d[X]+d[Z]*d[Z]);

	phi = acos(d[X]/d_len);
	if (d[Z]<0) phi = -phi;

	theta = acos(d_len);
	if (d[Y]<0) theta = -theta;

	// cram these rotation angles into two signed bytes
	Photon->theta=(signed char)(theta*127.0/M_PI);
	Photon->phi=(signed char)(phi*127.0/M_PI);

}

void PhotonMediaFunction::ComputeMediaAndDepositPhotons(MediaVector& medias, const Ray& ray, const Intersection& isect, Colour& colour, Trace::TraceTicket& ticket)
{
	LightSourceEntryVector lights;
	LitIntervalVector litintervals;
	MediaIntervalVector mediaintervals;
	Media *IMedia;
	bool all_constant_and_light_ray = ray.IsShadowTestRay();  // is all the media constant?
	bool ignore_photons = true;
	bool use_extinction = false;
	bool use_scattering = false;
	int minSamples;
	DBL aa_threshold = HUGE_VAL;

	// Find media with the largest number of intervals.
	IMedia = medias.front();

	for(MediaVector::iterator i(medias.begin()); i != medias.end(); i++)
	{
		// find media with the most intervals
		if((*i)->Intervals > IMedia->Intervals)
			IMedia = (*i);

		// find smallest AA_Threshold
		if((*i)->AA_Threshold < aa_threshold)
			aa_threshold = (*i)->AA_Threshold;

		// do not ignore photons if at least one media wants photons
		ignore_photons = ignore_photons && (*i)->ignore_photons;

		// use extinction if at leeast one media wants extinction
		use_extinction = use_extinction || (*i)->use_extinction;

		// use scattering if at leeast one media wants scattering
		use_scattering = use_scattering || (*i)->use_scattering;

		// NK fast light_ray media calculation for constant media
		if((*i)->Density)
			all_constant_and_light_ray = all_constant_and_light_ray && ((*i)->Density->Type == PLAIN_PATTERN);
	}

	// If this is a light ray and no extinction is used we can return.
	if((!use_extinction)) // TODO FIXME - this condition implies that when no extinction is used, no photons are deposited, which Nathan thinks is a bug [trf] September 5th, 2005
		return;

	// Prepare the Monte Carlo integration along the ray from P0 to P1.
	// for photon trace, this is always a light ray, so always do this
	ComputeMediaLightInterval(lights, litintervals, ray, isect);

	if(litintervals.empty())
		litintervals.push_back(LitInterval(false, 0.0, isect.Depth, 0, 0));

	// Set up sampling intervals (makes sure we will always have enough intervals)
	ComputeMediaSampleInterval(litintervals, mediaintervals, IMedia);

	if(mediaintervals.front().s0 > 0.0)
		mediaintervals.insert(mediaintervals.begin(),
		                      MediaInterval(false, 0,
		                                    0.0,
		                                    mediaintervals.front().s0,
		                                    mediaintervals.front().s0,
		                                    0, 0));
	if(mediaintervals.back().s1 < isect.Depth)
		mediaintervals.push_back(MediaInterval(false, 0,
		                                       mediaintervals.back().s1,
		                                       isect.Depth,
		                                       isect.Depth - mediaintervals.back().s1,
		                                       0, 0));

	minSamples = IMedia->Min_Samples;

	// Sample all intervals.
	// NOTE: We probably should change this to use only one interval
	//if((IMedia->Sample_Method == 3) && !all_constant_and_light_ray) //  adaptive sampling
	//  ComputeMediaAdaptiveSampling(medias, lights, mediaintervals, ray, IMedia, aa_threshold, minSamples, ignore_photons, use_scattering, false);
	//else
	DepositMediaPhotons(colour, medias, lights, mediaintervals, ray, minSamples, ignore_photons, use_scattering, all_constant_and_light_ray, ticket);
}

void PhotonMediaFunction::DepositMediaPhotons(Colour& colour, MediaVector& medias, LightSourceEntryVector& lights, MediaIntervalVector& mediaintervals,
                                              const Ray& ray, int minsamples, bool ignore_photons, bool use_scattering, bool all_constant_and_light_ray, Trace::TraceTicket& ticket)
{
	int j;
	RGBColour Od;
	DBL d0;
	RGBColour C0;
	RGBColour od0;
	RGBColour PhotonColour;

	Od = RGBColour(colour);

	for(MediaIntervalVector::iterator i(mediaintervals.begin()); i != mediaintervals.end(); i++)
	{
		DBL mediaSpacingFactor;

		if(!threadData->photonSourceLight->Parallel)
		{
			minsamples=(int)
				((*i).ds /
				(threadData->photonSpread *
				threadData->photonDepth *
				sceneData->photonSettings.mediaSpacingFactor));
		}
		else
		{
			minsamples=(int)
				((*i).ds /
				(threadData->photonSpread *
				sceneData->photonSettings.mediaSpacingFactor));
		}
		if (minsamples<=sceneData->photonSettings.maxMediaSteps)
		{
			// all's well
			mediaSpacingFactor = sceneData->photonSettings.mediaSpacingFactor;
		}
		else
		{
			// too many steps - use fewer steps and a bigger spacing factor
			minsamples = sceneData->photonSettings.maxMediaSteps;
			if(!threadData->photonSourceLight->Parallel)
			{
				mediaSpacingFactor =
				((*i).ds /
				(threadData->photonSpread *
				threadData->photonDepth *
				minsamples));
			}
			else
			{
				mediaSpacingFactor =
				((*i).ds /
				(threadData->photonSpread *
				minsamples));
			}
		}
		// Sample current interval.

		threadData->Stats()[Media_Intervals]++;

		for(j = 0; j < minsamples; j++)
		{
			d0 = (j + 0.5 + randomNumberGenerator()*sceneData->photonSettings.jitter - 0.5*sceneData->photonSettings.jitter) / minsamples;
			ComputeOneMediaSample(medias, lights, *i, ray, d0, C0, od0, 2 /* use method 2 */, ignore_photons, use_scattering, true, ticket);

			if (use_scattering && !ignore_photons)
			{
				if(!threadData->photonSourceLight->Parallel)
				{
					PhotonColour = Od * ( mediaSpacingFactor *
					                      threadData->photonSpread *
					                      (threadData->photonDepth+d0*(*i).ds+(*i).s0) );
				}
				else
				{
					PhotonColour = Od * ( mediaSpacingFactor *
					                      threadData->photonSpread );
				}

				//Od[0] = colour[0]*exp(-(*i).od[0]/(minsamples*2));
				//Od[1] = colour[1]*exp(-(*i).od[1]/(minsamples*2));
				//Od[2] = colour[2]*exp(-(*i).od[2]/(minsamples*2));

				VECTOR TempPoint;
				VEvaluateRay(TempPoint, ray.Origin, d0*(*i).ds+(*i).s0, ray.Direction);

				addMediaPhoton(TempPoint, ray.Origin, PhotonColour, d0*(*i).ds+(*i).s0);
			}
		}
	}

	// Sum the influences of all intervals.
	Od.clear();

	for(MediaIntervalVector::iterator i(mediaintervals.begin()); i != mediaintervals.end(); i++)
	{
		// Add optical depth of current interval.

		Od += (*i).od / (DBL)(*i).samples;
	}

	// Add contribution estimated for the participating media.
	colour[0] *= exp(-Od[0]);
	colour[1] *= exp(-Od[1]);
	colour[2] *= exp(-Od[2]);
}

/*****************************************************************************

 FUNCTION

   InitBacktraceEverything()

   Allocates memory.
   Initializes all photon mapping stuff.
   Does not create the photon map.

   Preconditions: InitBacktraceEverything() not yet called
                    or
                  both InitBacktraceEverything() and FreeBacktraceEverything() called

   Postconditions:
      If photonSettings.photonsEnabled is true, then
        memory for photon mapping is allocated.
      else
        nothing is done

******************************************************************************/
PhotonMap::PhotonMap()
{
	minGatherRad = 0.0;

	// defaults
	minGatherRadMult=1.0;
	gatherRadStep=0.5;
	gatherNumSteps=2;

	// memory initialization
	int k;

	// allocate the base array
	numPhotons = 0;
	numBlocks = INITIAL_BASE_ARRAY_SIZE;
	head = (PhotonBlock *)POV_MALLOC(sizeof(PhotonBlock *)*INITIAL_BASE_ARRAY_SIZE, "photons");

	// zero the array
	for(k=0; k<numBlocks; k++)
		head[k] = NULL;
}

SinCosOptimizations::SinCosOptimizations()
{
	int i;
	double theta;

	// create the sin/cos arrays for speed
	// range is -127..+127  =>  0..254
	sinTheta = (DBL *)POV_MALLOC(sizeof(DBL)*255, "Photon Map Info");
	cosTheta = (DBL *)POV_MALLOC(sizeof(DBL)*255, "Photon Map Info");
	for(i=0; i<255; i++)
	{
		theta = (double)(i-127)*M_PI/127.0;
		sinTheta[i] = sin(theta);
		cosTheta[i] = cos(theta);
	}
}

SinCosOptimizations::~SinCosOptimizations()
{
	if (sinTheta)
		POV_FREE(sinTheta);
	sinTheta = NULL;

	if (cosTheta)
		POV_FREE(cosTheta);
	cosTheta = NULL;
}


/*****************************************************************************

 FUNCTION

  AllocatePhoton(PhotonMap *map)
    allocates a photon

    Photons are allocated in blocks.  map->head is a
    dynamically-created array of these blocks.  The blocks themselves
    are allocated as they are needed.

  Preconditions:
    InitBacktraceEverything was called

  Postconditions:
    Marks another photon as allocated (and allocates another block of
    photons if necessary).
    Returns a pointer to the new photon.
    This will be the next available photon in array.

******************************************************************************/

Photon* PhotonMap::AllocatePhoton()
{
	// mutex would be needed if we were allocating photons into the same map
	// from different threads...
	// but we have a different map for each thread, so there's no danger there...
	//mutex::scoped_lock lock(allocatePhotonMutex);
	int i,j,k;

	// array mapping funciton
	
	// !!!!!!!!!!! warning
	// This code does the same function as the macro PHOTON_AMF
	// It is done here separatly instead of using the macro for
	// speed reasons (to avoid duplicate operations).  If the
	// macro is changed, this MUST ALSO BE CHANGED!
	i=(this->numPhotons & PHOTON_BLOCK_MASK);
	j=(this->numPhotons >> (PHOTON_BLOCK_POWER));

	// new photon
	this->numPhotons++;

	if(j == this->numBlocks)
	{
		// the base array is too small, we need to reallocate it
		Photon **newMap;
		newMap = (Photon **)POV_MALLOC(sizeof(Photon *)*this->numBlocks*2, "photons");
		this->numBlocks*=2;

		// copy entries
		for(k=0; k<j; k++)
			newMap[k] = this->head[k];

		// set new entries to zero
		for(k=j; k<this->numBlocks; k++)
			newMap[k] = NULL;

		// free old map and put the new map in place
		POV_FREE(this->head);
		this->head = newMap;
	}

	if(this->head[j] == NULL)
		// allocate a new block of photons
		this->head[j] = (Photon *)POV_MALLOC(sizeof(Photon)*PHOTON_BLOCK_SIZE, "photons");

	return &(this->head[j][i]);
}

/*
Merge the parameter photon map into this photon map.
"Delete" the contents of the parameter photon map after
the merge is complete.
*/
void PhotonMap::mergeMap(PhotonMap* map)
{
	int thisi = ((this->numPhotons) & PHOTON_BLOCK_MASK);
	int thisj = ((this->numPhotons) >> (PHOTON_BLOCK_POWER));

	int mapi = ((map->numPhotons) & (PHOTON_BLOCK_MASK));
	int mapj = ((map->numPhotons) >> (PHOTON_BLOCK_POWER));

	int blocksNeeded = thisj+mapj+2;

	// increase block count if necessary
	if(blocksNeeded > this->numBlocks)
	{
		// the base array is too small, we need to reallocate it
		Photon **newMap;
		newMap = (Photon **)POV_MALLOC(sizeof(Photon *)*blocksNeeded, "photons");
		this->numBlocks = blocksNeeded;

		int k;

		// copy entries
		for(k=0; k<=thisj; k++)
			newMap[k] = this->head[k];

		// set new entries to zero
		for(k=thisj+1; k<this->numBlocks; k++)
			newMap[k] = NULL;

		// free old map and put the new map in place
		POV_FREE(this->head);
		this->head = newMap;
	}

	// backup the pointer to the last block.
	Photon* lastBlock = this->head[thisj];

	int j;
	for(j=0; j<mapj; j++)
	{
		this->head[thisj] = map->head[j];
		thisj++;
	}
	this->head[thisj] = lastBlock;

	if(map->head[mapj]!=NULL)
	{
		if(this->head[thisj]==NULL)
			this->head[thisj] = (Photon *)POV_MALLOC(sizeof(Photon)*PHOTON_BLOCK_SIZE, "photons");

		int i;
		for(i=0; thisi<PHOTON_BLOCK_SIZE && i<mapi; i++,thisi++)
		{
			this->head[thisj][thisi] = map->head[mapj][i];
		}
		// continue in a new block if necessary
		if(i<mapi)
		{
			thisj++;
			this->head[thisj] = (Photon *)POV_MALLOC(sizeof(Photon)*PHOTON_BLOCK_SIZE, "photons");
			thisi=0;
			for(/*nothing*/;i<mapi;i++,thisi++)
			{
				this->head[thisj][thisi] = map->head[mapj][i];
			}
		}
		POV_FREE(map->head[mapj]);
	}

	// pretented that the passed-in map is already freed, so that we don't
	// de-allocate the memory when it is deconstructued (since we're using
	// its blocks in this map)
	map->head = NULL;

	this->numPhotons = this->numPhotons + map->numPhotons;
}



/*****************************************************************************

 FUNCTION

   InitPhotonMemory()

  Initializes photon memory.
  Must only be called by InitBacktraceEverything().

******************************************************************************/

/*****************************************************************************

 FUNCTION

	FreePhotonMemory()

	Frees all allocated blocks and the base array.
	Must be called only by FreeBacktraceEverything()

******************************************************************************/

PhotonMap::~PhotonMap()
{
	int j;

	// if already freed then stop now
	if (head==NULL)
		return;

	// free all non-NULL arrays
	for(j=0; j<numBlocks; j++)
	{
		if(head[j] != NULL)
		{
			POV_FREE(head[j]);
		}
	}

	// free the base array
	POV_FREE(head);
	head = NULL;
}






void ShootingDirection::recomputeForAreaLight(Ray& ray, int area_x, int area_y)
{
	// we need to make new up, left, and toctr vectors so we can
	// do proper rotations of theta and phi about toctr.  The
	// ray's initial point and ending points are both jittered to
	// produce the area-light effect.
	DBL Jitter_u, Jitter_v, ScaleFactor;
	VECTOR NewAxis1, NewAxis2;

	/*
	Jitter_u = (int)(FRAND()*Light->Area_Size1);
	Jitter_v = (int)(FRAND()*Light->Area_Size2);
	*/
	Jitter_u = area_x; //+(0.5*FRAND() - 0.25);
	Jitter_v = area_y; //+(0.5*FRAND() - 0.25);

	if (light->Area_Size1 > 1)
	{
		ScaleFactor = Jitter_u/(DBL)(light->Area_Size1 - 1) - 0.5;
		VScale (NewAxis1, light->Axis1, ScaleFactor);
	}
	else
	{
		Make_Vector(NewAxis1, 0.0, 0.0, 0.0);
	}

	if (light->Area_Size2 > 1)
	{
		ScaleFactor = Jitter_v/(DBL)(light->Area_Size2 - 1) - 0.5;
		VScale (NewAxis2, light->Axis2, ScaleFactor);
	}
	else
	{
		Make_Vector(NewAxis2, 0.0, 0.0, 0.0);
	}

	// need a new toctr & left
	VAddEq(ray.Origin, NewAxis1);
	VAddEq(ray.Origin, NewAxis2);

	VSub(toctr, ctr, ray.Origin);
	VLength(dist, toctr);

	VNormalizeEq(toctr);
	if ( fabs(fabs(toctr[Z])- 1.) < .1 )
	{
		// too close to vertical for comfort, so use cross product with horizon
		up[X] = 0.; up[Y] = 1.; up[Z] = 0.;
	}
	else
	{
		up[X] = 0.; up[Y] = 0.; up[Z] = 1.;
	}
	VCross(left, toctr, up);  VNormalizeEq(left);

	if (fabs(dist)<EPSILON)
	{
		Make_Vector(up, 1,0,0);
		Make_Vector(left, 0,1,0);
		Make_Vector(toctr, 0,0,1);
	}
}

void ShootingDirection::compute()
{
	// find bounding sphere based on bounding box
	ctr[X] = target->BBox.Lower_Left[X] + target->BBox.Lengths[X] / 2.0;
	ctr[Y] = target->BBox.Lower_Left[Y] + target->BBox.Lengths[Y] / 2.0;
	ctr[Z] = target->BBox.Lower_Left[Z] + target->BBox.Lengths[Z] / 2.0;
	VSub(v, ctr,target->BBox.Lower_Left);
	VLength(rad, v);

	// find direction from object to bounding sphere
	VSub(toctr, ctr, light->Center);
	VLength(dist, toctr);

	VNormalizeEq(toctr);
	if ( fabs(fabs(toctr[Z])- 1.) < .1 )
	{
		// too close to vertical for comfort, so use cross product with horizon
		up[X] = 0.; up[Y] = 1.; up[Z] = 0.;
	}
	else
	{
		up[X] = 0.; up[Y] = 0.; up[Z] = 1.;
	}

	// find "left", which is vector perpendicular to toctr
	if(light->Parallel)
	{
		// for parallel lights, left is actually perpendicular to the direction of the
		// light source
		VCross(left, light->Direction, up);  VNormalizeEq(left);
	}
	else
	{
		VCross(left, toctr, up);  VNormalizeEq(left);
	}


	/*
	light   dist    ctr
	* ------------- +
	    --__       /
	        --__  /  rad
	            --
	*/
}


/* ====================================================================== */
/* ====================================================================== */
/*                              KD - TREE                                 */
/* ====================================================================== */
/* ====================================================================== */

/*****************************************************************************

  FUNCTION

  swapPhotons

  swaps two photons

  Precondition:
    photon memory initialized
    'a' and 'b' are indexes within the range of photons in the map
      (NO ERROR CHECKING IS DONE)

  Postconditions:
    the photons indexed by 'a' and 'b' are swapped

*****************************************************************************/

void PhotonMap::swapPhotons(int a, int b)
{
	int ai,aj,bi,bj;
	Photon tmp;

	// !!!!!!!!!!! warning
	// This code does the same function as the macro PHOTON_AMF
	// It is done here separatly instead of using the macro for
	// speed reasons (to avoid duplicate operations).  If the
	// macro is changed, this MUST ALSO BE CHANGED!
	ai = a & PHOTON_BLOCK_MASK;
	aj = a >> PHOTON_BLOCK_POWER;
	bi = b & PHOTON_BLOCK_MASK;
	bj = b >> PHOTON_BLOCK_POWER;

	tmp = this->head[aj][ai];
	this->head[aj][ai] = this->head[bj][bi];
	this->head[bj][bi] = tmp;
}

/*****************************************************************************

  FUNCTION

  insertSort
  (modified from Data Structures textbook)

  Preconditions:
    photon memory initialized
    'start' is the index of the first photon
    'end' is the index of the last photon
    'd' is the dimension to sort on (X, Y, or Z)

  Postconditions:
    photons from 'start' to 'end' in the map are sorted in
    ascending order on dimension d
******************************************************************************/
void PhotonMap::insertSort(int start, int end, int d)
{
	int j,k;
	Photon tmp;

	for(k=end-1; k>=start; k--)
	{
		j=k+1;
		tmp = PHOTON_AMF(this->head, k);
		while ( (tmp.Loc[d] > PHOTON_AMF(this->head,j).Loc[d]) )
		{
			PHOTON_AMF(this->head,j-1) = PHOTON_AMF(this->head,j);
			j++;
			if (j>end) break;
		}
		PHOTON_AMF(this->head,j-1) = tmp;
	}
}

/*****************************************************************************

  FUNCTION

  quickSortRec
  (modified from Data Structures textbook)

  Recursive part of the quicksort routine
  This does not sort all the way.  once this is done, insertSort
  should be called to finish the sorting process!

  Preconditions:
    photon memory initialized
    'left' is the index of the first photon
    'right' is the index of the last photon
    'd' is the dimension to sort on (X, Y, or Z)

  Postconditions:
    photons from 'left' to 'right' in the map are MOSTLY sorted in
    ascending order on dimension d
******************************************************************************/
void PhotonMap::quickSortRec(int left, int right, int d)
{
	int j,k;
	if(left<right)
	{
		swapPhotons(((left+right)>>1), left+1);
		if(PHOTON_AMF(this->head,left+1).Loc[d] > PHOTON_AMF(this->head,right).Loc[d])
			swapPhotons(left+1,right);
		if(PHOTON_AMF(this->head,left).Loc[d] > PHOTON_AMF(this->head,right).Loc[d])
			swapPhotons(left,right);
		if(PHOTON_AMF(this->head,left+1).Loc[d] > PHOTON_AMF(this->head,left).Loc[d])
			swapPhotons(left+1,left);

		j=left+1; k=right;
		while(j<=k)
		{
			for(j++; ((j<=right)&&(PHOTON_AMF(this->head,j).Loc[d]<PHOTON_AMF(this->head,left).Loc[d])); j++) { }
			for(k--; ((k>=left)&&(PHOTON_AMF(this->head,k).Loc[d]>PHOTON_AMF(this->head,left).Loc[d])); k--) { }

			if(j<k)
				swapPhotons(j,k);
		}

		swapPhotons(left,k);
		if(k-left > 10)
		{
			quickSortRec(left,k-1,d);
		}
		if(right-k > 10)
		{
			quickSortRec(k+1,right,d);
		}
		// leave the rest for insertSort
	}
}

/*****************************************************************************

  FUNCTION

  halfSortRec
  (modified quicksort algorithm)

  Recursive part of the quicksort routine, but it only does half
  the quicksort.  It only recurses one branch - the branch that contains
  the midpoint (median).

  Preconditions:
    photon memory initialized
    'left' is the index of the first photon
    'right' is the index of the last photon
    'd' is the dimension to sort on (X, Y, or Z)
    'mid' is the index where the median will end up

  Postconditions:
    the photon at the midpoint (mid) is the median of the photons
    when sorted on dimention d.
******************************************************************************/
void PhotonMap::halfSortRec(int left, int right, int d, int mid)
{
	int j,k;
	if(left<right)
	{
		swapPhotons(((left+right)>>1), left+1);
		if(PHOTON_AMF(this->head,left+1).Loc[d] > PHOTON_AMF(this->head,right).Loc[d])
			swapPhotons(left+1,right);
		if(PHOTON_AMF(this->head,left).Loc[d] > PHOTON_AMF(this->head,right).Loc[d])
			swapPhotons(left,right);
		if(PHOTON_AMF(this->head,left+1).Loc[d] > PHOTON_AMF(this->head,left).Loc[d])
			swapPhotons(left+1,left);

		j=left+1; k=right;
		while(j<=k)
		{
			for(j++; ((j<=right)&&(PHOTON_AMF(this->head,j).Loc[d]<PHOTON_AMF(this->head,left).Loc[d])); j++) { }
			for(k--; ((k>=left)&&(PHOTON_AMF(this->head,k).Loc[d]>PHOTON_AMF(this->head,left).Loc[d])); k--) { }

			if(j<k)
				swapPhotons(j,k);
		}

		// put the pivot into its position
		swapPhotons(left,k);

		// only go down the side that contains the midpoint
		// don't do anything if the midpoint=k (the pivot, which is
		// now in the correct position
		if(k-left > 0 && (mid>=left) && (mid<k))
		{
			halfSortRec(left,k-1,d,mid);
		}
		else if(right-k > 0 && (mid>k) && (mid<=right))
		{
			halfSortRec(k+1,right,d,mid);
		}
	}
}

/*****************************************************************************

  FUNCTION

  sortAndSubdivide

  Finds the dimension with the greates range, sorts the photons on that
  dimension.  Then it recurses on the left and right halves (keeping
  the median photon as a pivot).  This produces a balanced kd-tree.

  Preconditions:
    photon memory initialized
    'start' is the index of the first photon
    'end' is the index of the last photon
    'sorted' is the dimension that was last sorted (so we don't sort again)

  Postconditions:
    photons from 'start' to 'end' in the map are in a valid kd-tree format
******************************************************************************/
void PhotonMap::sortAndSubdivide(int start, int end, int /*sorted*/)
{
	int i,j;             // counters
	SNGL_VECT min,max;   // min/max vectors for finding range
	int DimToUse;        // which dimesion has the greatest range
	int mid;             // index of median (middle)
	int len;             // length of the array we're sorting

	if (end==start)
	{
		PHOTON_AMF(this->head, start).info = 0;
		return;
	}

	if(end<start) return;

	// loop and find greatest range

	Make_Vector(min, 1/EPSILON, 1/EPSILON, 1/EPSILON);
	Make_Vector(max, -1/EPSILON, -1/EPSILON, -1/EPSILON);

	for(i=start; i<=end; i++)
	{
		for(j=X; j<=Z; j++)
		{
			Photon *ph = &(PHOTON_AMF(this->head,i));

			if (ph->Loc[j] < min[j])
				min[j]=ph->Loc[j];
			if (ph->Loc[j] > max[j])
				max[j]=ph->Loc[j];
		}
	}

	// choose which dimension to use
	DimToUse = X;
	if((max[Y]-min[Y])>(max[DimToUse]-min[DimToUse]))
		DimToUse=Y;
	if((max[Z]-min[Z])>(max[DimToUse]-min[DimToUse]))
		DimToUse=Z;

	// find midpoint
	mid = (end+start)>>1;

	// use half of a quicksort to find the median
	len = end-start;
	if (len>=2)
	{
		// only display status every so often
		if(len > 1000)
		{
			//gPhotonStat_end = end;
//			Send_ProgressUpdate(PROGRESS_SORTING_PHOTONS);
		}

		halfSortRec(start, end, DimToUse, mid);
		//don't do this - but why? quickSortRec(start, end, DimToUse);
	}

	// set DimToUse for the midpoint
	PHOTON_AMF(this->head, mid).info = DimToUse;

	// now recurse to continue building the kd-tree
	sortAndSubdivide(start, mid - 1, DimToUse);
	sortAndSubdivide(mid + 1, end, DimToUse);
}

/*****************************************************************************

  FUNCTION

  buildTree

  Builds the kd-tree by calling sortAndSubdivide().

  Preconditions:
    photon memory initialized
    'map' is a pointer to a photon map containing an array of unsorted
         photons

  Postconditions:
    photons are in a valid kd-tree format
******************************************************************************/
void PhotonMap::buildTree()
{
// 	Send_Progress("Sorting photons", PROGRESS_SORTING_PHOTONS);
	sortAndSubdivide(0,this->numPhotons-1,X+Y+Z/*this is not X, Y, or Z*/);
}

/*****************************************************************************

  FUNCTION

  setGatherOptions

  determines gather options

  Preconditions:
    photon memory initialized
    'map' points to an already-built (and sorted) photon map
    'mediaMap' is true if 'map' contians media photons, and false if
         'map' contains surface photons

  Postconditions:
    gather gather options are set for this map
******************************************************************************/
void PhotonMap::setGatherOptions(ScenePhotonSettings &photonSettings, int mediaMap)
{
	DBL r;
	DBL density;
	VECTOR Point;
	int numToSample;
	int n,i,j;
	DBL mind,maxd,avgd;
	DBL sum,sum2;
	DBL saveDensity;
	//int greaterThan;
	int lessThan;

	// if user did not set minimum gather radius,
	// then we should calculate it
	if (this->minGatherRad <= 0.0)
	{
		// TODO FIXME - lots of magic numbers here!

		mind=10000000.0;
		maxd=avgd=sum=sum2=0.0;

		// use 5% of photons, min 100, max 10000
		numToSample = this->numPhotons/20;
		if (numToSample>1000) numToSample = 1000;
		if (numToSample<100) numToSample = 100;

		for(i=0; i<numToSample; i++)
		{
			j = rand() % this->numPhotons;

			Assign_Vector(Point,(PHOTON_AMF(this->head, j)).Loc);

			// TODO FIXME (this allocates then frees memory each time around the loop)
			PhotonGatherer gatherer(this, photonSettings);
			n=gatherer.gatherPhotons(Point, 10000000.0, &r, NULL, false);

			// sometimes, if we gather all (100) photons at a single point, the radius will
			// be zero, which will lead to an infinite density, which will mess up all the
			// calculations.  For now (until a better fix is found), just IGNORE any sample
			// points that have a zero radius.  This does throw off the averaging calculation
			// a little bit, because this density counts as zero... but that's OK... we can
			// live with that.... actually, we can use the current average
			if(r>0.0)
			{
				if(mediaMap)
					density = 3.0 * n / (4.0*M_PI*r*r*r); // should that be 4/3?
				else
					density = n / (M_PI*r*r);
			}
			else
			{
				//povwin::WIN32_DEBUG_FILE_OUTPUT("FOUND ZERO RADIUS AT: %lf, %lf, %lf\n",Point[0],Point[1],Point[2]);
				// somehow we ended up with infinite density at this point... that CAN'T be
				// right, so we'll use the current average as the density for this point.
				// that should be SAFE.
				density = sum / i;
			}

			//povwin::WIN32_DEBUG_FILE_OUTPUT("num, rad, density: %d, %lf, %lf\n",n,r,density);

			if (density>maxd) maxd=density;
			if (density<mind) mind=density;
			sum+=density;
			//povwin::WIN32_DEBUG_FILE_OUTPUT("sum %lf\n",sum);
			sum2+=density*density;
		}
		//povwin::WIN32_DEBUG_FILE_OUTPUT("sum %lf\n",sum);
		//povwin::WIN32_DEBUG_FILE_OUTPUT("numToSample %d\n",numToSample);
		avgd = sum/numToSample;
		//povwin::WIN32_DEBUG_FILE_OUTPUT("avgd %lf\n",avgd);

		// try using some standard deviation stuff instead of this
		saveDensity = avgd;
/*
		greaterThan = 0;
		for(i=0; i<numToSample; i++)
		{
			j = rand() % this->numPhotons;

			Assign_Vector(Point,(PHOTON_AMF(this->head, j)).Loc);

			n=gatherPhotons(Point, 10000000.0, &r, NULL, false, map);

			if(mediaMap)
				density = 3.0 * n / (4.0*M_PI*r*r*r); // should that be 4/3?
			else
				density = n / (M_PI*r*r);

			if (density>saveDensity)
				greaterThan++;
		}

		density = saveDensity * (DBL)greaterThan / numToSample;
*/
		density = saveDensity;

		if(mediaMap)
			// TODO FIXME - what's that 0.3333 back there? Is that supposed to be 1/3?
			this->minGatherRad = pow(3.0 * photonSettings.maxGatherCount / (density*M_PI*4.0), 0.3333);
		else
			this->minGatherRad = sqrt(photonSettings.maxGatherCount / (density*M_PI));

		//povwin::WIN32_DEBUG_FILE_OUTPUT("photonSettings.maxGatherCount %d\n",photonSettings.maxGatherCount);
		//povwin::WIN32_DEBUG_FILE_OUTPUT("this->minGatherRad %lf\n",this->minGatherRad);

		lessThan = 0;
		for(i=0; i<numToSample; i++)
		{
			j = rand() % this->numPhotons;

			Assign_Vector(Point,(PHOTON_AMF(this->head, j)).Loc);

			PhotonGatherer gatherer(this, photonSettings);
			n=gatherer.gatherPhotons(Point, this->minGatherRad, &r, NULL, false);

			if(r>0)
			{
				if(mediaMap)
					density = 3.0 * n / (4.0*M_PI*r*r*r); // should that be 4/3?
				else
					density = n / (M_PI*r*r);

				// count as "lessThan" if the density is below 70% of the average,
				// and if it is at least above 5% of the average.
				if (density<(saveDensity*0.7) && density>(saveDensity*0.05))
					lessThan++;
			}
			else
			{
				//povwin::WIN32_DEBUG_FILE_OUTPUT("FOUND ZERO RADIUS AT: %lf, %lf, %lf\n",Point[0],Point[1],Point[2]);
			}
		}

		//povwin::WIN32_DEBUG_FILE_OUTPUT("lessThan %d\n",lessThan);
		//povwin::WIN32_DEBUG_FILE_OUTPUT("numToSample %d\n",numToSample);

		//povwin::WIN32_DEBUG_FILE_OUTPUT("this->minGatherRad %lf\n",this->minGatherRad);
		// the 30.0 is a bit of a fudge-factor.
		this->minGatherRad*=(1.0+20.0*((DBL)lessThan/(numToSample)));
		//povwin::WIN32_DEBUG_FILE_OUTPUT("this->minGatherRad %lf\n",this->minGatherRad);

	}

	// Now apply the user-defined multiplier, so that the user can tell
	// POV to take shortcuts which will improve speed at the expensive of
	// quality.  Alternatively, the user could tell POV to use a bigger
	// radius to improve quality.
	this->minGatherRad *= this->minGatherRadMult;
	//povwin::WIN32_DEBUG_FILE_OUTPUT("this->minGatherRad %lf\n",this->minGatherRad);

	if(mediaMap)
	{
		// double the radius if it is a media map
		// TODO CLARIFY - why?
		this->minGatherRad *= 2;
	}

	// always do this! - use 6 times the area
	this->gatherRadStep = this->minGatherRad*2;

	// somehow we could automatically determine the number of steps
}


/**************************************************************

  =========== PRIORITY QUEUES ===============
  Each priority stores its data in the static variables below (such as
  numfound_s) and in the global variables

  Preconditions:

  static DBL size_sq_s;   - maximum radius given squared
  static DBL Size_s;      - radius
  static DBL dmax_s;      - square of radius used so far
  static int TargetNum_s; - target number
  static DBL *pt_s;       - center point
  static numfound_s;      - number of photons in priority queue

  these must be allocated:
    renderer->sceneData->photonSettings.photonGatherList - array of photons in priority queue
    renderer->sceneData->photonSettings.photonDistances  - corresponding priorities(distances)

  *Each priority queue has the following functions:

  function PQInsert(Photon *photon, DBL d)

    Inserts 'photon' into the priority queue with priority (distance
    from target point) 'd'.

  void PQDelMax()

    Removes the photon with the greates distance (highest priority)
    from the queue.

********************************************************************/

// try different priority queues

#define ORDERED   0
#define UNORDERED 1
#define HEAP      2

#define PRI_QUE HEAP

// -------------- ordered list implementation -----------------
#if (PRI_QUE == ORDERED)
static void PQInsert(Photon *photon, DBL d)
{
	int i,j;

	GetViewDataPtr()->Stats()[Priority_Queue_Add]++;
	// save this in order, remove maximum, save new dmax_s

	// store in array and shift - assumption is that we don't have
	// to shift often
	for (i=0; GetSceneData()->photonSettings.photonDistances[i]<d && i<(gatheredPhotons.numFound); i++);
	for (j=gatheredPhotons.numFound; j>i; j--)
	{
		GetSceneData()->photonSettings.photonGatherList[j] = GetSceneData()->photonSettings.photonGatherList[j-1];
		GetSceneData()->photonSettings.photonDistances[j] = GetSceneData()->photonSettings.photonDistances[j-1];
	}

	gatheredPhotons.numFound++;
	GetSceneData()->photonSettings.photonGatherList[i] = photon;
	GetSceneData()->photonSettings.photonDistances[i] = d;
	if (gatheredPhotons.numFound==TargetNum_s)
		dmax_s=GetSceneData()->photonSettings.photonDistances[gatheredPhotons.numFound-1];

}

static void PQDelMax()
{
	GetViewDataPtr()->Stats()[Priority_Queue_Remove]++;
	gatheredPhotons.numFound--;
}
#endif

// -------------- unordered list implementation -----------------
#if (PRI_QUE == UNORDERED)
static void PQInsert(Photon *photon, DBL d)
{
	GetViewDataPtr()->Stats()[Priority_Queue_Add]++;

	GetSceneData()->photonSettings.photonGatherList[gatheredPhotons.numFound] = photon;
	GetSceneData()->photonSettings.photonDistances[gatheredPhotons.numFound] = d;

	if (d>dmax_s)
		dmax_s=d;

	gatheredPhotons.numFound++;
}

static void PQDelMax()
{
	int i,max;

	GetViewDataPtr()->Stats()[Priority_Queue_Remove]++;

	max=0;
	// find max
	for(i=1; i<gatheredPhotons.numFound; i++)
		if (GetSceneData()->photonSettings.photonDistances[i]>GetSceneData()->photonSettings.photonDistances[max]) max = i;

	// remove it, shifting the photons
	for(i=max+1; i<gatheredPhotons.numFound; i++)
	{
		GetSceneData()->photonSettings.photonGatherList[i-1] = GetSceneData()->photonSettings.photonGatherList[i];
		GetSceneData()->photonSettings.photonDistances[i-1] = GetSceneData()->photonSettings.photonDistances[i];
	}

	gatheredPhotons.numFound--;

	// find a new dmax_s
	dmax_s=GetSceneData()->photonSettings.photonDistances[0];
	for(i=1; i<gatheredPhotons.numFound; i++)
		if (GetSceneData()->photonSettings.photonDistances[i]>dmax_s) dmax_s = GetSceneData()->photonSettings.photonDistances[i];
}
#endif

// -------------- heap implementation -----------------
// this is from Sejwick (spelling?)
#if (PRI_QUE == HEAP)

void PhotonGatherer::PQInsert(Photon *photon, DBL d)
{
	// TODO FIXME STATS threadData->Stats()[Priority_Queue_Add]++;
	DBL* Distances = gatheredPhotons.photonDistances;
	Photon** Photons = gatheredPhotons.photonGatherList;

	unsigned int k = ++gatheredPhotons.numFound;

	while (k > 1)
	{
		unsigned int half_k = k / 2;
		DBL d_half_k_m1 = Distances[half_k - 1];

		if (d < d_half_k_m1)
			break;

		Distances [k - 1] = d_half_k_m1;
		Photons[k - 1] = Photons[half_k - 1];

		k = half_k;
	}

	Distances [k - 1] = d;
	Photons[k - 1] = photon;
}

void PhotonGatherer::FullPQInsert(Photon *photon, DBL d)
{
	// TODO FIXME STATS threadData->Stats()[Priority_Queue_Remove]++;
	DBL* Distances = gatheredPhotons.photonDistances;
	Photon** Photons = gatheredPhotons.photonGatherList;

	int k = 1, k2 = 2;
	for (; k2 < gatheredPhotons.numFound; k = k2, k2 += k)
	{
		DBL d_k2_m1 = Distances[k2 - 1],
		    d_k2 = Distances[k2];

		if (d_k2 > d_k2_m1)
		{
			d_k2_m1 = d_k2;
			++k2;
		}

		if (!(d_k2_m1 > d))
			break;

		Distances [k - 1] = d_k2_m1;
		Photons[k - 1] = Photons[k2 - 1];
	}

	if (k2 == gatheredPhotons.numFound) {
		DBL d_k2_m1 = Distances[k2 - 1];
		if (d_k2_m1 > d) {
			Distances [k - 1] = d_k2_m1;
			Photons[k - 1] = Photons[k2 - 1];
			k = k2;
		}
	}

	Distances [k - 1] = d;
	Photons[k - 1] = photon;

	// find a new dmax_s
	dmax_s = Distances[0];
}

#endif

/*****************************************************************************

  FUNCTION

  gatherPhotonsRec()

  Recursive part of gatherPhotons
  Searches the kd-tree with range start..end (midpoint is pivot)
  
  Preconditions:
    same preconditions as priority queue functions
    static variable map_s points to the map to use
    'start' is the first photon in this range
    'end' is the last photon in this range

    the range 'start..end' must have been used in building photon map!!!

  Postconditions:
    photons within the range of start..end are added to the priority
    queue (photons may be delted from the queue to make room for photons
    of lower priority)
 
******************************************************************************/

void PhotonGatherer::gatherPhotonsRec(int start, int end)
{
	DBL delta;
	int DimToUse;
	DBL d,dx,dy,dz;
	int mid;
	Photon *photon;
	VECTOR ptToPhoton;
	DBL discFix;   // use disc(ellipsoid) for gathering instead of sphere

	// find midpoint
	mid = (end+start)>>1;
	photon = &(PHOTON_AMF(map->head, mid));

	DimToUse = photon->info;// & PH_MASK_XYZ;

	// check this photon

	// find distance from pt
	ptToPhoton[X] = - pt_s[X] + photon->Loc[X];
	ptToPhoton[Y] = - pt_s[Y] + photon->Loc[Y];
	ptToPhoton[Z] = - pt_s[Z] + photon->Loc[Z];
	// all distances are squared
	dx = ptToPhoton[X]*ptToPhoton[X];
	dy = ptToPhoton[Y]*ptToPhoton[Y];
	dz = ptToPhoton[Z]*ptToPhoton[Z];

	if (!(  ((dx>dmax_s) && ((DimToUse)==X)) ||
	        ((dy>dmax_s) && ((DimToUse)==Y)) ||
	        ((dz>dmax_s) && ((DimToUse)==Z)) ))
	{
		// it fits manhatten distance - maybe we can use this photon

		// find euclidian distance (squared)
		d = dx + dy + dz;

		// now fix this distance so that we gather using an ellipsoid
		// alligned with the surface normal instead of a sphere.  This
		// minimizes false bleeding of photons at sharp corners

		// dmax_s is square of radius of major axis
		// dmax_s/16 is  "   "   "     " minor  "    (1/6 of major axis)
		/*
		VDot(discFix,norm_s,ptToPhoton);
		discFix*=discFix*(dmax_s/1000.0-dmax_s); // TODO FIXME - magic number
		*/

		if (flattenFactor!=0.0)
		{
			VDot(discFix,norm_s,ptToPhoton);
			discFix = fabs(discFix);
			d += flattenFactor*(discFix)*d*16;
		}
		// this will add zero if on the plane, and will double distance from
		// point to photon if it is ptToPhoton is perpendicular to the surface

		if(d < dmax_s)
		{
			if (gatheredPhotons.numFound+1>TargetNum_s)
			{
				FullPQInsert(photon, d);
				sqrt_dmax_s = sqrt(dmax_s);
			}
			else
				PQInsert(photon, d);
		}
	}

	// now go left & right if appropriate - if going left or right goes out
	// the current range, then don't go that way.
	/*
	delta=pt_s[DimToUse]-photon->Loc[DimToUse];
	if(delta<0)
	{
		if (end>=mid+1) gatherPhotonsRec(start, mid - 1);
		if (delta*delta < dmax_s )
			if (mid-1>=start) gatherPhotonsRec(mid + 1, end);
	}
	else
	{
		if (mid-1>=start) gatherPhotonsRec(mid+1,end);
		if (delta*delta < dmax_s )
			if (end>=mid+1) gatherPhotonsRec(start, mid - 1);
	}
	*/
	delta=pt_s[DimToUse]-photon->Loc[DimToUse];
	if(delta<0)
	{
		// on left - go left first
		if (pt_s[DimToUse]-sqrt_dmax_s < photon->Loc[DimToUse])
		{
			if (mid-1>=start)
				gatherPhotonsRec(start, mid - 1);
		}
		if (pt_s[DimToUse]+sqrt_dmax_s > photon->Loc[DimToUse])
		{
			if(end>=mid+1)
				gatherPhotonsRec(mid + 1, end);
		}
	}
	else
	{
		// on right - go right first
		if (pt_s[DimToUse]+sqrt_dmax_s > photon->Loc[DimToUse])
		{
			if(end>=mid+1)
				gatherPhotonsRec(mid + 1, end);
		}
		if (pt_s[DimToUse]-sqrt_dmax_s < photon->Loc[DimToUse])
		{
			if (mid-1>=start)
				gatherPhotonsRec(start, mid - 1);
		}
	}
}

/*****************************************************************************

  FUNCTION

  gatherPhotons()

  gathers photons from the global photon map

  Preconditons:

    renderer->sceneData->photonSettings.photonGatherList and renderer->sceneData->photonSettings.photonDistances
      are allocated and are each maxGatherCount in length

    'Size' - maximum search radius
    'r' points to a double

    BuildPhotonMaps() has been called for this scene.

  Postconditions:

    *r is radius actually used for gathereing (maximum value is 'Size')
    renderer->sceneData->photonSettings.photonGatherList and renderer->sceneData->photonSettings.photonDistances
      contain the gathered photons
    returns number of photons gathered

******************************************************************************/

int PhotonGatherer::gatherPhotons(const VECTOR pt, DBL Size, DBL *r, const VECTOR norm, bool flatten)
{
	if (map->numPhotons<=0) return 0; // no crashes, please...

	// set the static variables
	gatheredPhotons.numFound=0;
	size_sq_s = Size*Size;
	dmax_s = size_sq_s;
	sqrt_dmax_s = Size;
	norm_s = norm;

	if(flatten)
	{
		flattenFactor = 1.0;
	}
	else
	{
		flattenFactor = 0.0;
	}

	Size_s = Size;
	TargetNum_s = photonSettings.maxGatherCount;
	pt_s = pt;

	// now search the kd-tree recursively
	gatherPhotonsRec(0, map->numPhotons-1);

	// set the radius variable
	*r = sqrt_dmax_s;

	if(gatheredPhotons.numFound>0)
	{
		gatheredPhotons.numFound++;
		gatheredPhotons.numFound--;
	}

	// return the number of photons found
	return(gatheredPhotons.numFound);
}

PhotonGatherer::PhotonGatherer(PhotonMap *map, ScenePhotonSettings& photonSettings): map(map),photonSettings(photonSettings),gatheredPhotons(photonSettings.maxGatherCount)
{
	gathered = false;
}

DBL PhotonGatherer::gatherPhotonsAdaptive(const VECTOR pt, const VECTOR norm, bool flatten)
{
	// set up initial radius
	int num;
	DBL radius;
	DBL Size = map->minGatherRad;
	DBL prevDensity, thisDensity;
	bool expanded = false;

	// first try at gathering
	num=gatherPhotons(pt, Size, &radius, norm, flatten);
	prevDensity = thisDensity = num / (radius*radius);
	if (prevDensity==0)
		// TODO FIXME - Magic Value
		prevDensity = 0.0000000000000001;  // avoid div-by-zero error

	// start looping if necessary
	int step=1;
	while(num<photonSettings.minGatherCount && step<map->gatherNumSteps)
	{
		step++;
		DBL tempr = 0;
		int tempn;

		// save out the current set in case we want to revert
		GatheredPhotons savedGatheredPhotons(photonSettings.maxGatherCount);
		savedGatheredPhotons.swapWith(this->gatheredPhotons);

		// increase the size
		Size+=map->gatherRadStep;

		// gather again, with the new size
		tempn=gatherPhotons(pt, Size, &tempr, norm, flatten);

		// compute the density of this search
		thisDensity = tempn / (tempr*tempr);

		/*
		this next line handles the adaptive search
		if
		the density change ((thisDensity-prevDensity)/prevDensity) is small enough
		    or
		this is the first time through (step==0)
		    or
		the number gathered is less than photonSettings.minExpandCount and greater than zero

		then
		use the color from this new gathering step and discard any previous
		color

		This adaptive search is explained my paper "Simulating Reflective and Refractive
		Caustics in POV-Ray Using a Photon Map" - May, 1999
		*/
		if(((thisDensity-prevDensity)/prevDensity < photonSettings.expandTolerance)
			|| (step==0)
			|| (tempn<photonSettings.minExpandCount && tempn>0))
		{
			// it passes the tests, so use the new color
			expanded = true;

			// save variables in case we loop again
			prevDensity = thisDensity;
			if (prevDensity==0)
				// TODO FIXME - Magic Value
				prevDensity = 0.0000000000000001;  // avoid div-by-zero error

			// keep
			radius = tempr;
			num = tempn;
		}
		else
		{
			// put the old gathered photons back
			savedGatheredPhotons.swapWith(this->gatheredPhotons);
			// we're done - break out of the loop
			break;
		}
	}
	// TODO FIXME STATS
	//if (expanded)
		//threadData->Stats()[Gather_Expanded_Count]++;

	gathered = true;
	alreadyGatheredRadius = radius;
	return radius;
}

/******************************************************************
stuff grabbed from radiosit.h & radiosit.c
******************************************************************/

extern BYTE_XYZ rad_samples[];

/******************************************************************
******************************************************************/
void ChooseRay(Ray &NewRay, const VECTOR Normal, const Ray & /*ray*/, const VECTOR Raw_Normal, int WhichRay)
{
	Vector3d random_vec;
	VECTOR up, n2, n3;
	int i;
	DBL NRay_Direction;

#define REFLECT_FOR_RADIANCE 0
#if (REFLECT_FOR_RADIANCE)
	// Get direction of reflected ray.
	DBL n = -2.0 * (ray->Direction[X] * Normal[X] + ray->Direction[Y] * Normal[Y] + ray->Direction[Z] * Normal[Z]);

	VLinComb2(NewRay->Direction, n, Normal, 1.0, ray->Direction);

	VDot(NRay_Direction, NewRay->Direction, Raw_Normal);
	if (NRay_Direction < 0.0)
	{
		// subtract 2*(projection of NRay.Direction onto Raw_Normal)
		// from NRay.Direction
		DBL Proj;
		Proj = NRay_Direction * -2;
		VAddScaledEq(NewRay.Direction, Proj, Raw_Normal);
	}
	return;
#else
	Assign_Vector(NewRay.Direction, Normal);
#endif

	if ( fabs(fabs(NewRay.Direction[Z])- 1.) < .1 )
	{
		// too close to vertical for comfort, so use cross product with horizon
		up[X] = 0.; up[Y] = 1.; up[Z] = 0.;
	}
	else
	{
		up[X] = 0.; up[Y] = 0.; up[Z] = 1.;
	}

	VCross(n2, NewRay.Direction, up);  VNormalizeEq(n2);
	VCross(n3, NewRay.Direction, n2);  VNormalizeEq(n3);

	// TODO FIXME - Magic Numbers
	//i = (int)(FRAND()*1600);
	i = WhichRay;
	WhichRay = (WhichRay + 1) % 1600;

	VUnpack(random_vec, &rad_samples[i]);

	if ( fabs(NewRay.Direction[Y] - 1.) < .001 )         // pretty well straight Y, folks
	{
		// we are within 1/20 degree of pointing in the Y axis.
		// use all vectors as is--they're precomputed this way
		Assign_Vector(NewRay.Direction, *random_vec);
	}
	else
	{
		NewRay.Direction[X] = n2[X]*random_vec[X] + NewRay.Direction[X]*random_vec[Y] + n3[X]*random_vec[Z];
		NewRay.Direction[Y] = n2[Y]*random_vec[X] + NewRay.Direction[Y]*random_vec[Y] + n3[Y]*random_vec[Z];
		NewRay.Direction[Z] = n2[Z]*random_vec[X] + NewRay.Direction[Z]*random_vec[Y] + n3[Z]*random_vec[Z];
	}

	// if our new ray goes through, flip it back across raw_normal

	VDot(NRay_Direction, NewRay.Direction, Raw_Normal);
	if (NRay_Direction < 0.0)
	{
		// subtract 2*(projection of NRay.Direction onto Raw_Normal)
		// from NRay.Direction
		DBL Proj;
		Proj = NRay_Direction * -2;
		VAddScaledEq(NewRay.Direction, Proj, Raw_Normal);
	}

	VNormalizeEq(NewRay.Direction);
}

#if(0)
int GetPhotonStat(POVMSType a)
{
	switch(a)
	{
		case kPOVAttrib_ObjectPhotonCount:
			return gPhotonStat_i;
		case kPOVAttrib_TotalPhotonCount:
			return surfacePhotonMap.numPhotons;
		case kPOVAttrib_MediaPhotonCount:
			return mediaPhotonMap.numPhotons;
		case kPOVAttrib_PhotonXSamples:
			return gPhotonStat_x_samples;
		case kPOVAttrib_PhotonYSamples:
			return gPhotonStat_y_samples;
		case kPOVAttrib_CurrentPhotonCount:
			return gPhotonStat_end;
	}

	return 0;
}
#endif

void GatheredPhotons::swapWith(GatheredPhotons& toCopy)
{
	Photon** tmpPhotonGatherList = photonGatherList;
	DBL* tmpPhotonDistances = photonDistances;
	int tmpNumFound = numFound;

	photonGatherList = toCopy.photonGatherList;
	photonDistances = toCopy.photonDistances;
	numFound = toCopy.numFound;

	toCopy.photonGatherList = tmpPhotonGatherList;
	toCopy.photonDistances = tmpPhotonDistances;
	toCopy.numFound = tmpNumFound;
}

GatheredPhotons::GatheredPhotons(int maxGatherCount)
{
	numFound = 0;
	photonGatherList = (Photon**)POV_MALLOC(sizeof(Photon *)*maxGatherCount, "Photon Map Info");
	photonDistances = (DBL *)POV_MALLOC(sizeof(DBL)*maxGatherCount, "Photon Map Info");
}

GatheredPhotons::~GatheredPhotons()
{
	if(photonGatherList)
		POV_FREE(photonGatherList);
	photonGatherList = NULL;

	if(photonDistances)
		POV_FREE(photonDistances);
	photonDistances = NULL;
}



int LightTargetCombo::computeMergedFlags()
{
	if(target)
	{
		return light->Flags | target->Flags;
	}
	else
	{
		return light->Flags = PH_RFR_ON_FLAG | PH_RFL_ON_FLAG; //light->Flags;
	}
}


void LightTargetCombo::computeAnglesAndDeltas(shared_ptr<SceneData> sceneData)
{
	shootingDirection.compute();

	// calculate the spacial separation (spread)
	photonSpread = target->Ph_Density*sceneData->photonSettings.surfaceSeparation;

	// if rays aren't parallel, divide by dist so we get separation at a distance of 1 unit
	if (!light->Parallel)
	{
		photonSpread /= shootingDirection.dist;
	}

	// adjust spread if we are using an area light
	if(light->Area_Light && light->Photon_Area_Light)
	{
		photonSpread *= sqrt((DBL)(light->Area_Size1*light->Area_Size2));
	}

	// set the photon density - calculate angular density from spacial
	if(light->Parallel)
	{
		// OK, here we fake things a bit for parallel lights.  Theta is not really theta.
		// It really represents the radius... but why re-code an entire loop.  For POV 4.0
		// this should be re-written as an abstract class with polymorphism.
		dtheta = photonSpread;
	}
	else
	{
		// calculate delta theta
		dtheta = atan(photonSpread);
	}

	// if photonSpread <= 0.0, we must return or we'll get stuck in an infinite loop!
	if (photonSpread <= 0.0)
		return;

	mintheta = 0;
	if (light->Parallel)
	{
		maxtheta = shootingDirection.rad;
	}
	else if (shootingDirection.dist>=shootingDirection.rad)
	{
		maxtheta = asin(shootingDirection.rad/shootingDirection.dist);
	}
	else
	{
		maxtheta = M_PI;
		if (fabs(shootingDirection.dist)<EPSILON)
		{
			Make_Vector(shootingDirection.up, 1,0,0);
			Make_Vector(shootingDirection.left, 0,1,0);
			Make_Vector(shootingDirection.toctr, 0,0,1);
		}
		shootingDirection.dist = shootingDirection.rad;
	}
}

} // end of namespace

