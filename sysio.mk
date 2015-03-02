###############################################################################
# Copyright © 2015 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>     #
# All rights reserved.                                                        #
# This software is governed by the CeCILL license <http://www.cecill.info>    #
###############################################################################
# $Id$
CDEFS += -D_REENTRANT
EXTRA_LIBS += pthread rt
LDFLAGS += -pthread

# Conditionals
ifeq ($(SYSIO_LIB_ENABLE),ON)
#-------------------------------------------------------------------------------
EXTRA_LIBS += sysio

else
#-------------------------------------------------------------------------------
ifeq ($(SYSIO_ROOT),)
$(error SYSIO_ROOT not defined)
else
VPATH+=:$(SYSIO_ROOT)
EXTRA_INCDIRS += $(SYSIO_ROOT)

ifeq ($(ARCH),ARCH_ARM_RASPBERRYPI)
SYS  = SYS_LINUX
ARCH_DIR = sysio/arch/arm/rpi
CDEFS += -DARCH_ARM_RASPBERRYPI
endif

ifeq ($(ARCH),ARCH_GENERIC_LINUX)
SYS  = SYS_LINUX
ARCH_DIR = sysio/arch/generic
CDEFS += -DARCH_GENERIC_LINUX
endif

ifeq ($(SYS),SYS_LINUX)
CDEFS += -DSYS_LINUX
SYS_DIR = sysio/linux
POSIX = ON
endif

SRC += $(addprefix sysio/src/, $(notdir $(wildcard $(SYSIO_ROOT)/sysio/src/*.c)))
SRC += $(addprefix $(SYS_DIR)/, $(notdir $(wildcard $(SYSIO_ROOT)/$(SYS_DIR)/*.c)))
ifeq ($(POSIX),ON)
SRC += $(addprefix sysio/posix/, $(notdir $(wildcard $(SYSIO_ROOT)/sysio/posix/*.c)))
endif
SRC += $(addprefix $(ARCH_DIR)/, $(notdir $(wildcard $(SYSIO_ROOT)/$(ARCH_DIR)/*.c)))
#$(warning ARCH_DIR=$(ARCH_DIR))
#$(warning SYS_DIR=$(SYS_DIR))
#$(warning SRC=$(SRC))
endif

#-------------------------------------------------------------------------------
endif
