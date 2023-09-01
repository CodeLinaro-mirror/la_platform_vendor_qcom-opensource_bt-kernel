ifeq ($(CONFIG_MSM_BT_POWER),m)
KBUILD_CPPFLAGS += -DCONFIG_MSM_BT_POWER
endif

ccflags-y += -I$(BT_KERNEL_ROOT)/include

ifneq ($(MODNAME), btpower)
KBUILD_CPPFLAGS += -DQCA_AUTO_SECONDARY
endif

obj-$(CONFIG_MSM_BT_POWER) += $(MODNAME).o
$(MODNAME)-y = pwr/btpower.o
