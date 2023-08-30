# Android makefile for BT kernel modules

LOCAL_PATH := $(call my-dir)

# Build/Package only in case of supported target
ifeq ($(call is-board-platform-in-list,taro kalama bengal msmnile gen4), true)

BT_SELECT := CONFIG_MSM_BT_POWER=m
ifneq ($(TARGET_BOARD_AUTO),true)
ifeq ($(TARGET_KERNEL_DLKM_SECURE_MSM_OVERRIDE), true)
ifeq ($(ENABLE_PERIPHERAL_STATE_UTILS), true)
BT_SELECT += CONFIG_BT_HW_SECURE_DISABLE=y
endif
endif
endif # AUTO product check

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
BT_SRC_FILES := $(LOCAL_PATH)/pwr/btpower.c

################################ pwr ################################
include $(CLEAR_VARS)
LOCAL_SRC_FILES           := $(BT_SRC_FILES)
LOCAL_MODULE              := btpower.ko
LOCAL_MODULE_KBUILD_NAME  := pwr/btpower.ko
LOCAL_MODULE_TAGS         := optional
LOCAL_MODULE_DEBUG_ENABLE := true
LOCAL_MODULE_PATH         := $(KERNEL_MODULES_OUT)
include $(DLKM_DIR)/Build_external_kernelmodule.mk

endif # DLKM check
endif # supported target check
