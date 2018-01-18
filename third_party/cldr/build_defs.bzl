def transform_test(language):
  native.sh_test(
      name = language + "_transform_test",
      timeout = "short",
      srcs = ["//utils:eval.sh"],
      args = [
          """
          $(location //utils:test_icu_transform) \
            $(location %s-%s_FONIPA.txt) \
            < $(location %s-fonipa-t-%s.txt)
          """ % (language, language, language, language)
    ],
    data = [
        "%s-%s_FONIPA.txt" % (language, language),
        "%s-fonipa-t-%s.txt" % (language, language),
        "//utils:test_icu_transform",
    ],
)
