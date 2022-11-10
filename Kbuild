# SPDX-License-Identifier: GPL-2.0-only

ifeq ($(BT_ROOT),)
BT_ROOT=$(srctree)/techpack/bt
endif

ifeq ($(CONFIG_MSM_BT_POWER),m)
KBUILD_CPPFLAGS += -DCONFIG_MSM_BT_POWER
endif

ifeq ($(CONFIG_BTFM_SLIM),m)
KBUILD_CPPFLAGS += -DCONFIG_BTFM_SLIM
endif

ifeq ($(CONFIG_I2C_RTC6226_QCA),m)
KBUILD_CPPFLAGS += -DCONFIG_I2C_RTC6226_QCA
endif

USERINCLUDE += -I$(BT_ROOT)/include/ \
	-I$(BT_ROOT)/rtc6226 \
	-I$(BT_ROOT)/slimbus

obj-$(CONFIG_MSM_BT_POWER) += pwr/
obj-$(CONFIG_BTFM_SLIM) += slimbus/
obj-$(CONFIG_I2C_RTC6226_QCA) += rtc6226/
