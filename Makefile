#****************************************************************************
# Copyright (C) 2001-2010  PEAK System-Technik GmbH
#
# linux@peak-system.com
# www.peak-system.com
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#
# Maintainer(s): Klaus Hitschler (klaus.hitschler@gmx.de)
# Contributions: Pablo Yanez Trujillo (yanez@pse.de) cross-compile
#****************************************************************************

#
# Makefile - global Makefile for all components
#
# $Id$
#

# CROSS-COMPILING SETTINGS
#
# You need a cross-compiler. You can build one with crosstool
# http://www.kegel.com/crosstool/current/doc/crosstool-howto.html
#
# These variables work with the toolchains created by crosstool

# defines the architecture. For example 'arm' for an ARM system
#export ARCH=arm

# the path and prefix of the cross-compiler
#export CROSS_COMPILE=/home/yanez/matrix500/arm-9tdmi-linux-gnu/bin/arm-9tdmi-linux-gnu-
ifneq ($(CROSS_COMPILE),)
export CC=$(CROSS_COMPILE)gcc
endif

# MACROS AND DEFINES

# Strip quotes and then whitespaces
qstrip=$(strip $(subst ",,$(1)))
#"))

define do-make
@make -C driver $1
@make -C lib $1
@make -C test $1
endef

define make-all
@make -C driver depend
$(call do-make, all)
endef

define make-clean
$(call do-make, clean)
endef

define make-install
$(call do-make, install)
endef

define make-xeno
$(call do-make, xeno)
endef

define make-rtai
$(call do-make, rtai)
endef

define make-uninstall
$(call do-make, uninstall)
endef

# DO IT
all :
	$(make-all)

clean:
	$(make-clean)

install:
	$(make-install)
 
uninstall:
	$(make-uninstall)

xeno:
	$(make-xeno)

rtai:
	$(make-rtai)
# end


# DO NOT DELETE
