add_external(
  yaml
  CMAKE
  GIT_REPOSITORY https://github.com/yaml/libyaml.git
  GIT_TAG 2c891fc7a770e8ba2fec34fc6b545c672beb37e6 # v0.2.5
  INCLUDE_DIRS include
  LIBRARIES lib/yaml
)
