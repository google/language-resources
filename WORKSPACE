workspace(name = "language_resources")

# Android SDK

android_sdk_repository(
    name = "androidsdk",
    api_level = 24,
    build_tools_version = "24.0.3",
    path = "/usr/local/android-sdk",
)

# Protobuf

git_repository(
    name = "protobuf",
    remote = "https://github.com/google/protobuf.git",
    tag = "v3.3.2",
)

# proto_library rules depend on @com_google_protobuf//:protoc
http_archive(
    name = "com_google_protobuf",
    sha256 = "8b8e442becbeff637f160c1ef4a3a56769c50ba7c9ff939ccc94086530ff00e4",
    strip_prefix = "protobuf-3.3.2",
    urls = ["https://github.com/google/protobuf/archive/v3.3.2.tar.gz"],
)

# cc_proto_library rules depend on @com_google_protobuf_cc//:cc_toolchain
http_archive(
    name = "com_google_protobuf_cc",
    sha256 = "8b8e442becbeff637f160c1ef4a3a56769c50ba7c9ff939ccc94086530ff00e4",
    strip_prefix = "protobuf-3.3.2",
    urls = ["https://github.com/google/protobuf/archive/v3.3.2.tar.gz"],
)

# Google fundamental libraries

new_git_repository(
    name = "googletest",
    build_file = "googletest.BUILD",
    commit = "3447fc31b4eea1fbcb86fa0e2f5d9ed9f38776bf",
    remote = "https://github.com/google/googletest.git",
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

# World, SPTK, etc.

new_git_repository(
    name = "world",
    build_file = "world.BUILD",
    commit = "30c95a8a8072687b3e01d6c961159ec3490611de",
    remote = "https://github.com/mmorise/World.git",
)

new_http_archive(
    name = "sptk",
    build_file = "sptk.BUILD",
    sha256 = "a9bf59df6eb798eed3c6c08dac0443db25e4675e700235486add45a17d4ed9fe",
    strip_prefix = "SPTK-3.10",
    url = "http://downloads.sourceforge.net/sp-tk/SPTK-3.10.tar.gz",
)
