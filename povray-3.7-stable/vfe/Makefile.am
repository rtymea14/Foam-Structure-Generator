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
noinst_LIBRARIES = libvfe.a

# Source files.
libvfe_a_SOURCES = \
vfepovms.h syspovprotobackend.h vfesession.cpp vfeconf.h syspovdebug.h vfeprecomp.cpp vfe.h vfecontrol.cpp vfedisplay.cpp vfe.cpp vfepovms.cpp syspovprotobase.h vfesession.h vfeprecomp.h syspovprotofrontend.h unix/vfeplatform.cpp unix/unixoptions.h unix/unixoptions.cpp unix/syspovconfigbase.h unix/vfeplatform.h unix/unixconsole.cpp unix/syspovconfigbackend.h unix/platformbase.cpp unix/syspovconfigfrontend.h unix/syspovconfig.h

# Include paths for headers.
AM_CPPFLAGS = \
  -I$(top_srcdir)/vfe/unix \
  -I$(top_srcdir)/unix \
  -I$(top_srcdir)/source \
  -I$(top_srcdir)/source/base \
  -I$(top_srcdir)/source/backend

# Extra definitions for compiling.
# They cannot be placed in config.h since they indirectly rely on $prefix.
DEFS = \
  @DEFS@ \
  -DPOVLIBDIR=\"@datadir@/@PACKAGE@-@VERSION_BASE@\" \
  -DPOVCONFDIR=\"@sysconfdir@/@PACKAGE@/@VERSION_BASE@\" \
  -DPOVCONFDIR_BACKWARD=\"@sysconfdir@\"
