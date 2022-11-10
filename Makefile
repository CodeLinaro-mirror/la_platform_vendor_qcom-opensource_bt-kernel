# SPDX-License-Identifier: GPL-2.0-only

BT_ROOT=$(ROOTDIR)/vendor/qcom/opensource/bt-kernel
KBUILD_OPTIONS := BT_ROOT=$(BT_ROOT)
CONFIG_MSM_BT_POWER=$(MODULE_MSM_BT_POWER)
CONFIG_I2C_RTC6226_QCA=$(MODULE_I2C_RTC6226_QCA)
CONFIG_BTFM_SLIM=$(MODULE_BTFM_SLIM)
KBUILD_OPTIONS += CONFIG_MSM_BT_POWER=$(CONFIG_MSM_BT_POWER)
KBUILD_OPTIONS += CONFIG_I2C_RTC6226_QCA=$(CONFIG_I2C_RTC6226_QCA)
KBUILD_OPTIONS += CONFIG_BTFM_SLIM=$(CONFIG_BTFM_SLIM)

KBUILD_EXTRA_SYMBOLS=$(call intermediates-dir-for,DLKM,wlan-platform-module-symvers)/Module.symvers
KBUILD_EXTRA_SYMBOLS=$(OUT_DIR)/vendor/qcom/wlan/platform/Module.symvers
ccflags-y += -I$(WORKSPACE)/wlan/platform/inc

ifeq ($(TARGET_SUPPORT),genericarmv8)
        KBUILD_OPTIONS += CONFIG_ARCH_KALAMA=y
endif

obj-m += pwr/
obj-m += slimbus/
obj-m += rtc6226/

all:
	$(MAKE) -C $(KERNEL_SRC) M=$(M) modules $(KBUILD_OPTIONS)

modules_install:
	$(MAKE) INSTALL_MOD_STRIP=1 -C $(KERNEL_SRC) M=$(M) modules_install

%:
	$(MAKE) -C $(KERNEL_SRC) M=$(M) $@ $(KBUILD_OPTIONS)

clean:
	rm -f *.o *.ko *.mod.c *.mod.o *~ .*.cmd Module.symvers
	rm -rf .tmp_versions
