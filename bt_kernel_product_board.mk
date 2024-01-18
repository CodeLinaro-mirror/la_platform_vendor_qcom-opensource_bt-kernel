# Build BT kernel drivers
PRODUCT_PACKAGES += $(KERNEL_MODULES_OUT)/btpower.ko\
	$(KERNEL_MODULES_OUT)/bt_fm_slim.ko \
	$(KERNEL_MODULES_OUT)/radio-i2c-rtc6226-qca.ko
BT_KERNEL_DRIVER := $(KERNEL_MODULES_OUT)/btpower.ko\
             $(KERNEL_MODULES_OUT)/bt_fm_slim.ko \
             $(KERNEL_MODULES_OUT)/radio-i2c-rtc6226-qca.ko
BOARD_VENDOR_KERNEL_MODULES += $(BT_KERNEL_DRIVER)

