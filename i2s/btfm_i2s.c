// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 */

#include <linux/module.h>
#include <linux/of.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <sound/core.h>
#include "btfm_i2s.h"
#include "btfm_i2s_hw_interface.h"

/* Global driver state - accessed by DAI ops in btfm_i2s_hw_interface.c */
struct btfmi2s *pbtfmi2s;

static int __init btfm_i2s_init(void)
{
	struct device_node *np;
	int ret = 0;

	BTFMI2S_INFO("+++++ BTFM I2S module init start +++++");

	np = of_find_compatible_node(NULL, NULL, "qcom,btfmi2s_support");
	if (!np) {
		BTFMI2S_INFO("I2S_SUPPORT node NOT found - skipping");
		BTFMI2S_INFO("+++++ BTFM I2S module init done (no-op) +++++");
		return 0;
	}

	BTFMI2S_INFO("I2S_SUPPORT node found: %s", np->full_name);

	if (!of_property_read_bool(np, "enable-I2S")) {
		BTFMI2S_INFO("enable-I2S property NOT found - skipping registration");
		goto out;
	}
	BTFMI2S_INFO("enable-I2S property found");

	pbtfmi2s = kzalloc(sizeof(struct btfmi2s), GFP_KERNEL);
	if (!pbtfmi2s) {
		BTFMI2S_ERR("kzalloc failed for btfmi2s");
		ret = -ENOMEM;
		goto out;
	}
	BTFMI2S_DBG("btfmi2s state allocated at %p", pbtfmi2s);

	/*
	 * dev is left NULL: the i2s module is not backed by a bus
	 * device. btfm_register_codec() uses btfmcodec->dev for
	 * snd_soc_register_component, so hwep_info->dev being NULL
	 * is safe for registration.
	 */
	pbtfmi2s->dev = NULL;
	pbtfmi2s->initialized = false;

	BTFMI2S_DBG("calling btfm_i2s_register_hw_ep");
	ret = btfm_i2s_register_hw_ep(pbtfmi2s);
	if (ret) {
		BTFMI2S_ERR("ALSA registration FAILED ret=%d", ret);
		kfree(pbtfmi2s);
		pbtfmi2s = NULL;
	} else {
		BTFMI2S_INFO("ALSA registration SUCCESS");
	}

out:
	of_node_put(np);
	BTFMI2S_INFO("+++++ BTFM I2S module init done, ret=%d +++++", ret);
	return ret;
}

static void __exit btfm_i2s_exit(void)
{
	BTFMI2S_INFO("BTFM I2S module exit, pbtfmi2s=%p", pbtfmi2s);

	if (pbtfmi2s) {
		btfm_i2s_unregister_hwep();
		kfree(pbtfmi2s);
		pbtfmi2s = NULL;
	}
}

late_initcall(btfm_i2s_init);
module_exit(btfm_i2s_exit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("BTFM I2S driver");
