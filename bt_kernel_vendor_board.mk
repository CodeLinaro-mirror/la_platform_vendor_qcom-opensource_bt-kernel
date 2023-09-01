# Build audio kernel driver
ifneq (,$(call is-board-platform-in-list2, msmnile sm6150 gen4))
  BOARD_VENDOR_KERNEL_MODULES += $(KERNEL_MODULES_OUT)/btpower.ko
  ifeq ($(BOARD_HAVE_DUAL_BLUETOOTH),true)
    BOARD_VENDOR_KERNEL_MODULES += $(KERNEL_MODULES_OUT)/btpower_new.ko
  endif
endif
