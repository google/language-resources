workspace(name = "language_resources")

# Android SDK

android_sdk_repository(
    name = "androidsdk",
    api_level = 24,
    build_tools_version = "26.0.3",
    path = "/usr/local/android-sdk",
)

# Protobuf

protobuf_version = "3.5.0.1"

protobuf_sha256 = "86be71e61c76575c60839452a4f265449a6ea51570d7983cb929f06ad294b5f5"

# proto_library and related rules implicitly depend on @com_google_protobuf.
http_archive(
    name = "com_google_protobuf",
    sha256 = protobuf_sha256,
    strip_prefix = "protobuf-%s" % protobuf_version,
    urls = ["https://github.com/google/protobuf/archive/v%s.tar.gz" %
            protobuf_version],
)

# DEPREACTED. For backwards compatibility with older versions of Bazel:
http_archive(
    name = "com_google_protobuf_cc",
    sha256 = protobuf_sha256,
    strip_prefix = "protobuf-%s" % protobuf_version,
    urls = ["https://github.com/google/protobuf/archive/v%s.tar.gz" %
            protobuf_version],
)

new_http_archive(
    name = "six_1_10_0",
    build_file = "six.BUILD",
    sha256 = "105f8d68616f8248e24bf0e9372ef04d3cc10104f1980f54d57b2ce73a5ad56a",
    strip_prefix = "six-1.10.0",
    urls = ["https://pypi.python.org/packages/source/s/six/six-1.10.0.tar.gz"],
)

bind(
    name = "six",
    actual = "@six_1_10_0//:six",
)

# Google fundamental libraries

http_archive(
    name = "com_google_googletest",
    strip_prefix = "googletest-master",
    urls = ["https://github.com/google/googletest/archive/master.zip"],
)

# DEPRECATED. Aliases in //external referenced by @com_google_protobuf:
bind(
    name = "gtest",
    actual = "@com_google_googletest//:gtest",
)

bind(
    name = "gtest_main",
    actual = "@com_google_googletest//:gtest_main",
)

new_git_repository(
    name = "farmhash",
    build_file = "farmhash.BUILD",
    commit = "92e897b282426729f4724d91a637596c7e2fe28f",
    remote = "https://github.com/google/farmhash.git",
)

new_git_repository(
    name = "com_googlesource_code_re2",
    build_file = "re2.BUILD",
    remote = "https://github.com/google/re2.git",
    tag = "2016-11-01",
)

bind(
    name = "re2",
    actual = "@com_googlesource_code_re2//:re2",
)

http_archive(
    name = "com_google_absl",
    strip_prefix = "abseil-cpp-master",
    urls = ["https://github.com/abseil/abseil-cpp/archive/master.zip"],
)

# Time-zone framework, required by Abseil
http_archive(
    name = "com_googlesource_code_cctz",
    strip_prefix = "cctz-master",
    urls = ["https://github.com/google/cctz/archive/master.zip"],
)

# OpenFst, OpenGrm NGram & Thrax

new_git_repository(
    name = "openfst",
    build_file = "openfst.BUILD",
    remote = "https://github.com/mjansche/openfst.git",
    tag = "1.5.3",
)

new_git_repository(
    name = "opengrm_ngram",
    build_file = "opengrm-ngram.BUILD",
    remote = "https://github.com/mjansche/opengrm-ngram.git",
    tag = "1.3.0",
)

new_git_repository(
    name = "thrax",
    build_file = "thrax.BUILD",
    commit = "fa51f84e8bbff730c88df293aacc1041a0e4cf1a",
    remote = "https://github.com/mjansche/thrax.git",
)

# Sparrowhawk

new_git_repository(
    name = "sparrowhawk",
    build_file = "sparrowhawk.BUILD",
    commit = "eb97411535e9ab280982bae18c99d6a2c933e162",
    remote = "https://github.com/google/sparrowhawk.git",
)
