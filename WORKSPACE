workspace(name = "language_resources")

# Android SDK

android_sdk_repository(
    name = "androidsdk",
    # Uses the SDK referenced by the environment variable ANDROID_HOME.
)

# Protobuf

protobuf_version = "3.5.1.1"

protobuf_sha256 = "56b5d9e1ab2bf4f5736c4cfba9f4981fbc6976246721e7ded5602fbaee6d6869"

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
    name = "six_archive",
    build_file = "bazel/six.BUILD",
    sha256 = "70e8a77beed4562e7f14fe23a786b54f6296e34344c23bc42f07b15018ff98e9",
    strip_prefix = "six-1.11.0",
    urls = ["https://pypi.python.org/packages/16/d8/bc6316cf98419719bd59c91742194c111b6f2e85abac88e496adefaf7afe/six-1.11.0.tar.gz#md5=d12789f9baf7e9fb2524c0c64f1773f8"],
)

bind(
    name = "six",
    actual = "@six_archive//:six",
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

new_http_archive(
    name = "farmhash",
    build_file = "bazel/farmhash.BUILD",
    strip_prefix = "farmhash-master",
    urls = ["https://github.com/google/farmhash/archive/master.zip"],
)

new_git_repository(
    name = "com_googlesource_code_re2",
    build_file = "bazel/re2.BUILD",
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

# Time-zone framework, required by abseil-cpp
http_archive(
    name = "com_googlesource_code_cctz",
    strip_prefix = "cctz-master",
    urls = ["https://github.com/google/cctz/archive/master.zip"],
)

http_archive(
    name = "io_abseil_py",
    strip_prefix = "abseil-py-master",
    urls = ["https://github.com/abseil/abseil-py/archive/master.zip"],
)

bind(
    name = "absl/app",
    actual = "@io_abseil_py//absl:app",
)

bind(
    name = "absl/flags",
    actual = "@io_abseil_py//absl/flags",
)

bind(
    name = "absl/logging",
    actual = "@io_abseil_py//absl/logging",
)

# OpenFst, OpenGrm NGram & Thrax

new_git_repository(
    name = "openfst",
    build_file = "bazel/openfst.BUILD",
    remote = "https://github.com/mjansche/openfst.git",
    tag = "1.6.3",
)

new_git_repository(
    name = "opengrm_ngram",
    build_file = "bazel/opengrm-ngram.BUILD",
    remote = "https://github.com/mjansche/opengrm-ngram.git",
    tag = "1.3.2",
)

new_git_repository(
    name = "thrax",
    build_file = "bazel/thrax.BUILD",
    commit = "e5630ac984c186d57668d57074c7c23efdbce8a5",  # 1.2.3
    remote = "https://github.com/mjansche/thrax.git",
)

# Sparrowhawk

new_git_repository(
    name = "sparrowhawk",
    build_file = "bazel/sparrowhawk.BUILD",
    commit = "0d2fe1305439c213653ed15da639de9b72f454a4",
    remote = "https://github.com/google/sparrowhawk.git",
)
