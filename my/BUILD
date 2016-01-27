genrule(
    name = "compile_burmese_grm",
    srcs = [
        "burmese.grm",
        "grapheme.syms",
        "phoneme.syms",
    ],
    outs = ["burmese.far"],
    cmd = """
        $(location @thrax//:thraxcompiler) \
          --input_grammar=$(location burmese.grm) \
          --output_far=$@
        """,
    tools = ["@thrax//:thraxcompiler"],
)

genrule(
    name = "g2p_fst",
    srcs = ["burmese.far"],
    outs = ["G2P.fst"],
    cmd = """
        $(location @openfst//:farextract) \
          --filename_prefix=$(@D)/ \
          --keys=G2P \
          --filename_suffix=.fst \
          $<
        """,
    tools = ["@openfst//:farextract"],
)
