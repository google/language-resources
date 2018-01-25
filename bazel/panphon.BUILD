package(default_visibility = ["//visibility:public"])

licenses(["notice"])  # MIT License in file "LICENSE.txt"

exports_files(glob(["panphon/data/*"]))

genrule(
    name = "dummy_init_py",
    outs = ["__init__.py"],
    cmd = "touch $@",
)

py_library(
    name = "panphon_data",
    srcs = [":dummy_init_py"],
    data = glob(["panphon/data/*"]),
)

py_library(
    name = "panphon",
    srcs = glob(["panphon/*.py"]),
    data = glob(["panphon/data/*"]),
)

py_binary(
    name = "align_wordlists",
    srcs = ["panphon/bin/align_wordlists.py"],
    deps = [":panphon"],
)

py_binary(
    name = "generate_ipa_all",
    srcs = ["panphon/bin/generate_ipa_all.py"],
    deps = [":panphon"],
)

py_binary(
    name = "validate_ipa",
    srcs = ["panphon/bin/validate_ipa.py"],
    deps = [":panphon"],
)

genrule(
    name = "regenerate_ipa_all_csv",
    srcs = [
        "panphon/data/diacritic_definitions.yml",
        "panphon/data/sort_order.yml",
        "panphon/data/ipa_bases.csv",
    ],
    outs = ["regenerated_ipa_all.csv"],
    cmd = """
        $(location :generate_ipa_all) \
          -d $(location panphon/data/diacritic_definitions.yml) \
          -s $(location panphon/data/sort_order.yml) \
          $(location panphon/data/ipa_bases.csv) \
          $@
    """,
    tools = [":generate_ipa_all"],
)
