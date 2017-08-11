package(default_visibility = ["//visibility:public"])

COPTS = [
    "-DFORMAT=\\\"float\\\"",
    "-DHAVE_BZERO=1",
    "-DHAVE_MEMSET=1",
    "-DHAVE_STRING_H=1",
    "-DHAVE_STRRCHR=1",
    "-DPACKAGE_VERSION=\\\"3.10\\\"",
    "-Iexternal/sptk/include",
]

genrule(
    name = "make_cplusplus_header",
    srcs = ["include/SPTK.h"],
    outs = ["sptk.h"],
    cmd = """
        echo 'extern "C" {' > $@
        egrep -v 'wavread|wavwrite' $< >> $@
        echo '}  // extern "C"' >> $@
    """,
)

cc_library(
    name = "sptk",
    srcs = [
        "bin/b2mc/_b2mc.c",
        "bin/c2acr/_c2acr.c",
        "bin/c2sp/_c2sp.c",
        "bin/fft/_fft.c",
        "bin/fftr/_fftr.c",
        "bin/freqt/_freqt.c",
        "bin/gc2gc/_gc2gc.c",
        "bin/gnorm/_gnorm.c",
        "bin/ifft/_ifft.c",
        "bin/ifftr/_ifftr.c",
        "bin/ignorm/_ignorm.c",
        "bin/mc2b/_mc2b.c",
        "bin/mcep/_mcep.c",
        "bin/mgc2mgc/_mgc2mgc.c",
        "bin/mgc2sp/_mgc2sp.c",
        "include/SPTK.h",
        "lib/agexp.c",
        "lib/cholesky.c",
        "lib/fileio.c",
        "lib/fillz.c",
        "lib/getfp.c",
        "lib/getmem.c",
        "lib/gexp.c",
        "lib/glog.c",
        "lib/invert.c",
        "lib/matrix.c",
        "lib/movem.c",
        "lib/mseq.c",
        "lib/theq.c",
        "lib/toeplitz.c",
    ],
    hdrs = [
        "sptk.h",
    ],
    copts = COPTS,
)

cc_binary(
    name = "b2mc",
    srcs = ["bin/b2mc/b2mc.c"],
    copts = COPTS,
    deps = [":sptk"],
)

cc_binary(
    name = "bcp",
    srcs = ["bin/bcp/bcp.c"],
    copts = COPTS,
    deps = [":sptk"],
)

cc_binary(
    name = "c2acr",
    srcs = ["bin/c2acr/c2acr.c"],
    copts = COPTS,
    deps = [":sptk"],
)

cc_binary(
    name = "c2sp",
    srcs = ["bin/c2sp/c2sp.c"],
    copts = COPTS,
    deps = [":sptk"],
)

cc_binary(
    name = "fft",
    srcs = ["bin/fft/fft.c"],
    copts = COPTS,
    deps = [":sptk"],
)

cc_binary(
    name = "fftr",
    srcs = ["bin/fftr/fftr.c"],
    copts = COPTS,
    deps = [":sptk"],
)

cc_binary(
    name = "freqt",
    srcs = ["bin/freqt/freqt.c"],
    copts = COPTS,
    deps = [":sptk"],
)

cc_binary(
    name = "gc2gc",
    srcs = ["bin/gc2gc/gc2gc.c"],
    copts = COPTS,
    deps = [":sptk"],
)

cc_binary(
    name = "gnorm",
    srcs = ["bin/gnorm/gnorm.c"],
    copts = COPTS,
    deps = [":sptk"],
)

cc_binary(
    name = "ifft",
    srcs = ["bin/ifft/ifft.c"],
    copts = COPTS,
    deps = [":sptk"],
)

cc_binary(
    name = "ifftr",
    srcs = ["bin/ifftr/ifftr.c"],
    copts = COPTS,
    deps = [":sptk"],
)

cc_binary(
    name = "ignorm",
    srcs = ["bin/ignorm/ignorm.c"],
    copts = COPTS,
    deps = [":sptk"],
)

cc_binary(
    name = "mc2b",
    srcs = ["bin/mc2b/mc2b.c"],
    copts = COPTS,
    deps = [":sptk"],
)

cc_binary(
    name = "mcep",
    srcs = ["bin/mcep/mcep.c"],
    copts = COPTS,
    deps = [":sptk"],
)

cc_binary(
    name = "merge",
    srcs = ["bin/merge/merge.c"],
    copts = COPTS,
    deps = [":sptk"],
)

cc_binary(
    name = "mgc2mgc",
    srcs = ["bin/mgc2mgc/mgc2mgc.c"],
    copts = COPTS,
    deps = [":sptk"],
)

cc_binary(
    name = "mgc2sp",
    srcs = ["bin/mgc2sp/mgc2sp.c"],
    copts = COPTS,
    deps = [":sptk"],
)

cc_binary(
    name = "sopr",
    srcs = ["bin/sopr/sopr.c"],
    copts = COPTS,
    deps = [":sptk"],
)

cc_binary(
    name = "vopr",
    srcs = ["bin/vopr/vopr.c"],
    copts = COPTS,
    deps = [":sptk"],
)

cc_binary(
    name = "x2x",
    srcs = ["bin/x2x/x2x.c"],
    copts = COPTS,
    deps = [":sptk"],
)
