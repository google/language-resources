package(default_visibility = ["//visibility:public"])

prefix_dir = "src/"

cc_library(
    name = "fst-decl",
    hdrs = [prefix_dir + "include/fst/fst-decl.h"],
    includes = [prefix_dir + "include"],
    deps = [
        ":base",
    ],
)

cc_library(
    name = "fst",
    linkopts = ["-Wl,--export-dynamic"],
    deps = [":fst_no_export_dynamic"],
)

# This version does not have export-dynamic flag set and should not be used
# to load dynamic-shared object FST extensions. Please see "fst" rule above
# for binaries that need DSO loading.
cc_library(
    name = "fst_no_export_dynamic",
    srcs = [
        prefix_dir + "lib/fst.cc",
        prefix_dir + "lib/properties.cc",
        prefix_dir + "lib/symbol-table-ops.cc",
    ],
    hdrs = [
        prefix_dir + "include/fst/accumulator.h",
        prefix_dir + "include/fst/add-on.h",
        prefix_dir + "include/fst/arc.h",
        prefix_dir + "include/fst/arc-map.h",
        prefix_dir + "include/fst/arcfilter.h",
        prefix_dir + "include/fst/arcsort.h",
        prefix_dir + "include/fst/bi-table.h",
        prefix_dir + "include/fst/cache.h",
        prefix_dir + "include/fst/closure.h",
        prefix_dir + "include/fst/compact-fst.h",
        prefix_dir + "include/fst/complement.h",
        prefix_dir + "include/fst/compose.h",
        prefix_dir + "include/fst/compose-filter.h",
        prefix_dir + "include/fst/concat.h",
        prefix_dir + "include/fst/connect.h",
        prefix_dir + "include/fst/const-fst.h",
        prefix_dir + "include/fst/determinize.h",
        prefix_dir + "include/fst/dfs-visit.h",
        prefix_dir + "include/fst/difference.h",
        prefix_dir + "include/fst/disambiguate.h",
        prefix_dir + "include/fst/edit-fst.h",
        prefix_dir + "include/fst/encode.h",
        prefix_dir + "include/fst/epsnormalize.h",
        prefix_dir + "include/fst/equal.h",
        prefix_dir + "include/fst/equivalent.h",
        prefix_dir + "include/fst/expanded-fst.h",
        prefix_dir + "include/fst/factor-weight.h",
        prefix_dir + "include/fst/filter-state.h",
        prefix_dir + "include/fst/fst.h",
        prefix_dir + "include/fst/fstlib.h",
        prefix_dir + "include/fst/heap.h",
        prefix_dir + "include/fst/intersect.h",
        prefix_dir + "include/fst/invert.h",
        prefix_dir + "include/fst/label-reachable.h",
        prefix_dir + "include/fst/lookahead-filter.h",
        prefix_dir + "include/fst/lookahead-matcher.h",
        prefix_dir + "include/fst/map.h",
        prefix_dir + "include/fst/matcher.h",
        prefix_dir + "include/fst/matcher-fst.h",
        prefix_dir + "include/fst/memory.h",
        prefix_dir + "include/fst/minimize.h",
        prefix_dir + "include/fst/mutable-fst.h",
        prefix_dir + "include/fst/partition.h",
        prefix_dir + "include/fst/project.h",
        prefix_dir + "include/fst/properties.h",
        prefix_dir + "include/fst/prune.h",
        prefix_dir + "include/fst/push.h",
        prefix_dir + "include/fst/queue.h",
        prefix_dir + "include/fst/randequivalent.h",
        prefix_dir + "include/fst/randgen.h",
        prefix_dir + "include/fst/rational.h",
        prefix_dir + "include/fst/relabel.h",
        prefix_dir + "include/fst/replace.h",
        prefix_dir + "include/fst/replace-util.h",
        prefix_dir + "include/fst/reverse.h",
        prefix_dir + "include/fst/reweight.h",
        prefix_dir + "include/fst/rmepsilon.h",
        prefix_dir + "include/fst/rmfinalepsilon.h",
        prefix_dir + "include/fst/shortest-distance.h",
        prefix_dir + "include/fst/shortest-path.h",
        prefix_dir + "include/fst/state-map.h",
        prefix_dir + "include/fst/state-reachable.h",
        prefix_dir + "include/fst/state-table.h",
        prefix_dir + "include/fst/statesort.h",
        prefix_dir + "include/fst/string.h",
        prefix_dir + "include/fst/symbol-table-ops.h",
        prefix_dir + "include/fst/synchronize.h",
        prefix_dir + "include/fst/test-properties.h",
        prefix_dir + "include/fst/topsort.h",
        prefix_dir + "include/fst/union.h",
        prefix_dir + "include/fst/vector-fst.h",
        prefix_dir + "include/fst/verify.h",
        prefix_dir + "include/fst/visit.h",
    ],
    includes = [prefix_dir + "include"],
    linkopts = ["-lm"],
    visibility = ["//visibility:private"],
    deps = [
        ":base",
        ":fst-decl",
        ":icu",
        ":interval-set",
        ":register",
        ":symbol-table",
        ":union-find",
        ":util",
        ":weight",
    ],
)

cc_library(
    name = "symbol-table",
    srcs = [prefix_dir + "lib/symbol-table.cc"],
    hdrs = [prefix_dir + "include/fst/symbol-table.h"],
    includes = [prefix_dir + "include"],
    deps = [
        ":base",
        ":util",
    ],
)

cc_library(
    name = "weight",
    hdrs = [
        prefix_dir + "include/fst/expectation-weight.h",
        prefix_dir + "include/fst/float-weight.h",
        prefix_dir + "include/fst/lexicographic-weight.h",
        prefix_dir + "include/fst/pair-weight.h",
        prefix_dir + "include/fst/power-weight.h",
        prefix_dir + "include/fst/product-weight.h",
        prefix_dir + "include/fst/random-weight.h",
        prefix_dir + "include/fst/signed-log-weight.h",
        prefix_dir + "include/fst/sparse-power-weight.h",
        prefix_dir + "include/fst/sparse-tuple-weight.h",
        prefix_dir + "include/fst/string-weight.h",
        prefix_dir + "include/fst/tuple-weight.h",
        prefix_dir + "include/fst/weight.h",
    ],
    includes = [prefix_dir + "include"],
    linkopts = ["-lm"],
    deps = [
        ":base",
        ":util",
    ],
)

cc_library(
    name = "interval-set",
    hdrs = [prefix_dir + "include/fst/interval-set.h"],
    includes = [prefix_dir + "include"],
    deps = [
        ":base",
        ":util",
    ],
)

cc_library(
    name = "register",
    hdrs = [
        prefix_dir + "include/fst/generic-register.h",
        prefix_dir + "include/fst/register.h",
    ],
    includes = [prefix_dir + "include"],
    linkopts = ["-ldl"],
    deps = [
        ":base",
        ":util",
    ],
)

cc_library(
    name = "icu",
    hdrs = [
        prefix_dir + "include/fst/icu.h",
    ],
)

cc_library(
    name = "union-find",
    hdrs = [prefix_dir + "include/fst/union-find.h"],
    includes = [prefix_dir + "include"],
    deps = [
        ":base",
    ],
)

cc_library(
    name = "util",
    srcs = [
        prefix_dir + "lib/mapped-file.cc",
        prefix_dir + "lib/util.cc",
    ],
    hdrs = [
        prefix_dir + "include/fst/mapped-file.h",
        prefix_dir + "include/fst/util.h",
    ],
    includes = [prefix_dir + "include"],
    deps = [
        ":base",
    ],
)

cc_library(
    name = "base",
    srcs = [
        prefix_dir + "lib/compat.cc",
        prefix_dir + "lib/flags.cc",
    ],
    hdrs = [
        prefix_dir + "include/fst/compat.h",
        prefix_dir + "include/fst/config.h",
        prefix_dir + "include/fst/flags.h",
        prefix_dir + "include/fst/icu.h",
        prefix_dir + "include/fst/lock.h",
        prefix_dir + "include/fst/log.h",
        prefix_dir + "include/fst/types.h",
    ],
    includes = [prefix_dir + "include"],
)

cc_test(
    name = "fst_test",
    srcs = [
        prefix_dir + "test/fst_test.cc",
        prefix_dir + "test/fst_test.h",
    ],
    deps = [":fst"],
)

cc_library(
    name = "weight-tester",
    testonly = 1,
    hdrs = [prefix_dir + "test/weight-tester.h"],
    deps = [":weight"],
)

cc_test(
    name = "weight_test",
    srcs = [prefix_dir + "test/weight_test.cc"],
    deps = [
        ":fst",
        ":weight-tester",
    ],
)

cc_test(
    name = "algo_test",
    srcs = [
        prefix_dir + "test/algo_test.cc",
        prefix_dir + "test/algo_test.h",
    ],
    deps = [":fst"],
)
