load("//build/bazel_common_rules/dist:dist.bzl", "copy_to_dist_dir")
load("//build/kernel/kleaf:kernel.bzl", "ddk_module")

def bt_modules_lv(target, variant):
    kernel_build_variant = "{}_{}".format(target, variant)

    mod_list = []

    ddk_module(
        name = "{}_btpower".format(kernel_build_variant),
        out = "btpower.ko",
        srcs = [
            "pwr/btpower.c",
        ],
        hdrs = [
            "include/btpower.h",
        ],
        includes = ["include"],
        kernel_build = select({
            "//build/kernel/kleaf:socrepo_true": "//soc-repo:{}_base_kernel".format(kernel_build_variant),
            "//build/kernel/kleaf:socrepo_false": "//msm-kernel:{}".format(kernel_build_variant),
        }),

        deps = select({
            "//build/kernel/kleaf:socrepo_true": ["//soc-repo:all_headers"],
            "//build/kernel/kleaf:socrepo_false": ["//msm-kernel:all_headers_arm"],
        }),
    )

    mod_list.append("{}_btpower".format(kernel_build_variant))

    copy_to_dist_dir(
        name = "{}_btpower_dist".format(kernel_build_variant),
        data = mod_list,
        dist_dir = "out/target/product/{}/dlkm/lib/modules/".format(target),
        flat = True,
        wipe_dist_dir = False,
        allow_duplicate_filenames = False,
        mode_overrides = {"**/*": "644"},
        log = "info",
    )
