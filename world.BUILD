package(default_visibility = ["//visibility:public"])

cc_library(
    name = "world",
    srcs = [
        "src/cheaptrick.cpp",
        "src/codec.cpp",
        "src/common.cpp",
        "src/d4c.cpp",
        "src/dio.cpp",
        "src/fft.cpp",
        "src/harvest.cpp",
        "src/matlabfunctions.cpp",
        "src/stonemask.cpp",
        "src/synthesis.cpp",
        "src/synthesisrealtime.cpp",
    ],
    hdrs = [
        "src/world/cheaptrick.h",
        "src/world/codec.h",
        "src/world/common.h",
        "src/world/constantnumbers.h",
        "src/world/d4c.h",
        "src/world/dio.h",
        "src/world/fft.h",
        "src/world/harvest.h",
        "src/world/macrodefinitions.h",
        "src/world/matlabfunctions.h",
        "src/world/stonemask.h",
        "src/world/synthesis.h",
        "src/world/synthesisrealtime.h",
    ],
    includes = ["src"],
)

cc_library(
    name = "tools",
    srcs = [
        "tools/audioio.cpp",
        "tools/parameterio.cpp",
    ],
    hdrs = [
        "tools/audioio.h",
        "tools/parameterio.h",
    ],
    copts = ["-Wno-unused-result"],
    includes = ["."],
)
