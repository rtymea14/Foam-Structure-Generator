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

# Programs to build.
bin_PROGRAMS = povray

# Source files.
povray_SOURCES = \
  disp.h \
  disp_sdl.cpp disp_sdl.h \
  disp_text.cpp disp_text.h

# Include paths for headers.
AM_CPPFLAGS = \
  -I$(top_srcdir) \
  -I$(top_srcdir)/source \
  -I$(top_builddir)/source \
  -I$(top_srcdir)/source/backend \
  -I$(top_srcdir)/source/base \
  -I$(top_srcdir)/source/frontend \
  -I$(top_srcdir)/vfe \
  -I$(top_srcdir)/vfe/unix

# Libraries to link with.
# Beware: order does matter!
LDADD = \
  $(top_builddir)/vfe/libvfe.a \
  $(top_builddir)/source/libpovray.a
