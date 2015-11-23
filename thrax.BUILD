package(default_visibility = ["//visibility:public"])

openfst = "@openfst//"
prefix_dir = "src/"

# Command-line binaries (bin/)

cc_binary(
    name = "thraxcompiler",
    srcs = [prefix_dir + "bin/compiler.cc"],
    deps = [
        ":grm-compiler",
        ":thrax_main_lib",
    ],
)

cc_binary(
    name = "thraxrandom-generator",
    srcs = [prefix_dir + "bin/random-generator.cc"],
    copts = ["-Wno-sign-compare"],
    deps = [":thrax_main_lib"],
)

cc_binary(
    name = "thraxrewrite-tester",
    srcs = [prefix_dir + "bin/rewrite-tester.cc"],
    deps = [":thrax_main_lib"],
)

cc_library(
    name = "thrax_main_lib",
    hdrs = [prefix_dir + "bin/utildefs.h"],
    srcs = [prefix_dir + "bin/utildefs.cc"],
    visibility = ["//visibility:private"],
    deps = [
        ":compat",
	":function",
        ":grm-manager",
    ],
)

# Library (lib/)

cc_library(
    name = "thrax",
    hdrs = [prefix_dir + "include/thrax/thrax.h"],
    includes = [prefix_dir + "include"],
    deps = [
        ":compat",
        ":function",
        ":grm-compiler",
        ":grm-manager",
    ],
)

cc_library(
    name = "compat",
    hdrs = [
        prefix_dir + "include/thrax/compat/closure.h",
        prefix_dir + "include/thrax/compat/compat.h",
        prefix_dir + "include/thrax/compat/oneof.h",
        prefix_dir + "include/thrax/compat/registry.h",
        prefix_dir + "include/thrax/compat/stlfunctions.h",
        prefix_dir + "include/thrax/compat/utils.h",
        prefix_dir + "include/thrax/make-parens-pair-vector.h",
    ],
    srcs = [
        prefix_dir + "lib/util/utils.cc",
        prefix_dir + "lib/flags/flags.cc",
    ],
    includes = [prefix_dir + "include"],
    visibility = ["//visibility:private"],
    deps = [openfst + ":base"],
)

cc_library(
    name = "grm-manager",
    hdrs = [
        prefix_dir + "include/thrax/abstract-grm-manager.h",
        prefix_dir + "include/thrax/grm-manager.h",
    ],
    srcs = [
        prefix_dir + "lib/main/grm-manager.cc",
    ],
    includes = [prefix_dir + "include"],
    deps = [
        ":compat",
        openfst + ":far_base",
        openfst + ":fst_no_export_dynamic",
        openfst + ":pdt",
    ],
)

cc_library(
    name = "function",
    hdrs = [
        prefix_dir + "include/thrax/algo/context_dependent_rewrite.h",
        prefix_dir + "include/thrax/algo/prefix_tree.h",
        prefix_dir + "include/thrax/arcsort.h",
        prefix_dir + "include/thrax/assert-equal.h",
        prefix_dir + "include/thrax/cdrewrite.h",
        prefix_dir + "include/thrax/closure.h",
        prefix_dir + "include/thrax/compose.h",
        prefix_dir + "include/thrax/concat.h",
        prefix_dir + "include/thrax/connect.h",
        prefix_dir + "include/thrax/datatype.h",
        prefix_dir + "include/thrax/determinize.h",
        prefix_dir + "include/thrax/difference.h",
        prefix_dir + "include/thrax/expand.h",
        prefix_dir + "include/thrax/features.h",
        prefix_dir + "include/thrax/function.h",
        prefix_dir + "include/thrax/invert.h",
        prefix_dir + "include/thrax/loadfst.h",
        prefix_dir + "include/thrax/loadfstfromfar.h",
        prefix_dir + "include/thrax/minimize.h",
        prefix_dir + "include/thrax/optimize.h",
        prefix_dir + "include/thrax/paradigm.h",
        prefix_dir + "include/thrax/pdtcompose.h",
        prefix_dir + "include/thrax/project.h",
        prefix_dir + "include/thrax/reverse.h",
        prefix_dir + "include/thrax/rewrite.h",
        prefix_dir + "include/thrax/rmepsilon.h",
        prefix_dir + "include/thrax/stringfile.h",
        prefix_dir + "include/thrax/stringfst.h",
        prefix_dir + "include/thrax/symbols.h",
        prefix_dir + "include/thrax/symboltable.h",
        prefix_dir + "include/thrax/union.h",
    ],
    srcs = [
        prefix_dir + "lib/walker/loader.cc",
        prefix_dir + "lib/walker/stringfst.cc",
        prefix_dir + "lib/walker/symbols.cc",
    ],
    includes = [prefix_dir + "include"],
    visibility = ["//visibility:private"],
    deps = [
        ":compat",
        openfst + ":fst_no_export_dynamic",
    ],
)        

cc_library(
    name = "grm-compiler",
    hdrs = [
        prefix_dir + "include/thrax/collection-node.h",
        prefix_dir + "include/thrax/fst-node.h",
        prefix_dir + "include/thrax/function-node.h",
        prefix_dir + "include/thrax/grammar-node.h",
        prefix_dir + "include/thrax/identifier-node.h",
        prefix_dir + "include/thrax/import-node.h",
        prefix_dir + "include/thrax/node.h",
        prefix_dir + "include/thrax/return-node.h",
        prefix_dir + "include/thrax/rule-node.h",
        prefix_dir + "include/thrax/statement-node.h",
        prefix_dir + "include/thrax/string-node.h",

        prefix_dir + "include/thrax/grm-compiler.h",
        prefix_dir + "include/thrax/lexer.h",

        prefix_dir + "include/thrax/algo/resource-map.h",
        prefix_dir + "include/thrax/evaluator.h",
        prefix_dir + "include/thrax/namespace.h",
        prefix_dir + "include/thrax/printer.h",
        prefix_dir + "include/thrax/walker.h",
    ],
    srcs = [
        prefix_dir + "lib/ast/collection-node.cc",
        prefix_dir + "lib/ast/fst-node.cc",
        prefix_dir + "lib/ast/function-node.cc",
        prefix_dir + "lib/ast/grammar-node.cc",
        prefix_dir + "lib/ast/identifier-node.cc",
        prefix_dir + "lib/ast/import-node.cc",
        prefix_dir + "lib/ast/node.cc",
        prefix_dir + "lib/ast/return-node.cc",
        prefix_dir + "lib/ast/rule-node.cc",
        prefix_dir + "lib/ast/statement-node.cc",
        prefix_dir + "lib/ast/string-node.cc",

        prefix_dir + "lib/main/grm-compiler.cc",
        prefix_dir + "lib/main/lexer.cc",
        prefix_dir + "lib/parser.cc",

        prefix_dir + "lib/walker/evaluator-specializations.cc",
        prefix_dir + "lib/walker/identifier-counter.cc",
        prefix_dir + "lib/walker/namespace.cc",
        prefix_dir + "lib/walker/printer.cc",
        prefix_dir + "lib/walker/walker.cc",
    ],
    copts = [
        "-Wno-return-type",
        "-Wno-sign-compare",
    ],
    includes = [prefix_dir + "include"],
    deps = [
        ":compat",
        ":function",
	":grm-manager",
        openfst + ":far_base",
        openfst + ":fst_no_export_dynamic",
        openfst + ":pdt",
    ],
)
