ccflags-y += -I$(BT_ROOT)/include
ifeq ($(CONFIG_MSM_BT_CONVERGED), y)
ccflags-y += -I$(BT_ROOT)/../wlan/platform/inc
ccflags-y += -I$(BT_ROOT)/../wlan/platform/cnss_utils
endif

ifeq ($(CONFIG_BT_HW_SECURE_DISABLE), y)
ccflags-y += -I$(BT_ROOT)/../securemsm-kernel/smcinvoke/
ccflags-y += -I$(BT_ROOT)/../securemsm-kernel/linux/
endif

obj-$(CONFIG_MSM_BT_POWER_NEW)  += btpower_new.o
