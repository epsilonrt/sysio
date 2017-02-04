###############################################################################
# Copyright © 2015 epsilonRT, All rights reserved.                            #
# This software is governed by the CeCILL license <http://www.cecill.info>    #
###############################################################################

#-------------------------------------------------------------------------------
VPATH+=:$(SYSIO_ROOT)
EXTRA_INCDIRS += $(SYSIO_ROOT) $(SYSIO_ROOT)/sysio/src $(SYSIO_ROOT)/3rdparty

ifeq ($(BOARD),BOARD_RASPBERRYPI)
SYS  = SYS_LINUX
SYS_HAS_GPIO = ON
SYS_HAS_I2C = ON
SYS_HAS_SPI = ON
SYS_HAS_SERIAL = ON
ARCH_DIR = sysio/src/arch/arm/rpi
CDEFS += -DARCH_ARM -DSYSIO_HAS_GPIO -DSYSIO_HAS_I2C -DSYS_HAS_SERIAL
endif

ifneq (,$(filter $(BOARD),BOARD_NANOPI_NEO BOARD_NANOPI_AIR BOARD_NANOPI_M1))
SYS  = SYS_LINUX
SYS_HAS_GPIO = ON
SYS_HAS_I2C = ON
SYS_HAS_SPI = ON
SYS_HAS_SERIAL = ON
ARCH_DIR = sysio/src/arch/arm/nanopi
CDEFS += -DARCH_ARM -DSYSIO_HAS_GPIO -DSYSIO_HAS_I2C -DSYS_HAS_SERIAL
endif

ifeq ($(BOARD),BOARD_GENERIC_LINUX)
SYS  = SYS_LINUX
SYS_HAS_SERIAL = ON
ARCH_DIR = sysio/src/arch/generic
CDEFS += -DARCH_LINUX -DSYS_HAS_SERIAL
endif

ifeq ($(BOARD),BOARD_GENERIC_WIN32)
SYS  = SYS_WIN32
ARCH_DIR = sysio/src/arch/x86
CDEFS += -DARCH_X86
endif

ifeq ($(BOARD),ARCH_WIN64)
SYS  = SYS_WIN32
ARCH_DIR = sysio/src/arch/x86_64
CDEFS += -DARCH_X86_64
endif

ifeq ($(SYS),)
$(error BOARD is not set correctly)
endif

ifeq ($(SYS),SYS_LINUX)
CDEFS += -DSYS_LINUX
SYS_DIR = sysio/src/linux
SYS_UNIX = ON
endif

ifeq ($(SYS),SYS_WIN32)
CDEFS += -DSYS_WIN32
SYS_DIR = sysio/src/win32
endif

SRC += $(addprefix sysio/src/, $(notdir $(wildcard $(SYSIO_ROOT)/sysio/src/*.c)))
SRC += $(addprefix 3rdparty/ssdv/src/, $(notdir $(wildcard $(SYSIO_ROOT)/3rdparty/ssdv/src/*.c)))
SRC += $(addprefix 3rdparty/modbus/src/, $(notdir $(wildcard $(SYSIO_ROOT)/3rdparty/modbus/src/*.c)))
SRC += $(addprefix $(SYS_DIR)/, $(notdir $(wildcard $(SYSIO_ROOT)/$(SYS_DIR)/*.c)))

ifeq ($(SYS_HAS_GPIO),ON)
SRC += $(addprefix sysio/src/gpio/, $(notdir $(wildcard $(SYSIO_ROOT)/sysio/src/gpio/*.c)))
endif

ifeq ($(SYS_HAS_I2C),ON)
SRC += $(addprefix sysio/src/i2c/, $(notdir $(wildcard $(SYSIO_ROOT)/sysio/src/i2c/*.c)))
SRC += $(addprefix chipio/src/, $(notdir $(wildcard $(SYSIO_ROOT)/chipio/src/*.c)))
endif

ifeq ($(SYS_HAS_SPI),ON)
SRC += $(addprefix sysio/src/spi/, $(notdir $(wildcard $(SYSIO_ROOT)/sysio/src/spi/*.c)))
endif

ifeq ($(SYS_UNIX),ON)
SRC += $(addprefix sysio/src/unix/, $(notdir $(wildcard $(SYSIO_ROOT)/sysio/src/unix/*.c)))
SRC += $(addprefix radio/src/, $(notdir $(wildcard $(SYSIO_ROOT)/radio/src/*.c)))
endif

SRC += $(addprefix $(ARCH_DIR)/, $(notdir $(wildcard $(SYSIO_ROOT)/$(ARCH_DIR)/*.c)))

#--- Debug
#$(warning ARCH_DIR=$(ARCH_DIR))
#$(warning SYS_DIR=$(SYS_DIR))
#$(warning SRC=$(SRC))

#-------------------------------------------------------------------------------
