workspace(name = "language_resources")

load(
    "@bazel_tools//tools/build_defs/repo:git.bzl",
    "git_repository",
    "new_git_repository",
)
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# Android SDK

android_sdk_repository(
    name = "androidsdk",
    # Uses the SDK referenced by the environment variable ANDROID_HOME.
)

# Protobuf

protobuf_version = "3.8.0"

http_archive(
    name = "com_google_protobuf",
    strip_prefix = "protobuf-%s" % protobuf_version,
    urls = [
        "https://github.com/protocolbuffers/protobuf/archive/v%s.tar.gz"
	% protobuf_version,
    ],
)

load("@com_google_protobuf//:protobuf_deps.bzl", "protobuf_deps")

protobuf_deps()

http_archive(
    name = "six_archive",
    build_file = "//bazel:six.BUILD",
    sha256 = "70e8a77beed4562e7f14fe23a786b54f6296e34344c23bc42f07b15018ff98e9",
    strip_prefix = "six-1.11.0",
    urls = ["https://pypi.python.org/packages/16/d8/bc6316cf98419719bd59c91742194c111b6f2e85abac88e496adefaf7afe/six-1.11.0.tar.gz#md5=d12789f9baf7e9fb2524c0c64f1773f8"],
)

http_archive(
    name = "bazel_skylib",
    strip_prefix = "bazel-skylib-master",
    urls = ["https://github.com/bazelbuild/bazel-skylib/archive/master.zip"],
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

http_archive(
    name = "farmhash",
    build_file = "//bazel:farmhash.BUILD",
    strip_prefix = "farmhash-master",
    urls = ["https://github.com/google/farmhash/archive/master.zip"],
)

http_archive(
    name = "com_googlesource_code_re2",
    build_file = "//bazel:re2.BUILD",
    strip_prefix = "re2-master",
    urls = ["https://github.com/google/re2/archive/master.zip"],
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

# Google benchmark, required by abseil-cpp.
http_archive(
    name = "com_github_google_benchmark",
    strip_prefix = "benchmark-master",
    urls = ["https://github.com/google/benchmark/archive/master.zip"],
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

git_repository(
    name = "openfst",
    commit = "22867ee7b483598a729abe125da2517584a5d010",  # 1.7.2 plus Bazel
    remote = "https://github.com/mjansche/openfst.git",
)

new_git_repository(
    name = "opengrm_ngram",
    build_file = "//bazel:opengrm-ngram.BUILD",
    remote = "https://github.com/mjansche/opengrm-ngram.git",
    tag = "1.3.4",
)

new_git_repository(
    name = "thrax",
    build_file = "//bazel:thrax.BUILD",
    commit = "c65fb3d51f9bd0299503f3289a124f52c3431eeb",
    remote = "https://github.com/mjansche/thrax.git",
)

# Sparrowhawk

new_git_repository(
    name = "sparrowhawk",
    build_file = "//bazel:sparrowhawk.BUILD",
    commit = "a0503e26a433fbd3a9ff81ba7a08819e4a3bb668",
    remote = "https://github.com/google/sparrowhawk.git",
)
