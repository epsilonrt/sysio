###############################################################################
# Copyright © 2015 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>     #
# All rights reserved.                                                        #
# This software is governed by the CeCILL license <http://www.cecill.info>    #
###############################################################################

#-------------------------------------------------------------------------------
VPATH+=:$(SYSIO_ROOT)
EXTRA_INCDIRS += $(SYSIO_ROOT)

ifeq ($(ARCH),ARCH_ARM_RASPBERRYPI)
SYS  = SYS_LINUX
SYS_HAS_GPIO = ON
SYS_HAS_I2C = ON
ARCH_DIR = sysio/arch/arm/rpi
CDEFS += -DARCH_ARM_RASPBERRYPI -DSYSIO_HAS_GPIO -DSYSIO_HAS_I2C
endif

ifeq ($(ARCH),ARCH_GENERIC_LINUX)
SYS  = SYS_LINUX
ARCH_DIR = sysio/arch/generic
CDEFS += -DARCH_GENERIC_LINUX
endif

ifeq ($(SYS),SYS_LINUX)
CDEFS += -DSYS_LINUX
SYS_DIR = sysio/linux
SYS_POSIX = ON
endif

SRC += $(addprefix sysio/src/, $(notdir $(wildcard $(SYSIO_ROOT)/sysio/src/*.c)))
SRC += $(addprefix sysio/src/radio/, $(notdir $(wildcard $(SYSIO_ROOT)/sysio/src/radio/*.c)))
SRC += $(addprefix sysio/src/ssdv/, $(notdir $(wildcard $(SYSIO_ROOT)/sysio/src/ssdv/*.c)))
SRC += $(addprefix $(SYS_DIR)/, $(notdir $(wildcard $(SYSIO_ROOT)/$(SYS_DIR)/*.c)))
ifeq ($(SYS_HAS_GPIO),ON)
SRC += $(addprefix sysio/src/gpio/, $(notdir $(wildcard $(SYSIO_ROOT)/sysio/src/gpio/*.c)))
endif
ifeq ($(SYS_HAS_I2C),ON)
SRC += $(addprefix sysio/src/i2c/, $(notdir $(wildcard $(SYSIO_ROOT)/sysio/src/i2c/*.c)))
endif
ifeq ($(SYS_POSIX),ON)
SRC += $(addprefix sysio/posix/, $(notdir $(wildcard $(SYSIO_ROOT)/sysio/posix/*.c)))
endif
SRC += $(addprefix $(ARCH_DIR)/, $(notdir $(wildcard $(SYSIO_ROOT)/$(ARCH_DIR)/*.c)))
#$(warning ARCH_DIR=$(ARCH_DIR))
#$(warning SYS_DIR=$(SYS_DIR))
#$(warning SRC=$(SRC))

#-------------------------------------------------------------------------------
