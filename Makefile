KERNEL_SRC ?= /lib/modules/$(shell uname -r)/build
M ?= $(shell pwd)
# BT_ROOT must point to the module source directory so that sub-makefiles
# (e.g. pwr/Makefile) can locate headers under include/.
# In an Android/Bazel build M is a relative path appended to KERNEL_SRC, so
# the combined form is needed there.  For every other build (standalone,
# DKMS, debian package) M is already an absolute path, so use it directly.
ifdef ANDROID_BUILD_TOP
BT_ROOT=$(KERNEL_SRC)/$(M)
else
BT_ROOT=$(M)
endif

KBUILD_OPTIONS += BT_ROOT=$(BT_ROOT)
KBUILD_OPTIONS += MODNAME=$(MODNAME)

ifeq ($(CONFIG_MSM_BT_CONVERGED), y)
KBUILD_EXTRA_SYMBOLS=$(call intermediates-dir-for,DLKM,wlan-platform-module-symvers)/Module.symvers
KBUILD_EXTRA_SYMBOLS=$(OUT_DIR)/vendor/qcom/wlan/platform/Module.symvers
ccflags-y += -I$(BT_ROOT)/../wlan/platform/inc
endif

all: modules

modules:
	$(MAKE) -C $(KERNEL_SRC) M=$(M) $@ $(KBUILD_OPTIONS)

modules_install:
	$(MAKE) INSTALL_MOD_STRIP=1 -C $(KERNEL_SRC) M=$(M) modules_install

clean:
	$(MAKE) -C $(KERNEL_SRC) M=$(M) clean
