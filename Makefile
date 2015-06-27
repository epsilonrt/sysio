###############################################################################
# Copyright © 2015 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>     #
# All rights reserved.                                                        #
# This software is governed by the CeCILL license <http://www.cecill.info>    #
###############################################################################
# $Id$

SUBDIRS = lib

# Relative path of the project's root directory
SYSIO_ROOT = .

# Project build options
ARCH = ARCH_GENERIC_LINUX
#ARCH = ARCH_ARM_RASPBERRYPI

#---------------- Install Options ----------------
prefix=/usr/local

INSTALL_BINDIR=$(prefix)/bin
INSTALL_DATDIR=$(prefix)/share
MSG_INSTALL = [INSTALL]
MSG_UNINSTALL = [UNINSTALL]

all: $(SUBDIRS)
rebuild: $(SUBDIRS)
clean: $(SUBDIRS)
distclean: $(SUBDIRS)
install: install_utils $(SUBDIRS)
uninstall: uninstall_utils $(SUBDIRS)

install_utils: uninstall_utils
	@echo "$(MSG_INSTALL) $(TARGET) utils and templates in $(prefix)"
	@-install -d -m 0755 $(INSTALL_DATDIR)/sysio
	@-install -d -m 0755 $(INSTALL_DATDIR)/sysio/template
	@-install -d -m 0755 $(INSTALL_DATDIR)/sysio/template/cpp
	@-install -m 0644 $(SYSIO_ROOT)/util/template/* $(INSTALL_DATDIR)/sysio/template
	@-install -m 0644 $(SYSIO_ROOT)/util/template/cpp/* $(INSTALL_DATDIR)/sysio/template/cpp
	@-install -m 0755 $(SYSIO_ROOT)/util/bin/sysio-prj $(INSTALL_BINDIR)
	@-install -m 0755 $(SYSIO_ROOT)/util/bin/sysio-ver $(INSTALL_BINDIR)
	@sed -i -e "s#INSTALLED_TEMPLATE_DIR#$(INSTALL_DATDIR)/sysio/template#g" $(INSTALL_BINDIR)/sysio-prj

uninstall_utils:
	@echo "$(MSG_UNINSTALL) $(TARGET) utils and templates from $(prefix)"
	@-rm -fr $(INSTALL_DATDIR)/sysio
	@-rm -fr $(INSTALL_BINDIR)/sysio-prj
	@-rm -fr $(INSTALL_BINDIR)/sysio-ver

$(SUBDIRS):
	$(MAKE) -w -C $@ $(MAKECMDGOALS) prefix=$(prefix) ARCH=$(ARCH)

.PHONY: all rebuild clean distclean install uninstall $(SUBDIRS)
