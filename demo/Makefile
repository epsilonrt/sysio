###############################################################################
# Copyright © 2015 epsilonRT, All rights reserved.                            #
# This software is governed by the CeCILL license <http://www.cecill.info>    #
###############################################################################
SUBDIRS = led button switch xbee rs485 i2c tinfo modbus serial

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
