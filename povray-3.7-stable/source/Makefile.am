##############################################################################
#               Makefile.am
#
# Process this file with automake to produce Makefile.in
#
# ---------------------------------------------------------------------------
# Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
# Copyright 1991-2013 Persistence of Vision Raytracer Pty. Ltd.
#
# POV-Ray is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
#
# POV-Ray is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
# ---------------------------------------------------------------------------
# POV-Ray is based on the popular DKB raytracer version 2.12.
# DKBTrace was originally written by David K. Buck.
# DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
##############################################################################

# Makefile.am for the source distribution of POV-Ray 3.7 for UNIX
# Written by POV-Ray Bugtracker http://bugs.povray.org/

# Libraries to build.
noinst_LIBRARIES = libpovray.a

# Source files.
libpovray_a_SOURCES = \
base/path.h base/pov_err.cpp base/povms.cpp base/textstream.h base/povdebug.h base/stringutilities.cpp base/precomp.cpp base/textstreambuffer.cpp base/pointer.h base/processoptions.cpp base/safemath.h base/povmsgid.h base/configbase.h base/textstreambuffer.h base/pov_err.h base/povmscpp.cpp base/image/hdr.cpp base/image/tiff.cpp base/image/gifdecod.cpp base/image/ppm.cpp base/image/targa.cpp base/image/targa.h base/image/colourspace.cpp base/image/pgm.h base/image/jpeg.cpp base/image/openexr.h base/image/metadata.h base/image/png.cpp base/image/png_pov.h base/image/iff.cpp base/image/hdr.h base/image/encoding.h base/image/pgm.cpp base/image/openexr.cpp base/image/ppm.h base/image/image.cpp base/image/jpeg_pov.h base/image/colourspace.h base/image/bmp.cpp base/image/gif.cpp base/image/gif.h base/image/encoding.cpp base/image/tiff_pov.h base/image/bmp.h base/image/image.h base/image/iff.h base/povms.h base/font/cyrvetic.h base/font/crystal.cpp base/font/povlogo.cpp base/font/timrom.h base/font/cyrvetic.cpp base/font/povlogo.h base/font/timrom.cpp base/font/crystal.h base/timer.cpp base/precomp.h base/animation/animation.h base/animation/moov.cpp base/animation/moov.h base/animation/animation.cpp base/fileinputoutput.h base/platformbase.h base/timer.h base/types.h base/povmscpp.h base/path.cpp base/stringutilities.h base/fileinputoutput.cpp base/processoptions.h base/textstream.cpp povrayold.h lightgrp.cpp backend/povray.h backend/math/hcmplx.h backend/math/splines.h backend/math/polysolv.cpp backend/math/mathutil.h backend/math/chi2.h backend/math/hcmplx.cpp backend/math/matrices.h backend/math/quatern.cpp backend/math/polysolv.h backend/math/splines.cpp backend/math/chi2.cpp backend/math/matrices.cpp backend/math/quatern.h backend/math/vector.h backend/math/mathutil.cpp backend/configbackend.h backend/render/tracetask.cpp backend/render/ray.cpp backend/render/tracepixel.cpp backend/render/radiositytask.h backend/render/rendertask.h backend/render/radiositytask.cpp backend/render/rendertask.cpp backend/render/trace.cpp backend/render/tracetask.h backend/render/ray.h backend/render/trace.h backend/render/tracepixel.h backend/lighting/photonestimationtask.h backend/lighting/photonsortingtask.cpp backend/lighting/photonstrategytask.cpp backend/lighting/photons.cpp backend/lighting/photonsortingtask.h backend/lighting/point.h backend/lighting/photonshootingtask.h backend/lighting/radiosity.h backend/lighting/photonstrategytask.h backend/lighting/photonshootingtask.cpp backend/lighting/photonshootingstrategy.h backend/lighting/photonshootingstrategy.cpp backend/lighting/radiosity.cpp backend/lighting/subsurface.cpp backend/lighting/photonestimationtask.cpp backend/lighting/rad_data.cpp backend/lighting/subsurface.h backend/lighting/point.cpp backend/lighting/photons.h backend/pattern/warps.cpp backend/pattern/warps.h backend/pattern/pattern.h backend/pattern/pattern.cpp backend/precomp.cpp backend/shape/prism.cpp backend/shape/torus.h backend/shape/sor.cpp backend/shape/super.h backend/shape/quadrics.h backend/shape/lathe.cpp backend/shape/boxes.cpp backend/shape/bezier.h backend/shape/planes.cpp backend/shape/triangle.h backend/shape/ovus.h backend/shape/quadrics.cpp backend/shape/boxes.h backend/shape/blob.cpp backend/shape/polygon.h backend/shape/polygon.cpp backend/shape/prism.h backend/shape/discs.cpp backend/shape/cones.cpp backend/shape/csg.h backend/shape/truetype.h backend/shape/bezier.cpp backend/shape/hfield.h backend/shape/truetype.cpp backend/shape/isosurf.h backend/shape/mesh.h backend/shape/spheres.h backend/shape/blob.h backend/shape/fpmetric.h backend/shape/hfield.cpp backend/shape/planes.h backend/shape/poly.cpp backend/shape/mesh.cpp backend/shape/fractal.h backend/shape/fpmetric.cpp backend/shape/sphsweep.cpp backend/shape/csg.cpp backend/shape/spheres.cpp backend/shape/super.cpp backend/shape/sphsweep.h backend/shape/cones.h backend/shape/lathe.h backend/shape/fractal.cpp backend/shape/triangle.cpp backend/shape/ovus.cpp backend/shape/torus.cpp backend/shape/poly.h backend/shape/discs.h backend/shape/isosurf.cpp backend/shape/sor.h backend/interior/media.h backend/interior/interior.cpp backend/interior/interior.h backend/interior/media.cpp backend/frame.h backend/povray.cpp backend/parser/tokenize.cpp backend/parser/reswords.h backend/parser/fnsyntax.cpp backend/parser/parse.h backend/parser/express.cpp backend/parser/parstxtr.cpp backend/parser/parse.cpp backend/parser/parsestr.cpp backend/parser/function.cpp backend/parser/reswords.cpp backend/control/renderbackend.cpp backend/control/messagefactory.cpp backend/control/benchmark.h backend/control/benchmark.cpp backend/control/messagefactory.h backend/control/renderbackend.h backend/texture/texture.cpp backend/texture/texture.h backend/texture/avxfma4check.h backend/texture/normal.h backend/texture/pigment.cpp backend/texture/normal.cpp backend/texture/pigment.h backend/precomp.h backend/vm/fnintern.h backend/vm/fncode.h backend/vm/fnintern.cpp backend/vm/fnpovfpu.cpp backend/vm/fncode.cpp backend/vm/fnpovfpu.h backend/bounding/boundingtask.h backend/bounding/bcyl.h backend/bounding/bsphere.cpp backend/bounding/bsphere.h backend/bounding/boundingtask.cpp backend/bounding/bcyl.cpp backend/bounding/bbox.h backend/bounding/bbox.cpp backend/colour/colour.h backend/colour/spectral.cpp backend/colour/colour.cpp backend/colour/spectral.h backend/colour/colutils.cpp backend/colour/colutils.h backend/scene/view.h backend/scene/objects.cpp backend/scene/atmosph.h backend/scene/camera.cpp backend/scene/threaddata.cpp backend/scene/scene.cpp backend/scene/objects.h backend/scene/view.cpp backend/scene/scene.h backend/scene/threaddata.h backend/scene/atmosph.cpp backend/scene/camera.h backend/support/randomsequences.h backend/support/jitter.cpp backend/support/bsptree.h backend/support/msgutil.cpp backend/support/bsptree.cpp backend/support/taskqueue.h backend/support/fixedallocator.h backend/support/octree.h backend/support/randomsequences.cpp backend/support/simplevector.h backend/support/imageutil.cpp backend/support/octree.cpp backend/support/fileutil.h backend/support/fileutil.cpp backend/support/task.cpp backend/support/msgutil.h backend/support/task.h backend/support/statistics.cpp backend/support/statistics.h backend/support/jitter.h backend/support/taskqueue.cpp backend/support/imageutil.h optout.cpp frontend/shelloutprocessing.cpp frontend/configfrontend.h frontend/console.cpp frontend/imagemessagehandler.h frontend/imagemessagehandler.cpp frontend/display.h frontend/animationprocessing.h frontend/processrenderoptions.cpp frontend/defaultplatformbase.h frontend/processrenderoptions.h frontend/precomp.cpp frontend/filemessagehandler.cpp frontend/imageprocessing.cpp frontend/parsermessagehandler.h frontend/rendermessagehandler.h frontend/display.cpp frontend/rendermessagehandler.cpp frontend/defaultplatformbase.cpp frontend/imageprocessing.h frontend/precomp.h frontend/simplefrontend.h frontend/animationprocessing.cpp frontend/console.h frontend/shelloutprocessing.h frontend/filemessagehandler.h frontend/renderfrontend.h frontend/renderfrontend.cpp frontend/parsermessagehandler.cpp pov_mem.h lightgrp.h pov_mem.cpp statspov.h

# Include paths for headers.
AM_CPPFLAGS = \
  -I$(top_srcdir) \
  -I$(top_srcdir)/source/backend \
  -I$(top_srcdir)/source/base \
  -I$(top_srcdir)/source/frontend \
  -I$(top_srcdir)/unix \
  -I$(top_srcdir)/vfe \
  -I$(top_srcdir)/vfe/unix
