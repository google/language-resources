package(default_visibility = ["//visibility:public"])

licenses(["restricted"])  # CC BY-SA 3.0 per file "README.md"

exports_files(glob(["data/*"]))

py_library(
    name = "phoible_data",
    srcs = ["phoible/__init__.py"],  # pro forma only, cannot be imported
    data = glob(["data/*"]),
)
