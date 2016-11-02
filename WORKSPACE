workspace(name = "language_resources")

# Android SDK

android_sdk_repository(
    name = "androidsdk",
    api_level = 24,
    build_tools_version = "24.0.3",
    path = "/usr/local/android-sdk",
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

# Google Test & Protocol Buffers

new_git_repository(
    name = "googletest",
    build_file = "googletest.BUILD",
    commit = "ecd530865cefdfa7dea58e84f6aa1b548950363d",
    remote = "https://github.com/google/googletest.git",
)

git_repository(
    name = "protobuf",
    remote = "https://github.com/google/protobuf.git",
    tag = "v3.0.2",
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
    tag = "2016-08-01",
)

bind(
    name = "re2",
    actual = "@com_googlesource_code_re2//:re2",
)

new_git_repository(
    name = "sparrowhawk",
    build_file = "sparrowhawk.BUILD",
    remote = "https://github.com/google/sparrowhawk.git",
    tag = "0.1",
)
