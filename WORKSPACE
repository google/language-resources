workspace(name = "language_resources")

# Android SDK

android_sdk_repository(
    name = "androidsdk",
    api_level = 24,
    build_tools_version = "24.0.2",
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
    remote = "https://github.com/mjansche/thrax.git",
    tag = "1.2.2",
)

# Google Test & Protocol Buffers

new_git_repository(
    name = "googletest",
    build_file = "googletest.BUILD",
    commit = "ec44c6c1675c25b9827aacd08c02433cccde7780",
    remote = "https://github.com/google/googletest.git",
)

git_repository(
    name = "protobuf",
    remote = "https://github.com/google/protobuf.git",
    tag = "v3.0.0",
)

new_git_repository(
    name = "farmhash",
    build_file = "farmhash.BUILD",
    commit = "34c13ddfab0e35422f4c3979f360635a8c050260",
    remote = "https://github.com/google/farmhash.git",
)

new_git_repository(
    name = "re2",
    build_file = "re2.BUILD",
    remote = "https://github.com/google/re2.git",
    tag = "2016-08-01",
)

new_git_repository(
    name = "sparrowhawk",
    build_file = "sparrowhawk.BUILD",
    remote = "https://github.com/google/sparrowhawk.git",
    tag = "0.1",
)
