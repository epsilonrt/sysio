###############################################################################
# Copyright © 2015 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>     #
# All rights reserved.                                                        #
# This software is governed by the CeCILL license <http://www.cecill.info>    #
###############################################################################
SUBDIRS = i2c rtc eeprom hih6130 iaq

all: $(SUBDIRS)
clean: $(SUBDIRS)
distclean: $(SUBDIRS)
rebuild: $(SUBDIRS)
lib: $(SUBDIRS)
cleanlib: $(SUBDIRS)
extcoff: $(SUBDIRS)
program: $(SUBDIRS)
debug: $(SUBDIRS)
install: $(SUBDIRS)
uninstall: $(SUBDIRS)

elf: $(SUBDIRS)
hex: $(SUBDIRS)
eep: $(SUBDIRS)
lss: $(SUBDIRS)
sym: $(SUBDIRS)

.PHONY: all clean distclean rebuild lib cleanlib extcoff program debug elf hex eep lss sym install uninstall $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -w -C $@ $(MAKECMDGOALS)

