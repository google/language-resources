package(default_visibility = ["//visibility:public"])

COPTS = ["-DFORMAT=\\\"float\\\""]

cc_library(
    name = "sptk",
    srcs = [
        "bin/fft/_fft.c",
        "bin/fftr/_fftr.c",
        "bin/freqt/_freqt.c",
        "bin/ifftr/_ifftr.c",
        "bin/mcep/_mcep.c",
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
        "include/SPTK.h",
    ],
    copts = COPTS,
    includes = ["include"],
)

cc_binary(
    name = "mcep",
    srcs = ["bin/mcep/mcep.c"],
    copts = COPTS,
    deps = [":sptk"],
)
