# Android makefile for BT kernel modules

LOCAL_PATH := $(call my-dir)

# Build/Package only in case of supported target
ifeq ($(call is-board-platform-in-list,msmnile), true)

BT_SELECT := CONFIG_MSM_BT_POWER=m

LOCAL_PATH := $(call my-dir)

# This makefile is only for DLKM
ifneq ($(findstring vendor,$(LOCAL_PATH)),)

ifneq ($(findstring opensource,$(LOCAL_PATH)),)
	BT_BLD_DIR := $(abspath .)/vendor/qcom/opensource/bt-kernel
endif # opensource

DLKM_DIR := $(TOP)/device/qcom/common/dlkm


###########################################################
# This is set once per LOCAL_PATH, not per (kernel) module
KBUILD_OPTIONS := BT_KERNEL_ROOT=$(BT_BLD_DIR)
KBUILD_OPTIONS += $(foreach bt_select, \
       $(BT_SELECT), \
       $(bt_select))

BT_SRC_FILES := \
	$(wildcard $(LOCAL_PATH)/*) \
	$(wildcard $(LOCAL_PATH)/*/*) \


################################ pwr ################################
include $(CLEAR_VARS)
LOCAL_SRC_FILES           := $(BT_SRC_FILES)
LOCAL_MODULE              := btpower.ko
LOCAL_MODULE_KBUILD_NAME  := pwr/btpower.ko
LOCAL_MODULE_TAGS         := optional
LOCAL_MODULE_DEBUG_ENABLE := true
LOCAL_MODULE_PATH         := $(TARGET_OUT_VENDOR)/lib/modules
include $(DLKM_DIR)/AndroidKernelModule.mk
###########################################################

endif # DLKM check
endif # supported target check
