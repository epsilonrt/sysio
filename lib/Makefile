###############################################################################
# Copyright © 2015 epsilonRT, All rights reserved.                            #
# This software is governed by the CeCILL license <http://www.cecill.info>    #
###############################################################################

SUBDIRS = sysio sysio++

# Choix de l'architecture matérielle du système
#BOARD = BOARD_GENERIC_LINUX
#BOARD = BOARD_RASPBERRYPI

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
	$(MAKE) -w -C $@ $(MAKECMDGOALS) prefix=$(prefix) BOARD=$(BOARD) DEBUG=$(DEBUG)

.PHONY: all rebuild clean distclean install uninstall $(SUBDIRS)
