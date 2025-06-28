# Build audio kernel driver
ifneq ($(TARGET_BOARD_AUTO),true)
ifeq ($(TARGET_USES_QMAA),true)
  ifeq ($(TARGET_USES_QMAA_OVERRIDE_BLUETOOTH), true)
     ifneq (,$(call is-board-platform-in-list2,$(TARGET_BOARD_PLATFORM)))
         BOARD_VENDOR_KERNEL_MODULES += $(KERNEL_MODULES_OUT)/btpower.ko\
             $(KERNEL_MODULES_OUT)/bt_fm_slim.ko \
	     $(KERNEL_MODULES_OUT)/radio-i2c-rtc6226-qca.ko
     endif
  endif
else
  ifneq (,$(call is-board-platform-in-list2,$(TARGET_BOARD_PLATFORM)))
     BOARD_VENDOR_KERNEL_MODULES += $(KERNEL_MODULES_OUT)/btpower.ko\
            $(KERNEL_MODULES_OUT)/bt_fm_slim.ko \
	    $(KERNEL_MODULES_OUT)/radio-i2c-rtc6226-qca.ko

  endif
endif
else
  ifneq (,$(call is-board-platform-in-list2, msmnile sm6150 gen4 gen5))
    BOARD_VENDOR_KERNEL_MODULES += $(KERNEL_MODULES_OUT)/btpower.ko
  endif
endif
