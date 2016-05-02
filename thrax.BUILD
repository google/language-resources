package(default_visibility = ["//visibility:public"])

openfst = "@openfst//"

prefix_dir = "src/"

# Command-line binaries (bin/)

cc_binary(
    name = "thraxcompiler",
    srcs = [
        prefix_dir + "bin/compiler.cc",
        prefix_dir + "include/thrax/compiler.h",
        prefix_dir + "lib/main/compiler-log.cc",
        prefix_dir + "lib/main/compiler-log64.cc",
        prefix_dir + "lib/main/compiler-stdarc.cc",
    ],
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
    srcs = [
        prefix_dir + "bin/rewrite-tester.cc",
        prefix_dir + "bin/rewrite-tester-utils.cc",
        prefix_dir + "bin/rewrite-tester-utils.h",
    ],
    deps = [":thrax_main_lib"],
)

cc_library(
    name = "thrax_main_lib",
    srcs = [prefix_dir + "bin/utildefs.cc"],
    hdrs = [
        prefix_dir + "bin/utildefs.h",
        prefix_dir + "include/thrax/symbols.h",  # for k.*SymbolTableName
    ],
    visibility = ["//visibility:private"],
    deps = [
        ":compat",
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
    srcs = [
        prefix_dir + "lib/util/utils.cc",
        prefix_dir + "lib/flags/flags.cc",
    ],
    hdrs = [
        prefix_dir + "include/thrax/compat/compat.h",
        prefix_dir + "include/thrax/compat/registry.h",
        prefix_dir + "include/thrax/compat/stlfunctions.h",
        prefix_dir + "include/thrax/compat/utils.h",
        prefix_dir + "include/thrax/make-parens-pair-vector.h",
    ],
    includes = [prefix_dir + "include"],
    visibility = ["//visibility:private"],
    deps = [openfst + ":base"],
)

cc_library(
    name = "grm-manager",
    srcs = [
        prefix_dir + "lib/main/grm-manager.cc",
    ],
    hdrs = [
        prefix_dir + "include/thrax/abstract-grm-manager.h",
        prefix_dir + "include/thrax/grm-manager.h",
    ],
    includes = [prefix_dir + "include"],
    deps = [
        ":compat",
        openfst + ":far_base",
        openfst + ":fst",
        openfst + ":mpdt",
        openfst + ":pdt",
    ],
)

cc_library(
    name = "function",
    srcs = [
        prefix_dir + "lib/walker/loader.cc",
        prefix_dir + "lib/walker/stringfst.cc",
        prefix_dir + "lib/walker/symbols.cc",
    ],
    hdrs = [
        prefix_dir + "include/thrax/algo/cdrewrite.h",
        prefix_dir + "include/thrax/algo/optimize.h",
        prefix_dir + "include/thrax/algo/prefix_tree.h",
        prefix_dir + "include/thrax/compat/oneof.h",
        prefix_dir + "include/thrax/datatype.h",
        prefix_dir + "include/thrax/fst-node.h",  # for FstNodeType
        prefix_dir + "include/thrax/function.h",
        prefix_dir + "include/thrax/node.h",  # included by fst-node.h
        prefix_dir + "include/thrax/symbols.h",

        # thrax::function::Function implementations
        prefix_dir + "include/thrax/arcsort.h",
        prefix_dir + "include/thrax/assert-empty.h",
        prefix_dir + "include/thrax/assert-equal.h",
        prefix_dir + "include/thrax/assert-null.h",
        prefix_dir + "include/thrax/cdrewrite.h",
        prefix_dir + "include/thrax/closure.h",  # includes fst-node.h
        prefix_dir + "include/thrax/compose.h",
        prefix_dir + "include/thrax/concat.h",
        prefix_dir + "include/thrax/connect.h",
        prefix_dir + "include/thrax/determinize.h",
        prefix_dir + "include/thrax/difference.h",
        prefix_dir + "include/thrax/expand.h",
        prefix_dir + "include/thrax/features.h",
        prefix_dir + "include/thrax/invert.h",
        prefix_dir + "include/thrax/loadfst.h",
        prefix_dir + "include/thrax/loadfstfromfar.h",
        prefix_dir + "include/thrax/minimize.h",
        prefix_dir + "include/thrax/mpdtcompose.h",
        prefix_dir + "include/thrax/optimize.h",
        prefix_dir + "include/thrax/paradigm.h",
        prefix_dir + "include/thrax/pdtcompose.h",
        prefix_dir + "include/thrax/project.h",
        prefix_dir + "include/thrax/replace.h",
        prefix_dir + "include/thrax/reverse.h",
        prefix_dir + "include/thrax/rewrite.h",
        prefix_dir + "include/thrax/rmepsilon.h",
        prefix_dir + "include/thrax/stringfile.h",
        prefix_dir + "include/thrax/stringfst.h",
        prefix_dir + "include/thrax/symboltable.h",
        prefix_dir + "include/thrax/union.h",
    ],
    includes = [prefix_dir + "include"],
    visibility = ["//visibility:private"],
    deps = [
        ":compat",
        openfst + ":far_base",
        openfst + ":fst",
        openfst + ":mpdt",
        openfst + ":pdt",
    ],
)

cc_library(
    name = "grm-compiler",
    srcs = [
        # Abstract Syntax Tree classses
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

        # Lexer, parser, and core compiler
        prefix_dir + "lib/main/grm-compiler.cc",
        prefix_dir + "lib/main/lexer.cc",
        prefix_dir + "lib/main/parser.cc",

        # Walker/evaluator
        prefix_dir + "lib/walker/evaluator-specializations.cc",
        prefix_dir + "lib/walker/identifier-counter.cc",
        prefix_dir + "lib/walker/namespace.cc",
        prefix_dir + "lib/walker/printer.cc",
        prefix_dir + "lib/walker/walker.cc",
    ],
    hdrs = [
        # Abstract Syntax Tree classes
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

        # Lexer, parser, and core compiler
        prefix_dir + "include/thrax/grm-compiler.h",
        prefix_dir + "include/thrax/lexer.h",

        # Walker/evaluator
        prefix_dir + "include/thrax/algo/resource-map.h",
        prefix_dir + "include/thrax/compat/closure.h",
        prefix_dir + "include/thrax/evaluator.h",
        prefix_dir + "include/thrax/identifier-counter.h",
        prefix_dir + "include/thrax/namespace.h",
        prefix_dir + "include/thrax/printer.h",
        prefix_dir + "include/thrax/walker.h",
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
        openfst + ":fst",
    ],
)
