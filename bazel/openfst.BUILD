# Copyright 2015-2018 Google LLC. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

package(default_visibility = ["//visibility:public"])

licenses(["notice"])  # Apache 2.0

exports_files(["COPYING"])

prefix_dir = "src/"

static_binary = 1

# Core library (lib/)

cc_library(
    name = "fst-decl",
    hdrs = [prefix_dir + "include/fst/fst-decl.h"],
    includes = [prefix_dir + "include"],
    deps = [":base"],
)

PUBLIC_HEADERS = [
    prefix_dir + "include/fst/fstlib.h",
    #
    prefix_dir + "include/fst/accumulator.h",
    prefix_dir + "include/fst/add-on.h",
    prefix_dir + "include/fst/arc-arena.h",
    prefix_dir + "include/fst/arc-map.h",
    prefix_dir + "include/fst/arc.h",
    prefix_dir + "include/fst/arcfilter.h",
    prefix_dir + "include/fst/arcsort.h",
    prefix_dir + "include/fst/bi-table.h",
    prefix_dir + "include/fst/cache.h",
    prefix_dir + "include/fst/closure.h",
    prefix_dir + "include/fst/compact-fst.h",
    prefix_dir + "include/fst/complement.h",
    prefix_dir + "include/fst/compose-filter.h",
    prefix_dir + "include/fst/compose.h",
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
    prefix_dir + "include/fst/heap.h",
    prefix_dir + "include/fst/intersect.h",
    prefix_dir + "include/fst/invert.h",
    prefix_dir + "include/fst/isomorphic.h",
    prefix_dir + "include/fst/label-reachable.h",
    prefix_dir + "include/fst/lookahead-filter.h",
    prefix_dir + "include/fst/lookahead-matcher.h",
    prefix_dir + "include/fst/map.h",
    prefix_dir + "include/fst/matcher-fst.h",
    prefix_dir + "include/fst/matcher.h",
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
    prefix_dir + "include/fst/replace-util.h",
    prefix_dir + "include/fst/replace.h",
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
]

# This version does not have the export-dynamic flag set and should not be
# used to load dynamic-shared object FST extensions. Please see the
# "lib_export_dynamic" target below for binaries that need DSO loading.
cc_library(
    name = "lib_lite",
    srcs = [
        prefix_dir + "lib/fst.cc",
        prefix_dir + "lib/properties.cc",
        prefix_dir + "lib/symbol-table-ops.cc",
    ],
    hdrs = PUBLIC_HEADERS,
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
    name = "fst",
    hdrs = PUBLIC_HEADERS,
    includes = [prefix_dir + "include"],
    deps = [
        ":fst-types",
        ":lib_lite",
    ],
)

cc_library(
    name = "lib_export_dynamic",
    linkopts = ["-Wl,--export-dynamic"],
    deps = [":fst"],
)

cc_library(
    name = "fst-types",
    srcs = [prefix_dir + "lib/fst-types.cc"],
    deps = [":lib_lite"],
    alwayslink = 1,  # because of registration
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
    srcs = [prefix_dir + "lib/weight.cc"],
    hdrs = [
        prefix_dir + "include/fst/expectation-weight.h",
        prefix_dir + "include/fst/float-weight.h",
        prefix_dir + "include/fst/lexicographic-weight.h",
        prefix_dir + "include/fst/pair-weight.h",
        prefix_dir + "include/fst/power-weight.h",
        prefix_dir + "include/fst/product-weight.h",
        prefix_dir + "include/fst/set-weight.h",
        prefix_dir + "include/fst/signed-log-weight.h",
        prefix_dir + "include/fst/sparse-power-weight.h",
        prefix_dir + "include/fst/sparse-tuple-weight.h",
        prefix_dir + "include/fst/string-weight.h",
        prefix_dir + "include/fst/tuple-weight.h",
        prefix_dir + "include/fst/union-weight.h",
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
    timeout = "short",
    srcs = [
        prefix_dir + "test/fst_test.cc",
        prefix_dir + "include/fst/test/fst_test.h",
    ],
    copts = ["-Wno-sign-compare"],
    deps = [":fst"],
)

cc_library(
    name = "weight-tester",
    testonly = 1,
    hdrs = [prefix_dir + "include/fst/test/weight-tester.h"],
    includes = [prefix_dir],
    deps = [":weight"],
)

cc_test(
    name = "weight_test",
    timeout = "short",
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
        prefix_dir + "include/fst/test/algo_test.h",
        prefix_dir + "include/fst/test/rand-fst.h",
    ],
    copts = ["-Wno-unused-local-typedefs"],
    deps = [":fst"],
)

# Non-template scripting-language integration (script/)

cc_library(
    name = "fstscript_base",
    srcs = [
        prefix_dir + "script/arciterator-class.cc",
        prefix_dir + "script/encodemapper-class.cc",
        prefix_dir + "script/fst-class.cc",
        prefix_dir + "script/getters.cc",
        prefix_dir + "script/stateiterator-class.cc",
        prefix_dir + "script/text-io.cc",
        prefix_dir + "script/weight-class.cc",
    ],
    hdrs = [
        prefix_dir + "include/fst/script/arc-class.h",
        prefix_dir + "include/fst/script/arciterator-class.h",
        prefix_dir + "include/fst/script/arg-packs.h",
        prefix_dir + "include/fst/script/encodemapper-class.h",
        prefix_dir + "include/fst/script/fst-class.h",
        prefix_dir + "include/fst/script/fstscript-decl.h",
        prefix_dir + "include/fst/script/fstscript.h",
        prefix_dir + "include/fst/script/getters.h",
        prefix_dir + "include/fst/script/register.h",
        prefix_dir + "include/fst/script/script-impl.h",
        prefix_dir + "include/fst/script/stateiterator-class.h",
        prefix_dir + "include/fst/script/text-io.h",
        prefix_dir + "include/fst/script/weight-class.h",
        #
        prefix_dir + "include/fst/script/arcsort.h",
        prefix_dir + "include/fst/script/closure.h",
        prefix_dir + "include/fst/script/compile.h",
        prefix_dir + "include/fst/script/compile-impl.h",
        prefix_dir + "include/fst/script/compose.h",
        prefix_dir + "include/fst/script/concat.h",
        prefix_dir + "include/fst/script/connect.h",
        prefix_dir + "include/fst/script/convert.h",
        prefix_dir + "include/fst/script/decode.h",
        prefix_dir + "include/fst/script/determinize.h",
        prefix_dir + "include/fst/script/difference.h",
        prefix_dir + "include/fst/script/disambiguate.h",
        prefix_dir + "include/fst/script/draw.h",
        prefix_dir + "include/fst/script/draw-impl.h",
        prefix_dir + "include/fst/script/encode.h",
        prefix_dir + "include/fst/script/epsnormalize.h",
        prefix_dir + "include/fst/script/equal.h",
        prefix_dir + "include/fst/script/equivalent.h",
        prefix_dir + "include/fst/script/info.h",
        prefix_dir + "include/fst/script/info-impl.h",
        prefix_dir + "include/fst/script/intersect.h",
        prefix_dir + "include/fst/script/invert.h",
        prefix_dir + "include/fst/script/isomorphic.h",
        prefix_dir + "include/fst/script/map.h",
        prefix_dir + "include/fst/script/minimize.h",
        prefix_dir + "include/fst/script/print.h",
        prefix_dir + "include/fst/script/print-impl.h",
        prefix_dir + "include/fst/script/project.h",
        prefix_dir + "include/fst/script/prune.h",
        prefix_dir + "include/fst/script/push.h",
        prefix_dir + "include/fst/script/randequivalent.h",
        prefix_dir + "include/fst/script/randgen.h",
        prefix_dir + "include/fst/script/relabel.h",
        prefix_dir + "include/fst/script/replace.h",
        prefix_dir + "include/fst/script/reverse.h",
        prefix_dir + "include/fst/script/reweight.h",
        prefix_dir + "include/fst/script/rmepsilon.h",
        prefix_dir + "include/fst/script/shortest-distance.h",
        prefix_dir + "include/fst/script/shortest-path.h",
        prefix_dir + "include/fst/script/synchronize.h",
        prefix_dir + "include/fst/script/topsort.h",
        prefix_dir + "include/fst/script/union.h",
        prefix_dir + "include/fst/script/verify.h",
    ],
    copts = ["-Wno-sign-compare"],
    includes = [prefix_dir + "include"],
    deps = [":fst"],
)

cc_library(
    name = "fstscript_arcsort",
    srcs = [prefix_dir + "script/arcsort.cc"],
    hdrs = [prefix_dir + "include/fst/script/arcsort.h"],
    includes = [prefix_dir + "include"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_closure",
    srcs = [prefix_dir + "script/closure.cc"],
    hdrs = [prefix_dir + "include/fst/script/closure.h"],
    includes = [prefix_dir + "include"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_compile",
    srcs = [prefix_dir + "script/compile.cc"],
    hdrs = [
        prefix_dir + "include/fst/script/compile.h",
        prefix_dir + "include/fst/script/compile-impl.h",
    ],
    includes = [prefix_dir + "include"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_compose",
    srcs = [prefix_dir + "script/compose.cc"],
    hdrs = [prefix_dir + "include/fst/script/compose.h"],
    includes = [prefix_dir + "include"],
    deps = [
        ":fstscript_base",
        ":fstscript_connect",
    ],
)

cc_library(
    name = "fstscript_concat",
    srcs = [prefix_dir + "script/concat.cc"],
    hdrs = [prefix_dir + "include/fst/script/concat.h"],
    includes = [prefix_dir + "include"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_connect",
    srcs = [prefix_dir + "script/connect.cc"],
    hdrs = [prefix_dir + "include/fst/script/connect.h"],
    includes = [prefix_dir + "include"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_convert",
    srcs = [prefix_dir + "script/convert.cc"],
    hdrs = [prefix_dir + "include/fst/script/convert.h"],
    includes = [prefix_dir + "include"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_decode",
    srcs = [prefix_dir + "script/decode.cc"],
    hdrs = [prefix_dir + "include/fst/script/decode.h"],
    includes = [prefix_dir + "include"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_determinize",
    srcs = [prefix_dir + "script/determinize.cc"],
    hdrs = [prefix_dir + "include/fst/script/determinize.h"],
    includes = [prefix_dir + "include"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_difference",
    srcs = [prefix_dir + "script/difference.cc"],
    hdrs = [prefix_dir + "include/fst/script/difference.h"],
    includes = [prefix_dir + "include"],
    deps = [
        ":fstscript_base",
        ":fstscript_compose",
    ],
)

cc_library(
    name = "fstscript_disambiguate",
    srcs = [prefix_dir + "script/disambiguate.cc"],
    hdrs = [prefix_dir + "include/fst/script/disambiguate.h"],
    includes = [prefix_dir + "include"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_draw",
    srcs = [prefix_dir + "script/draw.cc"],
    hdrs = [
        prefix_dir + "include/fst/script/draw.h",
        prefix_dir + "include/fst/script/draw-impl.h",
    ],
    includes = [prefix_dir + "include"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_encode",
    srcs = [prefix_dir + "script/encode.cc"],
    hdrs = [prefix_dir + "include/fst/script/encode.h"],
    includes = [prefix_dir + "include"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_epsnormalize",
    srcs = [prefix_dir + "script/epsnormalize.cc"],
    hdrs = [prefix_dir + "include/fst/script/epsnormalize.h"],
    includes = [prefix_dir + "include"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_equal",
    srcs = [prefix_dir + "script/equal.cc"],
    hdrs = [prefix_dir + "include/fst/script/equal.h"],
    includes = [prefix_dir + "include"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_equivalent",
    srcs = [prefix_dir + "script/equivalent.cc"],
    hdrs = [prefix_dir + "include/fst/script/equivalent.h"],
    includes = [prefix_dir + "include"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_info",
    srcs = [
        prefix_dir + "script/info.cc",
        prefix_dir + "script/info-impl.cc",
    ],
    hdrs = [
        prefix_dir + "include/fst/script/info.h",
        prefix_dir + "include/fst/script/info-impl.h",
    ],
    includes = [prefix_dir + "include"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_intersect",
    srcs = [prefix_dir + "script/intersect.cc"],
    hdrs = [prefix_dir + "include/fst/script/intersect.h"],
    includes = [prefix_dir + "include"],
    deps = [
        ":fstscript_base",
        ":fstscript_compose",
    ],
)

cc_library(
    name = "fstscript_invert",
    srcs = [prefix_dir + "script/invert.cc"],
    hdrs = [prefix_dir + "include/fst/script/invert.h"],
    includes = [prefix_dir + "include"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_isomorphic",
    srcs = [prefix_dir + "script/isomorphic.cc"],
    hdrs = [prefix_dir + "include/fst/script/isomorphic.h"],
    includes = [prefix_dir + "include"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_map",
    srcs = [prefix_dir + "script/map.cc"],
    hdrs = [prefix_dir + "include/fst/script/map.h"],
    includes = [prefix_dir + "include"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_minimize",
    srcs = [prefix_dir + "script/minimize.cc"],
    hdrs = [prefix_dir + "include/fst/script/minimize.h"],
    includes = [prefix_dir + "include"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_print",
    srcs = [prefix_dir + "script/print.cc"],
    hdrs = [
        prefix_dir + "include/fst/script/print.h",
        prefix_dir + "include/fst/script/print-impl.h",
    ],
    includes = [prefix_dir + "include"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_project",
    srcs = [prefix_dir + "script/project.cc"],
    hdrs = [prefix_dir + "include/fst/script/project.h"],
    includes = [prefix_dir + "include"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_prune",
    srcs = [prefix_dir + "script/prune.cc"],
    hdrs = [prefix_dir + "include/fst/script/prune.h"],
    includes = [prefix_dir + "include"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_push",
    srcs = [prefix_dir + "script/push.cc"],
    hdrs = [prefix_dir + "include/fst/script/push.h"],
    includes = [prefix_dir + "include"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_randequivalent",
    srcs = [prefix_dir + "script/randequivalent.cc"],
    hdrs = [prefix_dir + "include/fst/script/randequivalent.h"],
    includes = [prefix_dir + "include"],
    deps = [
        ":fstscript_base",
        ":fstscript_randgen",
    ],
)

cc_library(
    name = "fstscript_randgen",
    srcs = [prefix_dir + "script/randgen.cc"],
    hdrs = [prefix_dir + "include/fst/script/randgen.h"],
    includes = [prefix_dir + "include"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_relabel",
    srcs = [prefix_dir + "script/relabel.cc"],
    hdrs = [prefix_dir + "include/fst/script/relabel.h"],
    includes = [prefix_dir + "include"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_replace",
    srcs = [prefix_dir + "script/replace.cc"],
    hdrs = [prefix_dir + "include/fst/script/replace.h"],
    copts = ["-Wno-sign-compare"],
    includes = [prefix_dir + "include"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_reverse",
    srcs = [prefix_dir + "script/reverse.cc"],
    hdrs = [prefix_dir + "include/fst/script/reverse.h"],
    includes = [prefix_dir + "include"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_reweight",
    srcs = [prefix_dir + "script/reweight.cc"],
    hdrs = [prefix_dir + "include/fst/script/reweight.h"],
    includes = [prefix_dir + "include"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_rmepsilon",
    srcs = [prefix_dir + "script/rmepsilon.cc"],
    hdrs = [prefix_dir + "include/fst/script/rmepsilon.h"],
    includes = [prefix_dir + "include"],
    deps = [
        ":fstscript_base",
        ":fstscript_shortest_distance",
    ],
)

cc_library(
    name = "fstscript_shortest_distance",
    srcs = [prefix_dir + "script/shortest-distance.cc"],
    hdrs = [prefix_dir + "include/fst/script/shortest-distance.h"],
    includes = [prefix_dir + "include"],
    deps = [
        ":fstscript_base",
        ":fstscript_prune",
    ],
)

cc_library(
    name = "fstscript_shortest_path",
    srcs = [prefix_dir + "script/shortest-path.cc"],
    hdrs = [prefix_dir + "include/fst/script/shortest-path.h"],
    includes = [prefix_dir + "include"],
    deps = [
        ":fstscript_base",
        ":fstscript_shortest_distance",
    ],
)

cc_library(
    name = "fstscript_synchronize",
    srcs = [prefix_dir + "script/synchronize.cc"],
    hdrs = [prefix_dir + "include/fst/script/synchronize.h"],
    includes = [prefix_dir + "include"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_topsort",
    srcs = [prefix_dir + "script/topsort.cc"],
    hdrs = [prefix_dir + "include/fst/script/topsort.h"],
    includes = [prefix_dir + "include"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_union",
    srcs = [prefix_dir + "script/union.cc"],
    hdrs = [prefix_dir + "include/fst/script/union.h"],
    includes = [prefix_dir + "include"],
    deps = [":fstscript_base"],
)

cc_library(
    name = "fstscript_verify",
    srcs = [prefix_dir + "script/verify.cc"],
    hdrs = [prefix_dir + "include/fst/script/verify.h"],
    includes = [prefix_dir + "include"],
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
    srcs = [
        prefix_dir + "bin/fstarcsort.cc",
        prefix_dir + "bin/fstarcsort-main.cc",
    ],
    linkstatic = static_binary,
    deps = [":fstscript_arcsort"],
)

cc_binary(
    name = "fstclosure",
    srcs = [
        prefix_dir + "bin/fstclosure.cc",
        prefix_dir + "bin/fstclosure-main.cc",
    ],
    linkstatic = static_binary,
    deps = [":fstscript_closure"],
)

cc_binary(
    name = "fstcompile",
    srcs = [
        prefix_dir + "bin/fstcompile.cc",
        prefix_dir + "bin/fstcompile-main.cc",
    ],
    linkstatic = static_binary,
    deps = [":fstscript_compile"],
)

cc_binary(
    name = "fstcompose",
    srcs = [
        prefix_dir + "bin/fstcompose.cc",
        prefix_dir + "bin/fstcompose-main.cc",
    ],
    linkstatic = static_binary,
    deps = [":fstscript_compose"],
)

cc_binary(
    name = "fstconcat",
    srcs = [
        prefix_dir + "bin/fstconcat.cc",
        prefix_dir + "bin/fstconcat-main.cc",
    ],
    linkstatic = static_binary,
    deps = [":fstscript_concat"],
)

cc_binary(
    name = "fstconnect",
    srcs = [
        prefix_dir + "bin/fstconnect.cc",
        prefix_dir + "bin/fstconnect-main.cc",
    ],
    linkstatic = static_binary,
    deps = [":fstscript_connect"],
)

cc_binary(
    name = "fstconvert",
    srcs = [
        prefix_dir + "bin/fstconvert.cc",
        prefix_dir + "bin/fstconvert-main.cc",
    ],
    linkstatic = static_binary,
    deps = [":fstscript_convert"],
)

cc_binary(
    name = "fstdeterminize",
    srcs = [
        prefix_dir + "bin/fstdeterminize.cc",
        prefix_dir + "bin/fstdeterminize-main.cc",
    ],
    linkstatic = static_binary,
    deps = [":fstscript_determinize"],
)

cc_binary(
    name = "fstdifference",
    srcs = [
        prefix_dir + "bin/fstdifference.cc",
        prefix_dir + "bin/fstdifference-main.cc",
    ],
    linkstatic = static_binary,
    deps = [":fstscript_difference"],
)

cc_binary(
    name = "fstdisambiguate",
    srcs = [
        prefix_dir + "bin/fstdisambiguate.cc",
        prefix_dir + "bin/fstdisambiguate-main.cc",
    ],
    linkstatic = static_binary,
    deps = [":fstscript_disambiguate"],
)

cc_binary(
    name = "fstdraw",
    srcs = [
        prefix_dir + "bin/fstdraw.cc",
        prefix_dir + "bin/fstdraw-main.cc",
    ],
    linkstatic = static_binary,
    deps = [":fstscript_draw"],
)

cc_binary(
    name = "fstencode",
    srcs = [
        prefix_dir + "bin/fstencode.cc",
        prefix_dir + "bin/fstencode-main.cc",
    ],
    linkstatic = static_binary,
    deps = [
        ":fstscript_decode",
        ":fstscript_encode",
    ],
)

cc_binary(
    name = "fstepsnormalize",
    srcs = [
        prefix_dir + "bin/fstepsnormalize.cc",
        prefix_dir + "bin/fstepsnormalize-main.cc",
    ],
    linkstatic = static_binary,
    deps = [":fstscript_epsnormalize"],
)

cc_binary(
    name = "fstequal",
    srcs = [
        prefix_dir + "bin/fstequal.cc",
        prefix_dir + "bin/fstequal-main.cc",
    ],
    linkstatic = static_binary,
    deps = [":fstscript_equal"],
)

cc_binary(
    name = "fstequivalent",
    srcs = [
        prefix_dir + "bin/fstequivalent.cc",
        prefix_dir + "bin/fstequivalent-main.cc",
    ],
    linkstatic = static_binary,
    deps = [
        ":fstscript_equivalent",
        ":fstscript_randequivalent",
    ],
)

cc_binary(
    name = "fstinfo",
    srcs = [
        prefix_dir + "bin/fstinfo.cc",
        prefix_dir + "bin/fstinfo-main.cc",
    ],
    linkstatic = static_binary,
    deps = [":fstscript_info"],
)

cc_binary(
    name = "fstintersect",
    srcs = [
        prefix_dir + "bin/fstintersect.cc",
        prefix_dir + "bin/fstintersect-main.cc",
    ],
    linkstatic = static_binary,
    deps = [":fstscript_intersect"],
)

cc_binary(
    name = "fstinvert",
    srcs = [
        prefix_dir + "bin/fstinvert.cc",
        prefix_dir + "bin/fstinvert-main.cc",
    ],
    linkstatic = static_binary,
    deps = [":fstscript_invert"],
)

cc_binary(
    name = "fstisomorphic",
    srcs = [
        prefix_dir + "bin/fstisomorphic.cc",
        prefix_dir + "bin/fstisomorphic-main.cc",
    ],
    linkstatic = static_binary,
    deps = [":fstscript_isomorphic"],
)

cc_binary(
    name = "fstmap",
    srcs = [
        prefix_dir + "bin/fstmap.cc",
        prefix_dir + "bin/fstmap-main.cc",
    ],
    linkstatic = static_binary,
    deps = [":fstscript_map"],
)

cc_binary(
    name = "fstminimize",
    srcs = [
        prefix_dir + "bin/fstminimize.cc",
        prefix_dir + "bin/fstminimize-main.cc",
    ],
    linkstatic = static_binary,
    deps = [":fstscript_minimize"],
)

cc_binary(
    name = "fstprint",
    srcs = [
        prefix_dir + "bin/fstprint.cc",
        prefix_dir + "bin/fstprint-main.cc",
    ],
    linkstatic = static_binary,
    deps = [":fstscript_print"],
)

cc_binary(
    name = "fstproject",
    srcs = [
        prefix_dir + "bin/fstproject.cc",
        prefix_dir + "bin/fstproject-main.cc",
    ],
    linkstatic = static_binary,
    deps = [":fstscript_project"],
)

cc_binary(
    name = "fstprune",
    srcs = [
        prefix_dir + "bin/fstprune.cc",
        prefix_dir + "bin/fstprune-main.cc",
    ],
    linkstatic = static_binary,
    deps = [":fstscript_prune"],
)

cc_binary(
    name = "fstpush",
    srcs = [
        prefix_dir + "bin/fstpush.cc",
        prefix_dir + "bin/fstpush-main.cc",
    ],
    linkstatic = static_binary,
    deps = [":fstscript_push"],
)

cc_binary(
    name = "fstrandgen",
    srcs = [
        prefix_dir + "bin/fstrandgen.cc",
        prefix_dir + "bin/fstrandgen-main.cc",
    ],
    linkstatic = static_binary,
    deps = [":fstscript_randgen"],
)

cc_binary(
    name = "fstrelabel",
    srcs = [
        prefix_dir + "bin/fstrelabel.cc",
        prefix_dir + "bin/fstrelabel-main.cc",
    ],
    linkstatic = static_binary,
    deps = [":fstscript_relabel"],
)

cc_binary(
    name = "fstreplace",
    srcs = [
        prefix_dir + "bin/fstreplace.cc",
        prefix_dir + "bin/fstreplace-main.cc",
    ],
    copts = ["-Wno-sign-compare"],
    linkstatic = static_binary,
    deps = [":fstscript_replace"],
)

cc_binary(
    name = "fstreverse",
    srcs = [
        prefix_dir + "bin/fstreverse.cc",
        prefix_dir + "bin/fstreverse-main.cc",
    ],
    linkstatic = static_binary,
    deps = [":fstscript_reverse"],
)

cc_binary(
    name = "fstreweight",
    srcs = [
        prefix_dir + "bin/fstreweight.cc",
        prefix_dir + "bin/fstreweight-main.cc",
    ],
    linkstatic = static_binary,
    deps = [":fstscript_reweight"],
)

cc_binary(
    name = "fstrmepsilon",
    srcs = [
        prefix_dir + "bin/fstrmepsilon.cc",
        prefix_dir + "bin/fstrmepsilon-main.cc",
    ],
    linkstatic = static_binary,
    deps = [":fstscript_rmepsilon"],
)

cc_binary(
    name = "fstshortestdistance",
    srcs = [
        prefix_dir + "bin/fstshortestdistance.cc",
        prefix_dir + "bin/fstshortestdistance-main.cc",
    ],
    linkstatic = static_binary,
    deps = [":fstscript_shortest_distance"],
)

cc_binary(
    name = "fstshortestpath",
    srcs = [
        prefix_dir + "bin/fstshortestpath.cc",
        prefix_dir + "bin/fstshortestpath-main.cc",
    ],
    linkstatic = static_binary,
    deps = [":fstscript_shortest_path"],
)

cc_binary(
    name = "fstsymbols",
    srcs = [
        prefix_dir + "bin/fstsymbols.cc",
        prefix_dir + "bin/fstsymbols-main.cc",
    ],
    linkstatic = static_binary,
    deps = [":fstscript_verify"],
)

cc_binary(
    name = "fstsynchronize",
    srcs = [
        prefix_dir + "bin/fstsynchronize.cc",
        prefix_dir + "bin/fstsynchronize-main.cc",
    ],
    linkstatic = static_binary,
    deps = [":fstscript_synchronize"],
)

cc_binary(
    name = "fsttopsort",
    srcs = [
        prefix_dir + "bin/fsttopsort.cc",
        prefix_dir + "bin/fsttopsort-main.cc",
    ],
    linkstatic = static_binary,
    deps = [":fstscript_topsort"],
)

cc_binary(
    name = "fstunion",
    srcs = [
        prefix_dir + "bin/fstunion.cc",
        prefix_dir + "bin/fstunion-main.cc",
    ],
    linkstatic = static_binary,
    deps = [":fstscript_union"],
)

# Extension: Fst ARchive a/k/a FAR (extensions/far/)

cc_library(
    name = "sttable",
    srcs = [
        prefix_dir + "extensions/far/stlist.cc",
        prefix_dir + "extensions/far/sttable.cc",
    ],
    hdrs = [
        prefix_dir + "include/fst/extensions/far/stlist.h",
        prefix_dir + "include/fst/extensions/far/sttable.h",
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
    srcs = [prefix_dir + "extensions/far/strings.cc"],
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
    includes = [prefix_dir + "include"],
    deps = [
        ":far_base",
        ":fst",
    ],
)

cc_library(
    name = "farscript",
    srcs = [
        prefix_dir + "extensions/far/far-class.cc",
        prefix_dir + "extensions/far/farscript.cc",
        prefix_dir + "extensions/far/getters.cc",
        prefix_dir + "extensions/far/script-impl.cc",
        prefix_dir + "extensions/far/strings.cc",
    ],
    hdrs = [
        prefix_dir + "include/fst/extensions/far/compile-strings.h",
        prefix_dir + "include/fst/extensions/far/far-class.h",
        prefix_dir + "include/fst/extensions/far/farscript.h",
        prefix_dir + "include/fst/extensions/far/getters.h",
        prefix_dir + "include/fst/extensions/far/script-impl.h",
    ],
    includes = [prefix_dir + "include"],
    deps = [
        ":base",
        ":far",
        ":fstscript_base",
    ],
)

cc_binary(
    name = "farcompilestrings",
    srcs = [prefix_dir + "extensions/far/farcompilestrings.cc"],
    copts = ["-Wno-sign-compare"],
    linkstatic = static_binary,
    deps = [":farscript"],
)

cc_binary(
    name = "farcreate",
    srcs = [prefix_dir + "extensions/far/farcreate.cc"],
    copts = ["-Wno-sign-compare"],
    linkstatic = static_binary,
    deps = [":farscript"],
)

cc_binary(
    name = "farequal",
    srcs = [prefix_dir + "extensions/far/farequal.cc"],
    linkstatic = static_binary,
    deps = [":farscript"],
)

cc_binary(
    name = "farextract",
    srcs = [prefix_dir + "extensions/far/farextract.cc"],
    linkstatic = static_binary,
    deps = [":farscript"],
)

cc_binary(
    name = "farinfo",
    srcs = [prefix_dir + "extensions/far/farinfo.cc"],
    linkstatic = static_binary,
    deps = [":farscript"],
)

cc_binary(
    name = "farisomorphic",
    srcs = [prefix_dir + "extensions/far/farisomorphic.cc"],
    linkstatic = static_binary,
    deps = [":farscript"],
)

cc_binary(
    name = "farprintstrings",
    srcs = [prefix_dir + "extensions/far/farprintstrings.cc"],
    linkstatic = static_binary,
    deps = [":farscript"],
)

# Extension: PushDown Transducers a/k/a PDT (extensions/pdt/)

cc_library(
    name = "pdt",
    hdrs = [
        prefix_dir + "include/fst/extensions/pdt/collection.h",
        prefix_dir + "include/fst/extensions/pdt/compose.h",
        prefix_dir + "include/fst/extensions/pdt/expand.h",
        prefix_dir + "include/fst/extensions/pdt/getters.h",
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
    srcs = [
        prefix_dir + "extensions/pdt/getters.cc",
        prefix_dir + "extensions/pdt/pdtscript.cc",
    ],
    hdrs = [
        prefix_dir + "include/fst/extensions/pdt/getters.h",
        prefix_dir + "include/fst/extensions/pdt/pdtscript.h",
    ],
    copts = ["-Wno-sign-compare"],
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

# Extension: Multi PushDown Transducers a/k/a MPDT (extensions/mpdt/)

cc_library(
    name = "mpdt",
    hdrs = [
        prefix_dir + "include/fst/extensions/mpdt/compose.h",
        prefix_dir + "include/fst/extensions/mpdt/expand.h",
        prefix_dir + "include/fst/extensions/mpdt/info.h",
        prefix_dir + "include/fst/extensions/mpdt/mpdt.h",
        prefix_dir + "include/fst/extensions/mpdt/mpdtlib.h",
        prefix_dir + "include/fst/extensions/mpdt/read_write_utils.h",
        prefix_dir + "include/fst/extensions/mpdt/reverse.h",
    ],
    includes = [prefix_dir + "include"],
    deps = [
        ":fst",
        ":pdt",
    ],
)

cc_library(
    name = "mpdtscript",
    srcs = [prefix_dir + "extensions/mpdt/mpdtscript.cc"],
    hdrs = [prefix_dir + "include/fst/extensions/mpdt/mpdtscript.h"],
    includes = [prefix_dir + "include"],
    deps = [
        ":fst",
        ":fstscript_base",
        ":mpdt",
        ":pdtscript",
    ],
)

cc_binary(
    name = "mpdtcompose",
    srcs = [prefix_dir + "extensions/mpdt/mpdtcompose.cc"],
    linkstatic = static_binary,
    deps = [
        ":fstscript_connect",
        ":mpdtscript",
    ],
)

cc_binary(
    name = "mpdtexpand",
    srcs = [prefix_dir + "extensions/mpdt/mpdtexpand.cc"],
    linkstatic = static_binary,
    deps = [":mpdtscript"],
)

cc_binary(
    name = "mpdtinfo",
    srcs = [prefix_dir + "extensions/mpdt/mpdtinfo.cc"],
    linkstatic = static_binary,
    deps = [":mpdtscript"],
)

cc_binary(
    name = "mpdtreverse",
    srcs = [prefix_dir + "extensions/mpdt/mpdtreverse.cc"],
    linkstatic = static_binary,
    deps = [":mpdtscript"],
)

# Extension: LOUDS compressed n-gram language models (extensions/ngram/)

cc_library(
    name = "ngram",
    srcs = [
        prefix_dir + "extensions/ngram/bitmap-index.cc",
        prefix_dir + "extensions/ngram/ngram-fst.cc",
        prefix_dir + "extensions/ngram/nthbit.cc",
    ],
    hdrs = [
        prefix_dir + "include/fst/extensions/ngram/bitmap-index.h",
        prefix_dir + "include/fst/extensions/ngram/ngram-fst.h",
        prefix_dir + "include/fst/extensions/ngram/nthbit.h",
    ],
    includes = [prefix_dir + "include"],
    deps = [
        ":fst",
    ],
)
