# Android makefile for BT kernel modules

LOCAL_PATH := $(call my-dir)

# Build/Package only in case of supported target
ifeq ($(call is-board-platform-in-list,taro kalama pineapple msmnile sm6150 gen4), true)

BT_SELECT := CONFIG_MSM_BT_POWER=m
LOCAL_PATH := $(call my-dir)

# This makefile is only for DLKM
ifneq ($(findstring vendor,$(LOCAL_PATH)),)

ifneq ($(findstring opensource,$(LOCAL_PATH)),)
	BT_BLD_DIR := $(abspath .)/vendor/qcom/opensource/bt-kernel
endif # opensource

LOCAL_DEV_NAME := $(patsubst .%,%,\
	$(lastword $(strip $(subst /, ,$(LOCAL_PATH)))))

DLKM_DIR := $(TOP)/device/qcom/common/dlkm

LOCAL_MULTI_KO := false

ifeq ($(LOCAL_DEV_NAME), bt-kernel)
LOCAL_MULTI_KO := true
ifeq ($(BOARD_HAVE_DUAL_BLUETOOTH), true)
TARGET_BT_CHIP := btpower btpower_new
else
TARGET_BT_CHIP := btpower
endif
endif # LOCAL_DEV_NAME check

ifeq ($(LOCAL_MULTI_KO), true)

include $(foreach chip, $(TARGET_BT_CHIP), $(LOCAL_PATH)/.$(chip)/Android.mk)

else

###########################################################
# This is set once per LOCAL_PATH, not per (kernel) module
KBUILD_OPTIONS := BT_KERNEL_ROOT=$(BT_BLD_DIR)
KBUILD_OPTIONS += MODNAME=$(LOCAL_DEV_NAME)
KBUILD_OPTIONS += $(foreach bt_select, \
       $(BT_SELECT), \
       $(bt_select))
BT_SRC_FILES := $(LOCAL_PATH)/pwr/btpower.c

ifeq ($(LOCAL_DEV_NAME), btpower)
########################### Module.symvers ############################
include $(CLEAR_VARS)
LOCAL_SRC_FILES           := $(BT_SRC_FILES)
LOCAL_MODULE              := btpower-module-symvers
LOCAL_MODULE_STEM         := Module.symvers
LOCAL_MODULE_KBUILD_NAME  := Module.symvers
LOCAL_MODULE_PATH         := $(KERNEL_MODULES_OUT)
include $(DLKM_DIR)/Build_external_kernelmodule.mk
endif

ifeq ($(LOCAL_DEV_NAME), btpower_new)
KBUILD_OPTIONS += KBUILD_EXTRA_SYMBOLS=$(PWD)/$(call intermediates-dir-for,DLKM,btpower-module-symvers)/Module.symvers
endif

################################ pwr ################################
include $(CLEAR_VARS)
LOCAL_SRC_FILES           := $(BT_SRC_FILES)
LOCAL_MODULE              := $(LOCAL_DEV_NAME).ko
LOCAL_MODULE_KBUILD_NAME  := $(LOCAL_DEV_NAME).ko
LOCAL_MODULE_TAGS         := optional
LOCAL_MODULE_DEBUG_ENABLE := true
LOCAL_MODULE_PATH         := $(KERNEL_MODULES_OUT)
ifeq ($(LOCAL_DEV_NAME), btpower_new)
LOCAL_REQUIRED_MODULES := btpower-module-symvers
LOCAL_ADDITIONAL_DEPENDENCIES += $(call intermediates-dir-for,DLKM,btpower-module-symvers)/Module.symvers
endif
include $(DLKM_DIR)/Build_external_kernelmodule.mk
################################ slimbus ################################

endif # MULTI ko check
endif # DLKM check
endif # supported target check
