###############################################################################
# Copyright © 2015 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>     #
# All rights reserved.                                                        #
# This software is governed by the CeCILL license <http://www.cecill.info>    #
###############################################################################

#-------------------------------------------------------------------------------
include $(SYSIO_ROOT)/sysio.mk
CPPSRC += $(addprefix sysio/src/, $(notdir $(wildcard $(SYSIO_ROOT)/sysio/src/*.cpp)))
CPPSRC += $(addprefix sysio/src/radio/, $(notdir $(wildcard $(SYSIO_ROOT)/sysio/src/radio/*.cpp)))

#-------------------------------------------------------------------------------
