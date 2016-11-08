package(default_visibility = ["//visibility:public"])

load("@protobuf//:protobuf.bzl", "cc_proto_library")

cc_proto_library(
    name = "sparrowhawk_configuration",
    srcs = ["src/proto/sparrowhawk_configuration.proto"],
    default_runtime = "@protobuf//:protobuf",
    protoc = "@protobuf//:protoc",
    include = "src/proto",
)

cc_proto_library(
    name = "rule_order",
    srcs = ["src/proto/rule_order.proto"],
    default_runtime = "@protobuf//:protobuf",
    protoc = "@protobuf//:protoc",
    include = "src/proto",
)

cc_proto_library(
    name = "links",
    srcs = ["src/proto/links.proto"],
    default_runtime = "@protobuf//:protobuf",
    protoc = "@protobuf//:protoc",
    include = "src/proto",
)

cc_proto_library(
    name = "semiotic_classes",
    srcs = ["src/proto/semiotic_classes.proto"],
    default_runtime = "@protobuf//:protobuf",
    protoc = "@protobuf//:protoc",
    include = "src/proto",
)

cc_proto_library(
    name = "items",
    srcs = ["src/proto/items.proto"],
    default_runtime = "@protobuf//:protobuf",
    protoc = "@protobuf//:protoc",
    include = "src/proto",
    deps = [
      ":links",
      ":semiotic_classes",
    ],
)

genrule(
    name = "make_compatibility_headers",
    outs = [
        "sparrowhawk/items.pb.h",
        "sparrowhawk/rule_order.pb.h",
        "sparrowhawk/sparrowhawk_configuration.pb.h",
    ],
    cmd = """
        for f in $(OUTS); do
          echo "#include <src/proto/$$(basename $$f)>" > $$f
        done
    """,
)

cc_library(
    name = "protos",
    hdrs = [
        "sparrowhawk/items.pb.h",
        "sparrowhawk/rule_order.pb.h",
        "sparrowhawk/sparrowhawk_configuration.pb.h",
    ],
    includes = ["."],
    deps = [
        ":items",
        ":rule_order",
        ":sparrowhawk_configuration",
    ],
)

cc_library(
    name = "utils",
    srcs = [
        "src/lib/io_utils.cc",
        "src/lib/string_utils.cc",
    ],
    hdrs = [
        "src/include/sparrowhawk/io_utils.h",
        "src/include/sparrowhawk/logger.h",
        "src/include/sparrowhawk/string_utils.h",
    ],
    copts = ["-Wno-sign-compare"],
    includes = ["src/include"],
    visibility = ["//visibility:private"],
)

cc_library(
    name = "regexp",
    srcs = ["src/lib/regexp.cc"],
    hdrs = ["src/include/sparrowhawk/regexp.h"],
    includes = ["src/include"],
    visibility = ["//visibility:private"],
    deps = [
        "@openfst//:base",
        "//external:re2",
        ":utils",
    ],
)

cc_library(
    name = "sentence_boundary",
    srcs = ["src/lib/sentence_boundary.cc"],
    hdrs = ["src/include/sparrowhawk/sentence_boundary.h"],
    copts = ["-Wno-sign-compare"],
    includes = ["src/include"],
    deps = [
        "@openfst//:base",
        ":regexp",
        ":utils",
    ],
)

cc_library(
    name = "normalizer",
    srcs = [
        "src/lib/normalizer.cc",
        "src/lib/normalizer_utils.cc",
        "src/lib/numbers.cc",
        "src/lib/protobuf_parser.cc",
        "src/lib/protobuf_serializer.cc",
        "src/lib/rule_system.cc",
    ],
    hdrs = [
        "src/include/sparrowhawk/rule_system.h",
        "src/include/sparrowhawk/normalizer.h",
        "src/include/sparrowhawk/numbers.h",
        "src/include/sparrowhawk/protobuf_parser.h",
        "src/include/sparrowhawk/protobuf_serializer.h",
    ],
    copts = ["-Wno-sign-compare"],
    includes = [
        ".",
        "src/include",
    ],
    deps = [
        "@openfst//:base",
        "@thrax//:grm-manager",
        "//external:re2",
        ":protos",
        ":sentence_boundary",
        ":utils",
    ],
)
