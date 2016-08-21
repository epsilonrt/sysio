###############################################################################
# Copyright © 2015 epsilonRT, All rights reserved.                            #
# This software is governed by the CeCILL license <http://www.cecill.info>    #
###############################################################################
SUBDIRS = blyss dinput dlist doutput gpio pwm rpi rs485 serial tinfo vector xbee

all: $(SUBDIRS)
clean: $(SUBDIRS)
distclean: $(SUBDIRS)
rebuild: $(SUBDIRS)
install: $(SUBDIRS)
uninstall: $(SUBDIRS)

elf: $(SUBDIRS)
lss: $(SUBDIRS)
sym: $(SUBDIRS)

.PHONY: all clean distclean rebuild install uninstall elf lss sym $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -w -C $@ $(MAKECMDGOALS)
