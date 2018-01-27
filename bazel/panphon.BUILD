package(default_visibility = ["//visibility:public"])

licenses(["notice"])  # MIT License in file "LICENSE.txt"

exports_files(glob(["panphon/data/*"]))

genrule(
    name = "dummy_init_py",
    outs = ["__init__.py"],
    cmd = "touch $@",
)

genrule(
    name = "cp_ipa_all_csv",
    srcs = ["panphon/data/ipa_all.csv"],
    outs = ["data/ipa_all.csv"],
    cmd = "cp $< $@",
)

py_library(
    name = "panphon_data",
    srcs = ["__init__.py"],
    data = ["data/ipa_all.csv"],
)
