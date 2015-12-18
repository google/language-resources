package(default_visibility = ["//visibility:public"])

cc_library(
    name = "gtest",
    srcs = glob(
        [
            "googletest/src/*.cc",
            "googletest/src/*.h",
        ],
        exclude = ["googletest/src/gtest-all.cc"],
    ),
    hdrs = glob(["googletest/include/**/*.h"]),
    copts = ["-Iexternal/googletest/googletest"],
    includes = ["googletest/include"],
    linkopts = ["-pthread"],
)

cc_library(
    name = "gtest_main",
    testonly = 1,
    srcs = ["googletest/src/gtest_main.cc"],
    deps = [":gtest"],
)
