package(default_visibility = ["//visibility:public"])

prefix_dir = "src/"
static_binary = 0

# Core library (lib/)

cc_library(
    name = "fst-decl",
    hdrs = [prefix_dir + "include/fst/fst-decl.h"],
    includes = [prefix_dir + "include"],
    deps = [":base"],
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
    copts = ["-Wno-sign-compare"],
    includes = [prefix_dir + "include"],
    linkopts = ["-lm"],
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
    copts = ["-Wno-sign-compare"],
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
    deps = [":base"],
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
    deps = [":base"],
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

# Core library tests (test/)

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
    includes = [prefix_dir + "include"],
    deps = [":weight"],
)

cc_test(
    name = "weight_test",
    srcs = [prefix_dir + "test/weight_test.cc"],
    copts = ["-Wno-unused-local-typedefs"],
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

# Non-template scripting-langugage integration (script/)

cc_library(
    name = "fstscript_base",
    hdrs = [
        prefix_dir + "include/fst/script/arg-packs.h",
        prefix_dir + "include/fst/script/fst-class.h",
        prefix_dir + "include/fst/script/fstscript-decl.h",
        prefix_dir + "include/fst/script/fstscript.h",
        prefix_dir + "include/fst/script/register.h",
        prefix_dir + "include/fst/script/script-impl.h",
        prefix_dir + "include/fst/script/text-io.h",
        prefix_dir + "include/fst/script/weight-class.h",
    ],
    srcs = [
        prefix_dir + "script/fst-class.cc",
        prefix_dir + "script/script-impl.cc",
        prefix_dir + "script/text-io.cc",
        prefix_dir + "script/weight-class.cc",
    ],
    copts = ["-Wno-sign-compare"],
    includes = [prefix_dir + "include"],
    visibility = ["//visibility:private"],
    deps = [":fst"],
)


cc_library(
    name = "fstscript_arcsort",
    hdrs = [prefix_dir + "include/fst/script/arcsort.h"],
    srcs = [prefix_dir + "script/arcsort.cc"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_closure",
    hdrs = [prefix_dir + "include/fst/script/closure.h"],
    srcs = [prefix_dir + "script/closure.cc"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_compile",
    hdrs = [
        prefix_dir + "include/fst/script/compile.h",
        prefix_dir + "include/fst/script/compile-impl.h",
    ],
    srcs = [prefix_dir + "script/compile.cc"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_compose",
    hdrs = [prefix_dir + "include/fst/script/compose.h"],
    srcs = [prefix_dir + "script/compose.cc"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_concat",
    hdrs = [prefix_dir + "include/fst/script/concat.h"],
    srcs = [prefix_dir + "script/concat.cc"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_connect",
    hdrs = [prefix_dir + "include/fst/script/connect.h"],
    srcs = [prefix_dir + "script/connect.cc"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_convert",
    hdrs = [prefix_dir + "include/fst/script/convert.h"],
    srcs = [prefix_dir + "script/convert.cc"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_decode",
    hdrs = [prefix_dir + "include/fst/script/decode.h"],
    srcs = [prefix_dir + "script/decode.cc"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_determinize",
    hdrs = [prefix_dir + "include/fst/script/determinize.h"],
    srcs = [prefix_dir + "script/determinize.cc"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_difference",
    hdrs = [prefix_dir + "include/fst/script/difference.h"],
    srcs = [prefix_dir + "script/difference.cc"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_disambiguate",
    hdrs = [prefix_dir + "include/fst/script/disambiguate.h"],
    srcs = [prefix_dir + "script/disambiguate.cc"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_draw",
    hdrs = [
        prefix_dir + "include/fst/script/draw.h",
        prefix_dir + "include/fst/script/draw-impl.h",
    ],
    srcs = [prefix_dir + "script/draw.cc"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_encode",
    hdrs = [prefix_dir + "include/fst/script/encode.h"],
    srcs = [prefix_dir + "script/encode.cc"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_epsnormalize",
    hdrs = [prefix_dir + "include/fst/script/epsnormalize.h"],
    srcs = [prefix_dir + "script/epsnormalize.cc"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_equal",
    hdrs = [prefix_dir + "include/fst/script/equal.h"],
    srcs = [prefix_dir + "script/equal.cc"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_equivalent",
    hdrs = [prefix_dir + "include/fst/script/equivalent.h"],
    srcs = [prefix_dir + "script/equivalent.cc"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_info",
    hdrs = [
        prefix_dir + "include/fst/script/info.h",
        prefix_dir + "include/fst/script/info-impl.h",
    ],
    srcs = [prefix_dir + "script/info.cc"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_intersect",
    hdrs = [prefix_dir + "include/fst/script/intersect.h"],
    srcs = [prefix_dir + "script/intersect.cc"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_invert",
    hdrs = [prefix_dir + "include/fst/script/invert.h"],
    srcs = [prefix_dir + "script/invert.cc"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_isomorphic",
    hdrs = [prefix_dir + "include/fst/script/isomorphic.h"],
    srcs = [prefix_dir + "script/isomorphic.cc"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_map",
    hdrs = [prefix_dir + "include/fst/script/map.h"],
    srcs = [prefix_dir + "script/map.cc"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_minimize",
    hdrs = [prefix_dir + "include/fst/script/minimize.h"],
    srcs = [prefix_dir + "script/minimize.cc"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_print",
    hdrs = [
        prefix_dir + "include/fst/script/print.h",
        prefix_dir + "include/fst/script/print-impl.h",
    ],
    srcs = [prefix_dir + "script/print.cc"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_project",
    hdrs = [prefix_dir + "include/fst/script/project.h"],
    srcs = [prefix_dir + "script/project.cc"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_prune",
    hdrs = [prefix_dir + "include/fst/script/prune.h"],
    srcs = [prefix_dir + "script/prune.cc"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_push",
    hdrs = [prefix_dir + "include/fst/script/push.h"],
    srcs = [prefix_dir + "script/push.cc"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_randequivalent",
    hdrs = [prefix_dir + "include/fst/script/randequivalent.h"],
    srcs = [prefix_dir + "script/randequivalent.cc"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_randgen",
    hdrs = [prefix_dir + "include/fst/script/randgen.h"],
    srcs = [prefix_dir + "script/randgen.cc"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_relabel",
    hdrs = [prefix_dir + "include/fst/script/relabel.h"],
    srcs = [prefix_dir + "script/relabel.cc"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_replace",
    hdrs = [prefix_dir + "include/fst/script/replace.h"],
    srcs = [prefix_dir + "script/replace.cc"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_reverse",
    hdrs = [prefix_dir + "include/fst/script/reverse.h"],
    srcs = [prefix_dir + "script/reverse.cc"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_reweight",
    hdrs = [prefix_dir + "include/fst/script/reweight.h"],
    srcs = [prefix_dir + "script/reweight.cc"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_rmepsilon",
    hdrs = [prefix_dir + "include/fst/script/rmepsilon.h"],
    srcs = [prefix_dir + "script/rmepsilon.cc"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_shortest_distance",
    hdrs = [prefix_dir + "include/fst/script/shortest-distance.h"],
    srcs = [prefix_dir + "script/shortest-distance.cc"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_shortest_path",
    hdrs = [prefix_dir + "include/fst/script/shortest-path.h"],
    srcs = [prefix_dir + "script/shortest-path.cc"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_synchronize",
    hdrs = [prefix_dir + "include/fst/script/synchronize.h"],
    srcs = [prefix_dir + "script/synchronize.cc"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_topsort",
    hdrs = [prefix_dir + "include/fst/script/topsort.h"],
    srcs = [prefix_dir + "script/topsort.cc"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_union",
    hdrs = [prefix_dir + "include/fst/script/union.h"],
    srcs = [prefix_dir + "script/union.cc"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_verify",
    hdrs = [prefix_dir + "include/fst/script/verify.h"],
    srcs = [prefix_dir + "script/verify.cc"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript",
    deps = [
        ":fstscript_arcsort",
        ":fstscript_closure",
        ":fstscript_compile",
        ":fstscript_compose",
        ":fstscript_concat",
        ":fstscript_connect",
        ":fstscript_convert",
        ":fstscript_decode",
        ":fstscript_determinize",
        ":fstscript_difference",
        ":fstscript_disambiguate",
        ":fstscript_draw",
        ":fstscript_encode",
        ":fstscript_epsnormalize",
        ":fstscript_equal",
        ":fstscript_equivalent",
        ":fstscript_info",
        ":fstscript_intersect",
        ":fstscript_invert",
        ":fstscript_isomorphic",
        ":fstscript_map",
        ":fstscript_minimize",
        ":fstscript_print",
        ":fstscript_project",
        ":fstscript_prune",
        ":fstscript_push",
        ":fstscript_randequivalent",
        ":fstscript_randgen",
        ":fstscript_relabel",
        ":fstscript_replace",
        ":fstscript_reverse",
        ":fstscript_reweight",
        ":fstscript_rmepsilon",
        ":fstscript_shortest_distance",
        ":fstscript_shortest_path",
        ":fstscript_synchronize",
        ":fstscript_topsort",
        ":fstscript_union",
        ":fstscript_verify",
    ],
)

# Command-line binaries (bin/)

cc_binary(
    name = "fstarcsort",
    srcs = [prefix_dir + "bin/fstarcsort.cc"],
    linkstatic = static_binary,
    deps = [":fstscript_arcsort"],
)

cc_binary(
    name = "fstclosure",
    srcs = [prefix_dir + "bin/fstclosure.cc"],
    linkstatic = static_binary,
    deps = [":fstscript_closure"],
)

cc_binary(
    name = "fstcompile",
    srcs = [prefix_dir + "bin/fstcompile.cc"],
    linkstatic = static_binary,
    deps = [":fstscript_compile"],
)

cc_binary(
    name = "fstcompose",
    srcs = [prefix_dir + "bin/fstcompose.cc"],
    linkstatic = static_binary,
    deps = [":fstscript_compose"],
)

cc_binary(
    name = "fstconcat",
    srcs = [prefix_dir + "bin/fstconcat.cc"],
    linkstatic = static_binary,
    deps = [":fstscript_concat"],
)

cc_binary(
    name = "fstconnect",
    srcs = [prefix_dir + "bin/fstconnect.cc"],
    linkstatic = static_binary,
    deps = [":fstscript_connect"],
)

cc_binary(
    name = "fstconvert",
    srcs = [prefix_dir + "bin/fstconvert.cc"],
    linkstatic = static_binary,
    deps = [":fstscript_convert"],
)

cc_binary(
    name = "fstdeterminize",
    srcs = [prefix_dir + "bin/fstdeterminize.cc"],
    linkstatic = static_binary,
    deps = [":fstscript_determinize"],
)

cc_binary(
    name = "fstdifference",
    srcs = [prefix_dir + "bin/fstdifference.cc"],
    linkstatic = static_binary,
    deps = [":fstscript_difference"],
)

cc_binary(
    name = "fstdisambiguate",
    srcs = [prefix_dir + "bin/fstdisambiguate.cc"],
    linkstatic = static_binary,
    deps = [":fstscript_disambiguate"],
)

cc_binary(
    name = "fstdraw",
    srcs = [prefix_dir + "bin/fstdraw.cc"],
    linkstatic = static_binary,
    deps = [":fstscript_draw"],
)

cc_binary(
    name = "fstencode",
    srcs = [prefix_dir + "bin/fstencode.cc"],
    linkstatic = static_binary,
    deps = [
        ":fstscript_decode",
        ":fstscript_encode",
    ],
)

cc_binary(
    name = "fstepsnormalize",
    srcs = [prefix_dir + "bin/fstepsnormalize.cc"],
    linkstatic = static_binary,
    deps = [":fstscript_epsnormalize"],
)

cc_binary(
    name = "fstequal",
    srcs = [prefix_dir + "bin/fstequal.cc"],
    linkstatic = static_binary,
    deps = [":fstscript_equal"],
)

cc_binary(
    name = "fstequivalent",
    srcs = [prefix_dir + "bin/fstequivalent.cc"],
    linkstatic = static_binary,
    deps = [
        ":fstscript_equivalent",
        ":fstscript_randequivalent",
    ],
)

cc_binary(
    name = "fstinfo",
    srcs = [prefix_dir + "bin/fstinfo.cc"],
    linkstatic = static_binary,
    deps = [":fstscript_info"],
)

cc_binary(
    name = "fstintersect",
    srcs = [prefix_dir + "bin/fstintersect.cc"],
    linkstatic = static_binary,
    deps = [":fstscript_intersect"],
)

cc_binary(
    name = "fstinvert",
    srcs = [prefix_dir + "bin/fstinvert.cc"],
    linkstatic = static_binary,
    deps = [":fstscript_invert"],
)

cc_binary(
    name = "fstisomorphic",
    srcs = [prefix_dir + "bin/fstisomorphic.cc"],
    linkstatic = static_binary,
    deps = [":fstscript_isomorphic"],
)

cc_binary(
    name = "fstmap",
    srcs = [prefix_dir + "bin/fstmap.cc"],
    linkstatic = static_binary,
    deps = [":fstscript_map"],
)

cc_binary(
    name = "fstminimize",
    srcs = [prefix_dir + "bin/fstminimize.cc"],
    linkstatic = static_binary,
    deps = [":fstscript_minimize"],
)

cc_binary(
    name = "fstprint",
    srcs = [prefix_dir + "bin/fstprint.cc"],
    linkstatic = static_binary,
    deps = [":fstscript_print"],
)

cc_binary(
    name = "fstproject",
    srcs = [prefix_dir + "bin/fstproject.cc"],
    linkstatic = static_binary,
    deps = [":fstscript_project"],
)

cc_binary(
    name = "fstprune",
    srcs = [prefix_dir + "bin/fstprune.cc"],
    linkstatic = static_binary,
    deps = [":fstscript_prune"],
)

cc_binary(
    name = "fstpush",
    srcs = [prefix_dir + "bin/fstpush.cc"],
    linkstatic = static_binary,
    deps = [":fstscript_push"],
)

cc_binary(
    name = "fstrandgen",
    srcs = [prefix_dir + "bin/fstrandgen.cc"],
    linkstatic = static_binary,
    deps = [":fstscript_randgen"],
)

cc_binary(
    name = "fstrelabel",
    srcs = [prefix_dir + "bin/fstrelabel.cc"],
    linkstatic = static_binary,
    deps = [":fstscript_relabel"],
)

cc_binary(
    name = "fstreplace",
    srcs = [prefix_dir + "bin/fstreplace.cc"],
    copts = ["-Wno-sign-compare"],
    linkstatic = static_binary,
    deps = [":fstscript_replace"],
)

cc_binary(
    name = "fstreverse",
    srcs = [prefix_dir + "bin/fstreverse.cc"],
    linkstatic = static_binary,
    deps = [":fstscript_reverse"],
)

cc_binary(
    name = "fstreweight",
    srcs = [prefix_dir + "bin/fstreweight.cc"],
    linkstatic = static_binary,
    deps = [":fstscript_reweight"],
)

cc_binary(
    name = "fstrmepsilon",
    srcs = [prefix_dir + "bin/fstrmepsilon.cc"],
    linkstatic = static_binary,
    deps = [":fstscript_rmepsilon"],
)

cc_binary(
    name = "fstshortestdistance",
    srcs = [prefix_dir + "bin/fstshortestdistance.cc"],
    linkstatic = static_binary,
    deps = [":fstscript_shortest_distance"],
)

cc_binary(
    name = "fstshortestpath",
    srcs = [prefix_dir + "bin/fstshortestpath.cc"],
    linkstatic = static_binary,
    deps = [":fstscript_shortest_path"],
)

cc_binary(
    name = "fstsymbols",
    srcs = [prefix_dir + "bin/fstsymbols.cc"],
    linkstatic = static_binary,
    deps = [":fstscript_verify"],
)

cc_binary(
    name = "fstsynchronize",
    srcs = [prefix_dir + "bin/fstsynchronize.cc"],
    linkstatic = static_binary,
    deps = [":fstscript_synchronize"],
)

cc_binary(
    name = "fsttopsort",
    srcs = [prefix_dir + "bin/fsttopsort.cc"],
    linkstatic = static_binary,
    deps = [":fstscript_topsort"],
)

cc_binary(
    name = "fstunion",
    srcs = [prefix_dir + "bin/fstunion.cc"],
    linkstatic = static_binary,
    deps = [":fstscript_union"],
)

# Extension: Fst ARchive a/k/a FAR (extensions/far/)

cc_library(
    name = "sttable",
    hdrs = [
        prefix_dir + "include/fst/extensions/far/stlist.h",
        prefix_dir + "include/fst/extensions/far/sttable.h",
    ],
    srcs = [
        prefix_dir + "extensions/far/stlist.cc",
        prefix_dir + "extensions/far/sttable.cc",
    ],
    includes = [prefix_dir + "include"],
    deps = [":util"],
)

cc_library(
    name = "far_base",
    hdrs = [
        prefix_dir + "include/fst/extensions/far/far.h",
    ],
    includes = [prefix_dir + "include"],
    deps = [
        ":fst",
        ":sttable",
    ],
)

cc_library(
    name = "far",
    hdrs = [
        prefix_dir + "include/fst/extensions/far/compile-strings.h",
        prefix_dir + "include/fst/extensions/far/create.h",
        prefix_dir + "include/fst/extensions/far/equal.h",
        prefix_dir + "include/fst/extensions/far/extract.h",
        prefix_dir + "include/fst/extensions/far/farlib.h",
        prefix_dir + "include/fst/extensions/far/info.h",
        prefix_dir + "include/fst/extensions/far/isomorphic.h",
        prefix_dir + "include/fst/extensions/far/print-strings.h",
    ],
    srcs = [prefix_dir + "extensions/far/strings.cc"],
    includes = [prefix_dir + "include"],
    deps = [
        ":far_base",
        ":fst",
    ],
)

cc_library(
    name = "farscript",
    hdrs = [prefix_dir + "include/fst/extensions/far/farscript.h"],
    srcs = [prefix_dir + "extensions/far/farscript.cc"],
    includes = [prefix_dir + "include"],
    deps = [
        ":base",
        ":far",
        ":fstscript_base",
    ],
)

cc_library(
    name = "far_main_lib",
    hdrs = [prefix_dir + "include/fst/extensions/far/main.h"],
    srcs = [prefix_dir + "extensions/far/main.cc"],
    includes = [prefix_dir + "include"],
    visibility = ["//visibility:private"],
    deps = [
        ":far",
        ":farscript",
    ],
)

cc_binary(
    name = "farcompilestrings",
    srcs = [prefix_dir + "extensions/far/farcompilestrings.cc"],
    copts = ["-Wno-sign-compare"],
    linkstatic = static_binary,
    deps = [":far_main_lib"],
)

cc_binary(
    name = "farcreate",
    srcs = [prefix_dir + "extensions/far/farcreate.cc"],
    copts = ["-Wno-sign-compare"],
    linkstatic = static_binary,
    deps = [":far_main_lib"],
)

cc_binary(
    name = "farequal",
    srcs = [prefix_dir + "extensions/far/farequal.cc"],
    linkstatic = static_binary,
    deps = [":far_main_lib"],
)

cc_binary(
    name = "farextract",
    srcs = [prefix_dir + "extensions/far/farextract.cc"],
    linkstatic = static_binary,
    deps = [":far_main_lib"],
)

cc_binary(
    name = "farinfo",
    srcs = [prefix_dir + "extensions/far/farinfo.cc"],
    linkstatic = static_binary,
    deps = [":far_main_lib"],
)

cc_binary(
    name = "farisomorphic",
    srcs = [prefix_dir + "extensions/far/farisomorphic.cc"],
    linkstatic = static_binary,
    deps = [":far_main_lib"],
)

cc_binary(
    name = "farprintstrings",
    srcs = [prefix_dir + "extensions/far/farprintstrings.cc"],
    linkstatic = static_binary,
    deps = [":far_main_lib"],
)

# Extension: PushDown Transducers a/k/a PDT (extensions/pdt/)

cc_library(
    name = "pdt",
    hdrs = [
        prefix_dir + "include/fst/extensions/pdt/collection.h",
        prefix_dir + "include/fst/extensions/pdt/compose.h",
        prefix_dir + "include/fst/extensions/pdt/expand.h",
        prefix_dir + "include/fst/extensions/pdt/info.h",
        prefix_dir + "include/fst/extensions/pdt/paren.h",
        prefix_dir + "include/fst/extensions/pdt/pdt.h",
        prefix_dir + "include/fst/extensions/pdt/pdtlib.h",
        prefix_dir + "include/fst/extensions/pdt/replace.h",
        prefix_dir + "include/fst/extensions/pdt/reverse.h",
        prefix_dir + "include/fst/extensions/pdt/shortest-path.h",
    ],
    includes = [prefix_dir + "include"],
    deps = [
        ":fst",
    ],
)

cc_library(
    name = "pdtscript",
    hdrs = [prefix_dir + "include/fst/extensions/pdt/pdtscript.h"],
    srcs = [prefix_dir + "extensions/pdt/pdtscript.cc"],
    includes = [prefix_dir + "include"],
    deps = [
        ":fst",
        ":fstscript_base",
        ":pdt",
    ],
)

cc_binary(
    name = "pdtcompose",
    srcs = [prefix_dir + "extensions/pdt/pdtcompose.cc"],
    linkstatic = static_binary,
    deps = [
        ":fstscript_connect",
        ":pdtscript",
    ],
)

cc_binary(
    name = "pdtexpand",
    srcs = [prefix_dir + "extensions/pdt/pdtexpand.cc"],
    linkstatic = static_binary,
    deps = [":pdtscript"],
)

cc_binary(
    name = "pdtinfo",
    srcs = [prefix_dir + "extensions/pdt/pdtinfo.cc"],
    linkstatic = static_binary,
    deps = [":pdtscript"],
)

cc_binary(
    name = "pdtreplace",
    srcs = [prefix_dir + "extensions/pdt/pdtreplace.cc"],
    copts = ["-Wno-sign-compare"],
    linkstatic = static_binary,
    deps = [":pdtscript"],
)

cc_binary(
    name = "pdtreverse",
    srcs = [prefix_dir + "extensions/pdt/pdtreverse.cc"],
    linkstatic = static_binary,
    deps = [":pdtscript"],
)

cc_binary(
    name = "pdtshortestpath",
    srcs = [prefix_dir + "extensions/pdt/pdtshortestpath.cc"],
    linkstatic = static_binary,
    deps = [":pdtscript"],
)
