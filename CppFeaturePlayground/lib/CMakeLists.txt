add_library(SmartList INTERFACE "SmartList.h")

include(FetchContent)
FetchContent_Declare(
  json_lib
  GIT_REPOSITORY https://github.com/nlohmann/json
  GIT_TAG        55f93686c01528224f448c19128836e7df245f72
)
FetchContent_MakeAvailable(json_lib)
target_link_libraries(SmartList INTERFACE nlohmann_json::nlohmann_json)
target_compile_options(SmartList INTERFACE --coverage)
target_link_options(SmartList INTERFACE --coverage)