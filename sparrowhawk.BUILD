package(default_visibility = ["//visibility:public"])

load("@com_google_protobuf//:protobuf.bzl", "cc_proto_library")

cc_proto_library(
    name = "sparrowhawk_configuration_cc_proto",
    srcs = ["src/proto/sparrowhawk_configuration.proto"],
    include = "src/proto",
    default_runtime = "@com_google_protobuf//:protobuf",
    protoc = "@com_google_protobuf//:protoc",
)

cc_proto_library(
    name = "rule_order_cc_proto",
    srcs = ["src/proto/rule_order.proto"],
    include = "src/proto",
    default_runtime = "@com_google_protobuf//:protobuf",
    protoc = "@com_google_protobuf//:protoc",
)

cc_proto_library(
    name = "links_cc_proto",
    srcs = ["src/proto/links.proto"],
    include = "src/proto",
    default_runtime = "@com_google_protobuf//:protobuf",
    protoc = "@com_google_protobuf//:protoc",
)

cc_proto_library(
    name = "semiotic_classes_cc_proto",
    srcs = ["src/proto/semiotic_classes.proto"],
    include = "src/proto",
    default_runtime = "@com_google_protobuf//:protobuf",
    protoc = "@com_google_protobuf//:protoc",
)

cc_proto_library(
    name = "items_cc_proto",
    srcs = ["src/proto/items.proto"],
    include = "src/proto",
    default_runtime = "@com_google_protobuf//:protobuf",
    protoc = "@com_google_protobuf//:protoc",
    deps = [
        ":links_cc_proto",
        ":semiotic_classes_cc_proto",
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
        ":items_cc_proto",
        ":rule_order_cc_proto",
        ":sparrowhawk_configuration_cc_proto",
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
        ":utils",
        "//external:re2",
        "@openfst//:base",
    ],
)

cc_library(
    name = "sentence_boundary",
    srcs = ["src/lib/sentence_boundary.cc"],
    hdrs = ["src/include/sparrowhawk/sentence_boundary.h"],
    copts = ["-Wno-sign-compare"],
    includes = ["src/include"],
    deps = [
        ":regexp",
        ":utils",
        "@openfst//:base",
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
        "src/include/sparrowhawk/normalizer.h",
        "src/include/sparrowhawk/numbers.h",
        "src/include/sparrowhawk/protobuf_parser.h",
        "src/include/sparrowhawk/protobuf_serializer.h",
        "src/include/sparrowhawk/rule_system.h",
    ],
    copts = ["-Wno-sign-compare"],
    includes = [
        ".",
        "src/include",
    ],
    deps = [
        ":protos",
        ":sentence_boundary",
        ":utils",
        "//external:re2",
        "@openfst//:base",
        "@thrax//:grm-manager",
    ],
)
