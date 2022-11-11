ccflags-y += -I$(BT_ROOT)/include
ifeq ($(CONFIG_MSM_BT_CONVERGED), y)
ccflags-y += -I$(BT_ROOT)/../wlan/platform/inc
ccflags-y += -I$(BT_ROOT)/../wlan/platform/cnss_utils
endif
obj-$(CONFIG_MSM_BT_POWER)  += btpower.o
