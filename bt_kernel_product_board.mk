# Build BT kernel drivers
PRODUCT_PACKAGES += $(KERNEL_MODULES_OUT)/btpower.ko
ifeq ($(BOARD_HAVE_DUAL_BLUETOOTH),true)
PRODUCT_PACKAGES += $(KERNEL_MODULES_OUT)/btpower_new.ko
endif
