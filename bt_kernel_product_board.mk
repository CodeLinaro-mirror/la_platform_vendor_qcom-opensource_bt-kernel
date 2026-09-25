# Build BT kernel drivers

ifeq ($(TARGET_USES_QMAA_OVERRIDE_BLUETOOTH), true)
PRODUCT_PACKAGES += $(KERNEL_MODULES_OUT)/btpower.ko
<<<<<<< HEAD
ifeq ($(TARGET_BOARD_PLATFORM), sun)
=======
endif
PRODUCT_PACKAGES += $(KERNEL_MODULES_OUT)/radio-i2c-rtc6226-qca.ko

ifeq ($(TARGET_USES_QMAA_OVERRIDE_BLUETOOTH_AUDIO), true)
ifeq ($(TARGET_BOARD_PLATFORM), sun canoe chora malabar)
>>>>>>> origin/bt-kernel.lnx.16.0
PRODUCT_PACKAGES += $(KERNEL_MODULES_OUT)/btfmcodec.ko
PRODUCT_PACKAGES += $(KERNEL_MODULES_OUT)/btfm_slim_codec.ko
ifneq ($(TARGET_BOARD_PLATFORM), malabar)
PRODUCT_PACKAGES += $(KERNEL_MODULES_OUT)/bt_fm_swr.ko
endif
else ifeq ($(TARGET_BOARD_PLATFORM),seraph)
PRODUCT_PACKAGES += $(KERNEL_MODULES_OUT)/btfmcodec.ko
PRODUCT_PACKAGES += $(KERNEL_MODULES_OUT)/bt_fm_swr.ko
else ifeq ($(TARGET_BOARD_PLATFORM), volcano)
PRODUCT_PACKAGES += $(KERNEL_MODULES_OUT)/btfmcodec.ko
PRODUCT_PACKAGES += $(KERNEL_MODULES_OUT)/bt_fm_swr.ko
PRODUCT_PACKAGES += $(KERNEL_MODULES_OUT)/bt_fm_slim.ko
else
PRODUCT_PACKAGES += $(KERNEL_MODULES_OUT)/bt_fm_slim.ko
endif
<<<<<<< HEAD
ifeq ($(TARGET_USES_QTI_UWB), true)
PRODUCT_PACKAGES += $(KERNEL_MODULES_OUT)/spi_cnss_proto.ko
=======
endif
ifeq ($(TARGET_USES_QTI_UWB), true)
PRODUCT_PACKAGES += $(KERNEL_MODULES_OUT)/spi_cnss_proto.ko
endif
ifeq ($(BOARD_HAVE_STANDALONE_THREAD), true)
PRODUCT_PACKAGES += $(KERNEL_MODULES_OUT)/thqspi_proto.ko
>>>>>>> origin/bt-kernel.lnx.16.0
endif
