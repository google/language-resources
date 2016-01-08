def transform_test(language):
  native.sh_test(
      name = language + "_transform_test",
      timeout = "short",
      srcs = ["//tools:eval.sh"],
      args = [
          """
          $(location //tools:test_icu_transform) \
            $(location %s-%s_FONIPA.txt) \
            $(location %s-fonipa-t-%s.txt)
          """ % (language, language, language, language)
    ],
    data = [
        "%s-%s_FONIPA.txt" % (language, language),
        "%s-fonipa-t-%s.txt" % (language, language),
        "//tools:test_icu_transform",
    ],
)
