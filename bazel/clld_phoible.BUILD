package(default_visibility = ["//visibility:public"])

licenses(["restricted"])  # CC BY-SA 3.0 per file "README.md"

exports_files(glob(["data/*"]))

genrule(
    name = "dummy_init_py",
    outs = ["__init__.py"],
    cmd = "touch $@",
)

py_library(
    name = "phoible_data",
    srcs = [":dummy_init_py"],
    data = glob(["data/*"]),
)
