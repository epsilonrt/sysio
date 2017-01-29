###############################################################################
# Copyright © 2015 epsilonRT, All rights reserved.                            #
# This software is governed by the CeCILL license <http://www.cecill.info>    #
###############################################################################
SUBDIRS = blyss dinput dlist doutput gpio pwm rs485 serial tinfo vector xbee
CLEANER_SUBDIRS = rpi nanopi

all: $(SUBDIRS)
clean: $(SUBDIRS) $(CLEANER_SUBDIRS)
distclean: $(SUBDIRS) $(CLEANER_SUBDIRS)
rebuild: $(SUBDIRS)
install: $(SUBDIRS)
uninstall: $(SUBDIRS)

elf: $(SUBDIRS)
lss: $(SUBDIRS)
sym: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -w -C $@ $(MAKECMDGOALS)

$(CLEANER_SUBDIRS):
	$(MAKE) -w -C $@ $(MAKECMDGOALS) prefix=$(prefix) BOARD=$(BOARD) DEBUG=$(DEBUG)

.PHONY: all clean distclean rebuild install uninstall elf lss sym $(SUBDIRS) $(CLEANER_SUBDIRS)
