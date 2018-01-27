# Natural language resources for speech and language processing.

package(default_visibility = ["//visibility:public"])

licenses(["notice"])  # Apache 2.0

exports_files(["LICENSE"])

sh_binary(
    name = "setup",
    srcs = ["setup.sh"],
    data = [
        "LICENSE",
        "setup.cfg",
        "setup.py",
        "//fonbund:README.md",
        "//fonbund:__init__.py",
        "//fonbund:config/ipa_symbols.textproto",
        "//fonbund:config/segment_repository_config_panphon.textproto",
        "//fonbund:config/segment_repository_config_phoible.textproto",
        "//fonbund:config/segment_repository_config_phoible_fonetikode.textproto",
        "//fonbund:distinctive_features_pb2.py",
        "//fonbund:features_for_segments.py",
        "//fonbund:helpers.py",
        "//fonbund:ipa_symbols_pb2.py",
        "//fonbund:ipa_symbols_repository.py",
        "//fonbund:segment_normalizer.py",
        "//fonbund:segment_normalizer_config_pb2.py",
        "//fonbund:segment_repository_config_pb2.py",
        "//fonbund:segment_repository_reader.py",
        "//fonbund:segment_to_features_converter.py",
        "//fonbund:segments.py",
        "//fonbund:show_segments.py",
    ],
)
