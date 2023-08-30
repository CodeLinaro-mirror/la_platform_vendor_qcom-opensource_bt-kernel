# Build audio kernel driver
ifeq ($(call is-board-platform-in-list, msmnile gen4), true)
  BOARD_VENDOR_KERNEL_MODULES += $(KERNEL_MODULES_OUT)/btpower.ko
endif
