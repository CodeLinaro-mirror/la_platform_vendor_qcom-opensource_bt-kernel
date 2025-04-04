load("//build/bazel_common_rules/dist:dist.bzl", "copy_to_dist_dir")
load("//msm-kernel:target_variants.bzl", "get_all_variants")
load("//build/kernel/kleaf:kernel.bzl", "ddk_module")

def bt_modules(target, variant):
    kernel_build_variant = "{}_{}".format(target, variant)
    include_base = "../../../{}".format(native.package_name())

    include_defconfig = ":{}_defconfig".format(variant)

    mod_list = []

    ddk_module(
        name = "{}-defconfig_btpower".format(kernel_build_variant),
        out = "btpower.ko",
        srcs = [
            "pwr/btpower.c",
        ],
        hdrs = [
            "include/btpower.h",
        ],
		includes = ["include"],
    kernel_build = "//msm-kernel:{}-defconfig".format(kernel_build_variant),
        deps = [
            "//msm-kernel:all_headers_arm",
        ],
    )
    mod_list.append("{}-defconfig_btpower".format(kernel_build_variant))

    copy_to_dist_dir(
        name = "{}-defconfig_btpower_dist".format(kernel_build_variant),
        data = mod_list,
        dist_dir = "out/target/product/{}/dlkm/lib/modules/".format(target),
        flat = True,
        wipe_dist_dir = False,
        allow_duplicate_filenames = False,
        mode_overrides = {"**/*": "644"},
        log = "info",
    )
