# Build audio kernel driver
ifeq ($(call is-board-platform-in-list, msmnile gen4), true)
  BOARD_VENDOR_KERNEL_MODULES += $(KERNEL_MODULES_OUT)/btpower.ko
  ifeq ($(BOARD_HAVE_DUAL_BLUETOOTH),true)
    BOARD_VENDOR_KERNEL_MODULES += $(KERNEL_MODULES_OUT)/btpower_new.ko
  endif
endif
