package(default_visibility = ["//visibility:public"])

openfst = "@openfst//"

prefix_dir = "src/"

cc_library(
    name = "opengrm-ngram",
    srcs = [
        prefix_dir + "lib/hist-arc.cc",
        prefix_dir + "lib/ngram-absolute.cc",
        prefix_dir + "lib/ngram-context.cc",
        prefix_dir + "lib/ngram-count-prune.cc",
        prefix_dir + "lib/ngram-kneser-ney.cc",
        prefix_dir + "lib/ngram-marginalize.cc",
        prefix_dir + "lib/ngram-output.cc",
        prefix_dir + "lib/util.cc",
    ],
    hdrs = [
        prefix_dir + "include/ngram/hist-arc.h",
        prefix_dir + "include/ngram/hist-mapper.h",
        prefix_dir + "include/ngram/lexicographic-map.h",
        prefix_dir + "include/ngram/ngram-absolute.h",
        prefix_dir + "include/ngram/ngram-bayes-model-merge.h",
        prefix_dir + "include/ngram/ngram-complete.h",
        prefix_dir + "include/ngram/ngram-context.h",
        prefix_dir + "include/ngram/ngram-context-merge.h",
        prefix_dir + "include/ngram/ngram-context-prune.h",
        prefix_dir + "include/ngram/ngram-count.h",
        prefix_dir + "include/ngram/ngram-count-merge.h",
        prefix_dir + "include/ngram/ngram-count-of-counts.h",
        prefix_dir + "include/ngram/ngram-count-prune.h",
        prefix_dir + "include/ngram/ngram.h",
        prefix_dir + "include/ngram/ngram-hist-merge.h",
        prefix_dir + "include/ngram/ngram-input.h",
        prefix_dir + "include/ngram/ngram-katz.h",
        prefix_dir + "include/ngram/ngram-kneser-ney.h",
        prefix_dir + "include/ngram/ngram-make.h",
        prefix_dir + "include/ngram/ngram-marginalize.h",
        prefix_dir + "include/ngram/ngram-merge.h",
        prefix_dir + "include/ngram/ngram-model.h",
        prefix_dir + "include/ngram/ngram-model-merge.h",
        prefix_dir + "include/ngram/ngram-mutable-model.h",
        prefix_dir + "include/ngram/ngram-output.h",
        prefix_dir + "include/ngram/ngram-randgen.h",
        prefix_dir + "include/ngram/ngram-relentropy.h",
        prefix_dir + "include/ngram/ngram-seymore-shrink.h",
        prefix_dir + "include/ngram/ngram-shrink.h",
        prefix_dir + "include/ngram/ngram-split.h",
        prefix_dir + "include/ngram/ngram-transfer.h",
        prefix_dir + "include/ngram/ngram-unsmoothed.h",
        prefix_dir + "include/ngram/ngram-witten-bell.h",
        prefix_dir + "include/ngram/util.h",
    ],
    copts = [
        "-Wno-maybe-uninitialized",
        "-Wno-sign-compare",
        "-Wno-unused-but-set-variable",
    ],
    includes = [prefix_dir + "include"],
    deps = [
        openfst + ":fst",
        openfst + ":fstscript_base",
    ],
)

cc_binary(
    name = "ngramapply",
    srcs = [prefix_dir + "bin/ngramapply_main.cc"],
    copts = ["-Wno-maybe-uninitialized"],
    deps = [
        ":opengrm-ngram",
        openfst + ":far_base",
    ],
)

cc_binary(
    name = "ngramcount",
    srcs = [prefix_dir + "bin/ngramcount_main.cc"],
    copts = ["-Wno-sign-compare"],
    deps = [
        ":opengrm-ngram",
        openfst + ":far_base",
    ],
)

cc_binary(
    name = "ngraminfo",
    srcs = [prefix_dir + "bin/ngraminfo_main.cc"],
    copts = ["-Wno-sign-compare"],
    deps = [":opengrm-ngram"],
)

cc_binary(
    name = "ngrammake",
    srcs = [prefix_dir + "bin/ngrammake_main.cc"],
    deps = [":opengrm-ngram"],
)

cc_binary(
    name = "ngrammarginalize",
    srcs = [prefix_dir + "bin/ngrammarginalize_main.cc"],
    deps = [":opengrm-ngram"],
)

cc_binary(
    name = "ngrammerge",
    srcs = [prefix_dir + "bin/ngrammerge_main.cc"],
    copts = ["-Wno-sign-compare"],
    deps = [":opengrm-ngram"],
)

cc_binary(
    name = "ngramperplexity",
    srcs = [prefix_dir + "bin/ngramperplexity_main.cc"],
    deps = [
        ":opengrm-ngram",
        openfst + ":far_base",
    ],
)

cc_binary(
    name = "ngramprint",
    srcs = [prefix_dir + "bin/ngramprint_main.cc"],
    deps = [":opengrm-ngram"],
)

cc_binary(
    name = "ngramrandgen",
    srcs = [prefix_dir + "bin/ngramrandgen_main.cc"],
    copts = ["-Wno-sign-compare"],
    deps = [
        ":opengrm-ngram",
        openfst + ":far_base",
    ],
)

cc_binary(
    name = "ngramread",
    srcs = [prefix_dir + "bin/ngramread_main.cc"],
    deps = [":opengrm-ngram"],
)

cc_binary(
    name = "ngramshrink",
    srcs = [prefix_dir + "bin/ngramshrink_main.cc"],
    deps = [":opengrm-ngram"],
)

cc_binary(
    name = "ngramsort",
    srcs = [prefix_dir + "bin/ngramsort_main.cc"],
    deps = [":opengrm-ngram"],
)

cc_binary(
    name = "ngramsplit",
    srcs = [prefix_dir + "bin/ngramsplit_main.cc"],
    deps = [":opengrm-ngram"],
)

cc_binary(
    name = "ngramsymbols",
    srcs = [prefix_dir + "bin/ngramsymbols_main.cc"],
    deps = [":opengrm-ngram"],
)

cc_binary(
    name = "ngramtransfer",
    srcs = [prefix_dir + "bin/ngramtransfer_main.cc"],
    copts = ["-Wno-sign-compare"],
    deps = [":opengrm-ngram"],
)
