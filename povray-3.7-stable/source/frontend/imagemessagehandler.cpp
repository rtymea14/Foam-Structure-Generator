/*******************************************************************************
 * imagemessagehandler.cpp
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
 * $File: //depot/public/povray/3.x/source/frontend/imagemessagehandler.cpp $
 * $Revision: #1 $
 * $Change: 6069 $
 * $DateTime: 2013/11/06 11:59:40 $
 * $Author: chrisc $
 *******************************************************************************/

// configbase.h must always be the first POV file included within base *.cpp files
#include "base/configbase.h"
#include "base/types.h"

#include "frontend/configfrontend.h"
#include "frontend/renderfrontend.h"
#include "frontend/imagemessagehandler.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov_frontend
{

ImageMessageHandler::ImageMessageHandler()
{
}

ImageMessageHandler::~ImageMessageHandler()
{
}

void ImageMessageHandler::HandleMessage(const SceneData& sd, const ViewData& vd, POVMSType ident, POVMS_Object& msg)
{
	switch(ident)
	{
		case kPOVMsgIdent_PixelSet:
			DrawPixelSet(sd, vd, msg);
			break;
		case kPOVMsgIdent_PixelBlockSet:
			DrawPixelBlockSet(sd, vd, msg);
			break;
		case kPOVMsgIdent_PixelRowSet:
			DrawPixelRowSet(sd, vd, msg);
			break;
		case kPOVMsgIdent_RectangleFrameSet:
			DrawRectangleFrameSet(sd, vd, msg);
			break;
		case kPOVMsgIdent_FilledRectangleSet:
			DrawFilledRectangleSet(sd, vd, msg);
			break;
	}
}

void ImageMessageHandler::DrawPixelSet(const SceneData& sd, const ViewData& vd, POVMS_Object& msg)
{
	POVMS_Attribute pixelposattr;
	POVMS_Attribute pixelcolattr;
	unsigned int psize(msg.GetInt(kPOVAttrib_PixelSize));

	msg.Get(kPOVAttrib_PixelPositions, pixelposattr);
	msg.Get(kPOVAttrib_PixelColors, pixelcolattr);

	vector<POVMSInt> pixelpositions(pixelposattr.GetIntVector());
	vector<POVMSFloat> pixelcolors(pixelcolattr.GetFloatVector());

	if((pixelpositions.size() / 2) != (pixelcolors.size() / 5))
		throw POV_EXCEPTION(kInvalidDataSizeErr, "Number of pixel colors and pixel positions does not match!");

	GammaCurvePtr gamma;
	if (vd.display)
		gamma = vd.display->GetGamma();

	for(int i = 0, ii = 0; (i < pixelcolors.size()) && (ii < pixelpositions.size()); i += 5, ii += 2)
	{
		Colour col(pixelcolors[i], pixelcolors[i + 1], pixelcolors[i + 2], pixelcolors[i + 3], pixelcolors[i + 4]);
		Colour gcol(col);
		unsigned int x(pixelpositions[ii]);
		unsigned int y(pixelpositions[ii + 1]);
		Display::RGBA8 rgba;
		float dither = GetDitherOffset(x, y);

		if(vd.display)
		{
			// TODO ALPHA - display may profit from receiving the data in its original, premultiplied form
			// Premultiplied alpha was good for the math, but the display expects non-premultiplied alpha, so fix this if possible.
			float alpha = gcol.FTtoA();
			if (alpha != 1.0 && fabs(alpha) > 1e-6) // TODO FIXME - magic value
			{
				gcol.red()   /= alpha;
				gcol.green() /= alpha;
				gcol.blue()  /= alpha;
			}
		}

		rgba.red   = IntEncode(gamma, gcol.red(),   255, dither);
		rgba.green = IntEncode(gamma, gcol.green(), 255, dither);
		rgba.blue  = IntEncode(gamma, gcol.blue(),  255, dither);
		rgba.alpha = IntEncode(       gcol.FTtoA(), 255, dither);

		if(psize == 1)
		{
			if(vd.display)
				vd.display->DrawPixel(x, y, rgba);

			if((vd.image) && (x < vd.image->GetWidth()) && (y < vd.image->GetHeight()))
				vd.image->SetRGBAValue(x, y, col.red(), col.green(), col.blue(), col.FTtoA());
		}
		else
		{
			if(vd.display)
				vd.display->DrawFilledRectangle(x, y, x + psize - 1, y + psize - 1, rgba);

			if(vd.image)
			{
				for(unsigned int py = 0; (py < psize) && (y + py < vd.image->GetHeight()); py++)
				{
					for(unsigned int px = 0; (px < psize) && (x + px < vd.image->GetWidth()); px++)
						vd.image->SetRGBAValue(x + px, y + py, col.red(), col.green(), col.blue(), col.FTtoA());
				}
			}
		}
	}

	if(vd.imageBackup)
	{
		msg.Write(*vd.imageBackup);
		vd.imageBackup->flush();
	}
}

void ImageMessageHandler::DrawPixelBlockSet(const SceneData& sd, const ViewData& vd, POVMS_Object& msg)
{
	POVRect rect(msg.GetInt(kPOVAttrib_Left), msg.GetInt(kPOVAttrib_Top), msg.GetInt(kPOVAttrib_Right), msg.GetInt(kPOVAttrib_Bottom));
	POVMS_Attribute pixelattr;
	vector<Colour> cols;
	vector<Display::RGBA8> rgbas;
	unsigned int psize(msg.GetInt(kPOVAttrib_PixelSize));
	int i = 0;

	msg.Get(kPOVAttrib_PixelBlock, pixelattr);

	vector<POVMSFloat> pixelvector(pixelattr.GetFloatVector());

	cols.reserve(rect.GetArea());
	rgbas.reserve(rect.GetArea());

	GammaCurvePtr gamma;

	if (vd.display)
		gamma = vd.display->GetGamma();

	for(i = 0; i < rect.GetArea() *  5; i += 5)
	{
		Colour col(pixelvector[i], pixelvector[i + 1], pixelvector[i + 2], pixelvector[i + 3], pixelvector[i + 4]);
		Colour gcol(col);
		Display::RGBA8 rgba;
		unsigned int x(rect.left + (i/5) % rect.GetWidth());
		unsigned int y(rect.top  + (i/5) / rect.GetWidth());
		float dither = GetDitherOffset(x, y);

		if(vd.display)
		{
			// TODO ALPHA - display may profit from receiving the data in its original, premultiplied form
			// Premultiplied alpha was good for the math, but the display expects non-premultiplied alpha, so fix this if possible.
			float alpha = gcol.FTtoA();
			if (alpha != 1.0 && fabs(alpha) > 1e-6) // TODO FIXME - magic value
			{
				gcol.red()   /= alpha;
				gcol.green() /= alpha;
				gcol.blue()  /= alpha;
			}
		}

		rgba.red   = IntEncode(gamma, gcol.red(),   255, dither);
		rgba.green = IntEncode(gamma, gcol.green(), 255, dither);
		rgba.blue  = IntEncode(gamma, gcol.blue(),  255, dither);
		rgba.alpha = IntEncode(       gcol.FTtoA(), 255, dither);

		cols.push_back(col);
		rgbas.push_back(rgba);
	}

	if(vd.display)
	{
		if(psize == 1)
			vd.display->DrawPixelBlock(rect.left, rect.top, rect.right, rect.bottom, &rgbas[0]);
		else
		{
			for(unsigned int y = rect.top, i = 0; y <= rect.bottom; y += psize)
			{
				for(unsigned int x = rect.left; x <= rect.right; x += psize, i++)
					vd.display->DrawFilledRectangle(x, y, x + psize - 1, y + psize - 1, rgbas[0]);
			}
		}
	}

	if(vd.image)
	{
		for(unsigned int y = rect.top, i = 0; y <= rect.bottom; y += psize)
		{
			for(unsigned int x = rect.left; x <= rect.right; x += psize, i++)
			{
				for(unsigned int py = 0; py < psize; py++)
				{
					for(unsigned int px = 0; px < psize; px++)
						vd.image->SetRGBAValue(x + px, y + py, cols[i].red(), cols[i].green(), cols[i].blue(), cols[i].FTtoA());
				}
			}
		}
	}

	if(vd.imageBackup)
	{
		msg.Write(*vd.imageBackup);
		vd.imageBackup->flush();
	}
}

void ImageMessageHandler::DrawPixelRowSet(const SceneData& sd, const ViewData& vd, POVMS_Object& msg)
{
}

void ImageMessageHandler::DrawRectangleFrameSet(const SceneData& sd, const ViewData& vd, POVMS_Object& msg)
{
}

void ImageMessageHandler::DrawFilledRectangleSet(const SceneData& sd, const ViewData& vd, POVMS_Object& msg)
{
}

}
