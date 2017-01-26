###############################################################################
# Copyright © 2015 epsilonRT, All rights reserved.                            #
# This software is governed by the CeCILL license <http://www.cecill.info>    #
###############################################################################

SUBDIRS = lib
CLEANER_SUBDIRS = demo doc util sysio/test

# Chemin relatif du répertoire racine de SysIO
SYSIO_ROOT = .

# Choix de l'architecture matérielle du système
ARCH = ARCH_GENERIC_LINUX

# Activation des informations Debug (ON/OFF)
# Si défini sur ON, aucune information de debug ne sera générée
#DEBUG = ON

#---------------- Install Options ----------------
prefix=/usr/local

INSTALL_BINDIR=$(prefix)/bin
INSTALL_DATDIR=$(prefix)/share
MSG_INSTALL = [INSTALL]
MSG_UNINSTALL = [UNINSTALL]

all: $(SUBDIRS)
rebuild: $(SUBDIRS)
clean: $(SUBDIRS) $(CLEANER_SUBDIRS)
distclean: $(SUBDIRS) $(CLEANER_SUBDIRS)
install: install_utils $(SUBDIRS)
uninstall: uninstall_utils $(SUBDIRS)

install_utils: uninstall_utils
	@echo "$(MSG_INSTALL) $(TARGET) utils and templates in $(prefix)"
	@-install -d -m 0755 $(INSTALL_DATDIR)/sysio
	@-install -d -m 0755 $(INSTALL_DATDIR)/sysio/template
	@-install -d -m 0755 $(INSTALL_DATDIR)/sysio/template/cpp
	@-install -m 0644 $(SYSIO_ROOT)/util/template/Makefile $(INSTALL_DATDIR)/sysio/template
	@-install -m 0644 $(SYSIO_ROOT)/util/template/template.c $(INSTALL_DATDIR)/sysio/template
	@-install -m 0644 $(SYSIO_ROOT)/util/template/template.project $(INSTALL_DATDIR)/sysio/template
	@-install -m 0644 $(SYSIO_ROOT)/util/template/cpp/* $(INSTALL_DATDIR)/sysio/template/cpp
	@-install -m 0755 $(SYSIO_ROOT)/util/bin/sysio-prj $(INSTALL_BINDIR)
	@-install -m 0755 $(SYSIO_ROOT)/util/bin/sysio-ver $(INSTALL_BINDIR)
	@-install -m 0755 $(SYSIO_ROOT)/util/bin/rpi-cpu $(INSTALL_BINDIR)
	@-install -m 0755 $(SYSIO_ROOT)/util/bin/rpi-rev $(INSTALL_BINDIR)
	@-install -m 0755 $(SYSIO_ROOT)/util/bin/hardware-cpu $(INSTALL_BINDIR)
	@-install -m 0755 $(SYSIO_ROOT)/util/bin/sunxi-board $(INSTALL_BINDIR)
	@sed -i -e "s#INSTALLED_TEMPLATE_DIR#$(INSTALL_DATDIR)/sysio/template#g" $(INSTALL_BINDIR)/sysio-prj

uninstall_utils:
	@echo "$(MSG_UNINSTALL) $(TARGET) utils and templates from $(prefix)"
	@-rm -fr $(INSTALL_DATDIR)/sysio
	@-rm -fr $(INSTALL_BINDIR)/sysio-prj
	@-rm -fr $(INSTALL_BINDIR)/sysio-ver
	@-rm -fr $(INSTALL_BINDIR)/rpi-cpu
	@-rm -fr $(INSTALL_BINDIR)/rpi-rev
	@-rm -fr $(INSTALL_BINDIR)/hardware-cpu
	@-rm -fr $(INSTALL_BINDIR)/sunxi-board

$(SUBDIRS):
	$(MAKE) -w -C $@ $(MAKECMDGOALS) prefix=$(prefix) ARCH=$(ARCH) DEBUG=$(DEBUG)

$(CLEANER_SUBDIRS):
	$(MAKE) -w -C $@ $(MAKECMDGOALS) prefix=$(prefix) ARCH=$(ARCH) DEBUG=$(DEBUG)

.PHONY: all rebuild clean distclean install uninstall $(SUBDIRS) $(CLEANER_SUBDIRS)
