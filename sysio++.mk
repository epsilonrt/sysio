###############################################################################
# Copyright © 2015 epsilonRT, All rights reserved.                            #
# This software is governed by the CeCILL license <http://www.cecill.info>    #
###############################################################################

#-------------------------------------------------------------------------------
include $(SYSIO_ROOT)/sysio.mk
CPPSRC += $(addprefix sysio/src/, $(notdir $(wildcard $(SYSIO_ROOT)/sysio/src/*.cpp)))
CPPSRC += $(addprefix radio/src/, $(notdir $(wildcard $(SYSIO_ROOT)/radio/src/*.cpp)))

#-------------------------------------------------------------------------------
