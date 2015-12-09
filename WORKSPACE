# OpenFst, OpenGrm NGram & Thrax
#
# It is possible to configure the Bazel workspace so that dependencies
# will be downloaded automatically. Unfortunately, the following all fail
# with an HTTP 403 status code:
#
# new_http_archive(
#   name = "openfst",
#   url = "http://openfst.org/twiki/pub/FST/FstDownload/openfst-1.5.0.tar.gz",
#   sha256 = "01c2b810295a942fede5b711bd04bdc9677855c846fedcc999c792604e02177b",
#   build_file = "openfst.BUILD",
# )
#
# new_http_archive(
#   name = "opengrm-ngram",
#   url = "http://openfst.org/twiki/pub/GRM/NGramDownload/opengrm-ngram-1.2.1.tar.gz"
#   sha256 = "713f07dccf225cde29cb048ce955d45d3c2a5ce6be7d923b5a688012d4285453",
#   build_file = "opengrm-ngram.BUILD",
# )
#
# new_http_archive(
#   name = "thrax",
#   url = "http://openfst.org/twiki/pub/GRM/ThraxDownload/thrax-1.1.0.tar.gz",
#   sha256 = "ce99d5b9b67b0d4ef3c9d7003aebad37f31235ef03191a44b11facd8e1b917da",
#   build_file = "thrax.BUILD",
# )
#
# Work around this with GitHub mirrors:

new_git_repository(
    name = "openfst",
    build_file = "openfst.BUILD",
    remote = "https://github.com/mjansche/openfst.git",
    tag = "1.5.0",
)

new_git_repository(
    name = "opengrm-ngram",
    build_file = "opengrm-ngram.BUILD",
    remote = "https://github.com/mjansche/opengrm-ngram.git",
    tag = "1.2.1",
)

new_git_repository(
    name = "thrax",
    build_file = "thrax.BUILD",
    remote = "https://github.com/mjansche/thrax.git",
    tag = "1.1.0",
)

# Google Test & Protocol Buffers

new_git_repository(
    name = "googletest",
    remote = "https://github.com/google/googletest.git",
    commit = "57b376a9cb8b7d0efd958bfaa3204bb33a03ac5e",
    build_file = "googletest.BUILD",
)

bind(
    name = "gtest",
    actual = "@googletest//:gtest",
)

bind(
    name = "gtest_main",
    actual = "@googletest//:gtest_main",
)

git_repository(
    name = "protobuf",
    remote = "https://github.com/google/protobuf.git",
    commit = "27c522940b6ea134d206416db5c9c47b22b05683",
)

bind(
    name = "protobuf_stubs",
    actual = "@protobuf//:protobuf_lite",
)
