ifeq ($(TARGET_BOARD_PLATFORM), msmnile)
    BOARD_VENDOR_KERNEL_MODULES += $(KERNEL_MODULES_OUT)/btpower_new.ko
endif
