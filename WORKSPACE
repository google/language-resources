# OpenFst 1.5.0
#
# It is possible to configure the Bazel workspace so that dependencies
# will be downloaded automatically. Unfortunately, the following fails
# with an HTTP 403 status code:
#
# new_http_archive(
#   name = "openfst",
#   url = "http://openfst.org/twiki/pub/FST/FstDownload/openfst-1.5.0.tar.gz",
#   sha256 = "01c2b810295a942fede5b711bd04bdc9677855c846fedcc999c792604e02177b",
#   build_file = "openfst.BUILD",
# )
#
# Work around this by downloading and unpacking the OpenFst distribution
# manually. The local path below may have to be adjusted to reflect the
# download location.
#
new_local_repository(
  name = "openfst",
  path = "/usr/local/src/openfst-1.5.0",
  build_file = "openfst.BUILD",
)
