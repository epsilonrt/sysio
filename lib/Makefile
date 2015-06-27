###############################################################################
# Copyright © 2015 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>     #
# All rights reserved.                                                        #
# This software is governed by the CeCILL license <http://www.cecill.info>    #
###############################################################################
# $Id$

SUBDIRS = sysio sysio++

# Project build options
ARCH = ARCH_GENERIC_LINUX
#ARCH = ARCH_ARM_RASPBERRYPI

#---------------- Install Options ----------------
prefix=/usr/local

all: $(SUBDIRS)
rebuild: $(SUBDIRS)
clean: $(SUBDIRS)
distclean: $(SUBDIRS)
install: $(SUBDIRS)
uninstall: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -w -C $@ $(MAKECMDGOALS) prefix=$(prefix) ARCH=$(ARCH)

.PHONY: all rebuild clean distclean install uninstall $(SUBDIRS)
