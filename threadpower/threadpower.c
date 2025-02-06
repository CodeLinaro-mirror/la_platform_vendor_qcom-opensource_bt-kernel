// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2025 Qualcomm Innovation Center, Inc. All rights reserved.
 */

#include "threadpower.h"

//--------------[FUNCTION PROTO TYPE]---------------------------------------------------
static int vreg_enable(struct vreg_data *vreg);
static int vreg_disable(struct vreg_data *vreg);
static int vreg_enable_retention(struct vreg_data *vreg);
static int vreg_disable_retention(struct vreg_data *vreg);
static long thread_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
static int thread_power_probe(struct platform_device *pdev);

//--------------[VARIABLE DECLARTION]---------------------------------------------------

static struct vreg_data thread_resources[] = {
	{NULL, "qcom,resource1", 0, 0, 0, 0, 0, {0, 0}},
	{NULL, "qcom,resource2", 0, 0, 0, 0, 0, {0, 0}},
	{NULL, "qcom,resource3", 0, 0, 0, 0, 0, {0, 0}},
};

static const struct file_operations thread_fops = {
	.unlocked_ioctl = thread_ioctl,
	.compat_ioctl = thread_ioctl
};

static struct pwr_data thread_node_info = {
	.compatible = "qcom,thread_node",
	.reg_resource = thread_resources,
	.num_reg_resource = ARRAY_SIZE(thread_resources)
};

static const struct of_device_id thread_power_dt[] = {
	{
		.compatible = "qcom,thread_node",
		.data = &thread_node_info
	},
	{}
};

static struct platform_driver thread_power_driver = {
	.driver = {
		.name = "thread_power",
		.of_match_table = thread_power_dt,
	},
	.probe = thread_power_probe,
};

static int (*voltage_regulator_handler[THREAD_PWR_MAX_OPTION])(struct vreg_data *) = {
	vreg_enable,
	vreg_disable,
	vreg_enable_retention,
	vreg_disable_retention
};

char *default_crash_reason = "Crash reason not found";
static struct thread_pwr_struct *thrd_dev;
static dev_t threadpower_drv_id;
struct cdev thread_cdev;
static struct class *thread_class;

//--------------[FUNCTIONS]---------------------------------------------------

/**
 * vreg_configure() - this api handles the regulator configuration
 *
 * @arg: regulator info
 *
 * Context:
 *  - this api set the regulator voltage
 *  - this api set the load current
 *
 * Return: 0 on success.
 *         error code on failure.
 */
static int vreg_configure(struct vreg_data *vreg, bool pull_down)
{
	int rc = 0;

	THREAD_ERR(" %s", vreg->name);

	if ((vreg->min_vol != 0) && (vreg->max_vol != 0)) {
		rc = regulator_set_voltage(vreg->reg, (pull_down ? 0 : vreg->min_vol),
					   vreg->max_vol);
		if (rc < 0) {
			THREAD_ERR("regulator_set_voltage(%s) failed rc=%d", vreg->name, rc);
			return rc;
		}
	}

	if (vreg->load_curr >= 0) {
		rc = regulator_set_load(vreg->reg, (pull_down ? 0 : vreg->load_curr));
		if (rc < 0) {
			THREAD_ERR("regulator_set_load(%s) failed rc=%d", vreg->name, rc);
			return rc;
		}
	}
	return rc;
}

/**
 * vreg_enable() - this api handles the regulator enable
 *
 * @arg: regulator info
 *
 * Context:
 *  - this api helps to enable regulator
 *
 * Return: 0 on success.
 *         error code on failure.
 */
static int vreg_enable(struct vreg_data *vreg)
{
	int rc = 0;

	if (!vreg)
		return rc;

	THREAD_ERR(" %s", vreg->name);

	if (vreg->is_enabled) {
		THREAD_ERR("Regulator: %s is alreday enabled, returning from here",
			vreg->name);
		return -EPERM;
	}

	rc = vreg_configure(vreg, false);
	if (rc < 0)
		return rc;

	rc = regulator_enable(vreg->reg);
	if (rc < 0) {
		THREAD_ERR("regulator_enable(%s) failed. rc=%d", vreg->name, rc);
		return rc;
	}

	vreg->is_enabled = true;
	return rc;
}

/**
 * vreg_disable() -  this api handles the regulator disable
 *
 * @arg: regulator info
 *
 * Context:
 *  - this api disable regulator voltage
 *  - this api set the regulator voltage to zero
 *  - this api set the load current to zero
 *
 * Return: 0 on success.
 *         error code on failure.
 */
static int vreg_disable(struct vreg_data *vreg)
{
	int rc = 0;

	if (!vreg)
		return rc;

	THREAD_ERR(" %s", vreg->name);

	if (!vreg->is_enabled) {
		THREAD_ERR("Regulator: %s is alreday disabled, returning from here",
			vreg->name);
		return -EPERM;
	}

	rc = regulator_disable(vreg->reg);
	if (rc < 0) {
		THREAD_ERR("regulator_disable(%s) failed. rc=%d", vreg->name, rc);
		return rc;
	}

	vreg->is_enabled = false;

	return vreg_configure(vreg, true);
}

/**
 * vreg_enable_retention() -  this api handles the regulator retention enable
 *
 * @arg: regulator info
 *
 * Context:
 *  - this api helps to enable regulator retention
 *
 * Return: 0 on success.
 *         error code on failure.
 */
static int vreg_enable_retention(struct vreg_data *vreg)
{
	int rc = 0;

	if (!vreg)
		return rc;

	THREAD_ERR(" %s", vreg->name);

	if ((vreg->is_enabled) && (vreg->is_retention_supp))
		if ((vreg->min_vol != 0) && (vreg->max_vol != 0))
			rc = vreg_configure(vreg, true);

	return rc;
}

/**
 * vreg_disable_retention() -  this api handles the regulator retention disable
 *
 * @arg: regulator info
 *
 * Context:
 *  - this api helps to disable regulator retention
 *
 * Return: 0 on success.
 *         error code on failure.
 */
static int vreg_disable_retention(struct vreg_data *vreg)
{
	int rc = 0;

	if (!vreg)
		return rc;

	THREAD_ERR(" %s", vreg->name);

	if ((vreg->is_enabled) && (vreg->is_retention_supp))
		rc = vreg_configure(vreg, false);

	return rc;
}

/**
 * ConvertPowerOptionToString() - this api covert power vote option into string
 *
 * @arg: power vote option.
 *
 * Context:
 *  - this api covert power vote option into string.
 *
 * Return: return the coresponding poer vote oprtion string.
 */
static inline char *ConvertPowerOptionToString(int arg)
{
	switch (arg) {
	case POWER_ON:
		return "POWER_ON";
	case POWER_OFF:
		return "POWER_OFF";
	case POWER_RETENION_EN:
		return "POWER_RETENION_ENABLE";
	case POWER_RETENION_DIS:
		return "POWER_RETENION_DISABLE";
	default:
		return "INVALID STATE";
	}
};

/**
 * thread_power_vote_handler() - this api handles all the power
 * vote request
 *
 * @arg1: power vote request
 *
 * Context:
 *  - this api queues all the power vote POWER ON, POWER OFF,
 * POWER RETENION ENABLE, POWER RETENTION DISABLE request.
 *
 * Return: 0 on success.
 *         error code on failure.
 */
static int thread_power_vote_handler(uint32_t option)
{
	int ret = 0;

	for (int i = 0; i < ARRAY_SIZE(thread_resources); i++) {
		ret = voltage_regulator_handler[option](&thread_resources[i]);
		if (ret) {
			THREAD_ERR(" Failed to perform option = %d, for the regulator = %s",
				option, (&thread_resources[i])->name);
			return ret;
		}
	}
	return ret;
}

/**
 * thread_power_kernel_panic() - this api perfrom the kernel panic based on client req
 *
 * @arg: Crash reason info struct
 *
 * Context:
 *  - this api perfrom the kernel panic based on client req
 *
 * Return: Whole system will put down on success.
 *         error code on failure.
 */
int thread_power_kernel_panic(char *arg)
{
	int ret = 0;
	struct {
		char PrimaryReason[50];
		char SecondaryReason[100];
	} CrashInfo;

	if (copy_from_user(&CrashInfo, (char *)arg, sizeof(CrashInfo))) {
		THREAD_ERR("Failed copy to panic reason from THREAD-HAL");
		memset(&CrashInfo, 0, sizeof(CrashInfo));
		strscpy(CrashInfo. PrimaryReason,
			default_crash_reason, strlen(default_crash_reason));
		strscpy(CrashInfo. SecondaryReason,
			default_crash_reason, strlen(default_crash_reason));
		ret = -EFAULT;
	}

	THREAD_ERR("kernel panic Primary reason = %s, Secondary reason = %s",
		CrashInfo.PrimaryReason, CrashInfo.SecondaryReason);

	panic("THERAD kernel panic Primary reason = %s, Secondary reason = %s",
		CrashInfo.PrimaryReason, CrashInfo.SecondaryReason);

	return ret;
}

/**
 * thread_ioctl() - its an interface api for executing
 * device-specific commands from user space
 * @arg1: pointer to the file structure representing
 *        the open file descriptor
 * @arg2: this parameter specifies the command to be
 *        executed
 * @arg3: this parameter is used to pass additional data
 *        to the command specified by cmd.
 *
 * Context:
 *  - this api recvice the cmd from the user and serve the request
 *
 * Return: 0 on success.
 *         error code on failure.
 */
static long thread_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int ret = -1;
	uint32_t power_option;

	if (!thrd_dev->is_probe_pending) {
		THREAD_ERR("Probe is pending, returning from here");
		return (long)ret;
	}

	switch (cmd) {
	case THREAD_CMD_PWR_VOTE:
		power_option = (uint32_t)arg;
		if ((power_option >= THREAD_PWR_MAX_OPTION) && (power_option < 0)) {
			THREAD_ERR("Inalid user command recived from client");
			return ret;
		}
		THREAD_ERR("THREAD_CMD_PWR_VOTE to %s",
			ConvertPowerOptionToString(power_option));
		ret = thread_power_vote_handler(power_option);
		break;
	case THREAD_CMD_GET_REG_VLTG:
		THREAD_ERR("TODO : THREAD_CMD_GET_REG_VLTG");
		break;
	case THREAD_CMD_GET_CHIPSET_ID:
		THREAD_ERR("THREAD_CMD_GET_CHIPSET_ID");
		THREAD_ERR("Chip id = %s", thrd_dev->chip_set_id);
		if (copy_to_user((void __user *)arg,
				thrd_dev->chip_set_id, MAX_PROP_SIZE)) {
			THREAD_ERR("copy to user failed");
			ret = -EFAULT;
		} else
			ret = 0;
		break;
	case THREAD_CMD_UPDATE_CHIPSET_VER:
		THREAD_ERR("THREAD_CMD_UPDATE_CHIPSET_VER");
		thrd_dev->chipset_version = (int)arg;
		if (thrd_dev->chipset_version) {
			THREAD_ERR("SOC Version : %x", thrd_dev->chipset_version);
			ret = 0;
		} else {
			THREAD_ERR("got invalid soc version");
		}
		break;
	case THREAD_CMD_PANIC:
		THREAD_ERR("THREAD_CMD_UPDATE_CHIPSET_VER");
		ret = thread_power_kernel_panic((char *)arg);
		break;
	default:
		THREAD_ERR("default");
		break;
	}
	THREAD_ERR("request completed");
	return (long)ret;
}

/**
 * thread_power_get_reg_info_from_dt() - this api helps to get the
 * regulator resource info
 *
 * @arg1: pointer to dev node
 * @arg2: pointer to regulator node
 *
 * Context:
 *  - this api helps to get the regulator resource info from the DTBO
 *
 * Return: 0 on success.
 *         error code on failure.
 */
static int thread_power_get_reg_info_from_dt(struct device *dev,
	struct vreg_data *vreg_data)
{

	int len, ret = 0;
	const __be32 *prop;
	char prop_name[MAX_PROP_SIZE];
	struct vreg_data *vreg = vreg_data;

	THREAD_ERR("vreg dev tree parse for %s", vreg_data->name);

	snprintf(prop_name, sizeof(prop_name), "%s-supply", vreg_data->name);
	if (of_parse_phandle(dev->of_node, prop_name, 0)) {
		vreg->reg = regulator_get(dev, vreg_data->name);
		if (IS_ERR(vreg->reg)) {
			ret = PTR_ERR(vreg->reg);
			vreg->reg = NULL;
			THREAD_ERR("failed to get: %s error:%d", vreg_data->name, ret);
			return ret;
		}

		snprintf(prop_name, sizeof(prop_name), "%s-config", vreg->name);
		prop = of_get_property(dev->of_node, prop_name, &len);
		if (!prop) {
			THREAD_ERR("Property %s %s, use default",
				prop_name, prop ? "invalid format" : "doesn't exist");
		} else if (len >= (4 * sizeof(__be32))) {
			vreg->min_vol = be32_to_cpup(&prop[0]);
			vreg->max_vol = be32_to_cpup(&prop[1]);
			vreg->load_curr = be32_to_cpup(&prop[2]);
			vreg->is_retention_supp = be32_to_cpup(&prop[3]);
		}

		THREAD_ERR("Regulator: %s, min_vol: %u, max_vol: %u, load_curr: %u, retention: %u",
			vreg->name, vreg->min_vol, vreg->max_vol,
			vreg->load_curr, vreg->is_retention_supp);
	} else {
		THREAD_ERR("%s is not provided in device tree",  vreg_data->name);
	}

	return ret;
}

/**
 * thread_power_decode_dt_handler() - this api helps to get the
 * resource info
 *
 * @arg1: pointer to dev node
 *
 * Context:
 *  - this api helps to get the resource info from the DTBO
 *
 * Return: 0 on success.
 *         error code on failure.
 */
static int thread_power_decode_dt_handler(struct platform_device *pdev)
{
	int rc;
	const struct pwr_data *data;

	THREAD_ERR("");

	data = of_device_get_match_data(&pdev->dev);
	if (!data) {
		THREAD_ERR("failed to get dev node");
		return -EINVAL;
	}

	memcpy(&thrd_dev->chip_set_id, &data->compatible, MAX_PROP_SIZE);

	THREAD_ERR("chip_set_id = %s, compatible=%s", thrd_dev->chip_set_id, data->compatible);

	for (int i = 0; i < ARRAY_SIZE(thread_resources); i++) {
		rc = thread_power_get_reg_info_from_dt(&(pdev->dev), &thread_resources[i]);
		/* No point to go further if failed to get regulator handler */
		if (rc)
			return rc;
	}
	return rc;
}

/**
 * thread_power_probe() - this api detect and initialize the driver
 *
 * @arg1: pointer to dev node
 *
 * Context:
 *  - this api detect and initialize the driver
 *
 * Return: 0 on success.
 *         error code on failure.
 */
static int thread_power_probe(struct platform_device *pdev)
{
	THREAD_ERR("Start");
	thrd_dev = kzalloc(sizeof(*thrd_dev), GFP_KERNEL);

	if (!thrd_dev)
		return -ENOMEM;

	thrd_dev->is_probe_pending = false;

	thread_power_decode_dt_handler(pdev);

	thrd_dev->is_probe_pending = true;

	THREAD_ERR("Completed");
	return 0;
}

/**
 * thread_power_init() - register the Thread module
 *
 * @arg: none
 *
 * Context:
 *  - this api will takes care of driver registration.
 *
 * Return: 0 on success.
 *         error code on failure.
 */
static int __init thread_power_init(void)
{
	int ret = 0;

	THREAD_ERR("starting up the module");

	ret = platform_driver_register(&thread_power_driver);
	if (ret) {
		THREAD_ERR("platform_driver_register error: %d", ret);
		goto driver_err;
	}

	ret = alloc_chrdev_region(&threadpower_drv_id, 0, 1, "thread");
	if (ret < 0) {
		THREAD_ERR("Failed to allocate Driver MAJOR NUM");
		goto driver_err;
	}

	cdev_init(&thread_cdev, &thread_fops);
	thread_cdev.owner = THIS_MODULE;
	ret = cdev_add(&thread_cdev, threadpower_drv_id, 1);
	if (ret) {
		THREAD_ERR("Failed to add thread_dev to the system");
		goto unregister_chrdev;
	}

	thread_class = class_create("thread-dev");
	if (IS_ERR(thread_class)) {
		THREAD_ERR("coudn't create class");
		ret = -1;
		goto delete_cdev;
	}
	THREAD_ERR("class_create sucessfull");

	if (device_create(thread_class, NULL, threadpower_drv_id,
		NULL, "threadpower") == NULL) {
		THREAD_ERR("failed to allocate char dev");
		goto destroy_class;
	}
	THREAD_ERR("Thread power driver insert sucessfull");
	return 0;

destroy_class:
	class_destroy(thread_class);
delete_cdev:
	cdev_del(&thread_cdev);
unregister_chrdev:
	unregister_chrdev_region(MAJOR(threadpower_drv_id), 1);
driver_err:
	return ret;
}

/**
 * thread_power_exit() - unregister the module
 *
 * @arg: none
 *
 * Context:
 *  - this api will takes care of driver unregister.
 *
 * Return: none
 */
static void __exit thread_power_exit(void)
{
	class_destroy(thread_class);
	cdev_del(&thread_cdev);
	unregister_chrdev_region(MAJOR(threadpower_drv_id), 1);
}

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("MSM Thread Power Resource Handling Driver");

module_init(thread_power_init);
module_exit(thread_power_exit);
