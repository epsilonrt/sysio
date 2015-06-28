###############################################################################
# Copyright © 2015 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>     #
# All rights reserved.                                                        #
# This software is governed by the CeCILL license <http://www.cecill.info>    #
###############################################################################

SUBDIRS = sysio sysio++

# Choix de l'architecture matérielle du système
ARCH = ARCH_GENERIC_LINUX
#ARCH = ARCH_ARM_RASPBERRYPI

# Activation des informations Debug (ON/OFF)
# Si défini sur ON, aucune information de debug ne sera générée
#DEBUG = ON

#---------------- Install Options ----------------
prefix=/usr/local

all: $(SUBDIRS)
rebuild: $(SUBDIRS)
clean: $(SUBDIRS)
distclean: $(SUBDIRS)
install: $(SUBDIRS)
uninstall: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -w -C $@ $(MAKECMDGOALS) prefix=$(prefix) ARCH=$(ARCH) DEBUG=$(DEBUG)

.PHONY: all rebuild clean distclean install uninstall $(SUBDIRS)
