package(default_visibility = ["//visibility:public"])

licenses(["notice"])  # Unicode License Agreement - Data Files and Software

exports_files([
    "xh-xh_FONIPA.txt",
    "zu-zu_FONIPA.txt",
])

load("build_defs", "transform_test")

transform_test("xh")

transform_test("zu")
