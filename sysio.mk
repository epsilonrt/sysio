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
SYS_HAS_SERIAL = ON
ARCH_DIR = sysio/src/arch/arm/rpi
CDEFS += -DARCH_ARM_RASPBERRYPI -DSYSIO_HAS_GPIO -DSYSIO_HAS_I2C
endif

ifeq ($(ARCH),ARCH_GENERIC_LINUX)
SYS  = SYS_LINUX
SYS_HAS_SERIAL = ON
ARCH_DIR = sysio/src/arch/generic
CDEFS += -DARCH_GENERIC_LINUX
endif

ifeq ($(ARCH),ARCH_WIN32)
SYS  = SYS_WIN32
ARCH_DIR = sysio/src/arch/x86
CDEFS += -DARCH_X86
endif

ifeq ($(ARCH),ARCH_WIN64)
SYS  = SYS_WIN32
ARCH_DIR = sysio/src/arch/x86_64
CDEFS += -DARCH_X86_64
endif

ifeq ($(SYS),SYS_LINUX)
CDEFS += -DSYS_LINUX
SYS_DIR = sysio/src/linux
SYS_POSIX = ON
endif

ifeq ($(SYS),SYS_WIN32)
CDEFS += -DSYS_WIN32
SYS_DIR = sysio/src/win32
endif

SRC += $(addprefix sysio/src/, $(notdir $(wildcard $(SYSIO_ROOT)/sysio/src/*.c)))
SRC += $(addprefix ssdv/src/, $(notdir $(wildcard $(SYSIO_ROOT)/ssdv/src/*.c)))
SRC += $(addprefix $(SYS_DIR)/, $(notdir $(wildcard $(SYSIO_ROOT)/$(SYS_DIR)/*.c)))

ifeq ($(SYS_HAS_GPIO),ON)
SRC += $(addprefix sysio/src/gpio/, $(notdir $(wildcard $(SYSIO_ROOT)/sysio/src/gpio/*.c)))
endif

ifeq ($(SYS_HAS_I2C),ON)
SRC += $(addprefix sysio/src/i2c/, $(notdir $(wildcard $(SYSIO_ROOT)/sysio/src/i2c/*.c)))
SRC += $(addprefix chipio/src/, $(notdir $(wildcard $(SYSIO_ROOT)/chipio/src/*.c)))
endif

ifeq ($(SYS_POSIX),ON)
SRC += $(addprefix sysio/src/posix/, $(notdir $(wildcard $(SYSIO_ROOT)/sysio/src/posix/*.c)))
SRC += $(addprefix radio/src/, $(notdir $(wildcard $(SYSIO_ROOT)/radio/src/*.c)))
endif

SRC += $(addprefix $(ARCH_DIR)/, $(notdir $(wildcard $(SYSIO_ROOT)/$(ARCH_DIR)/*.c)))

#--- Debug
#$(warning ARCH_DIR=$(ARCH_DIR))
#$(warning SYS_DIR=$(SYS_DIR))
#$(warning SRC=$(SRC))

#-------------------------------------------------------------------------------
