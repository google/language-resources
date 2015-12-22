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
