add_library(SmartList INTERFACE "SmartList.h")

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest
  GIT_TAG        52eb8108c5bdec04579160ae17225d66034bd723
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

FetchContent_Declare(
  json_lib
  GIT_REPOSITORY https://github.com/nlohmann/json
  GIT_TAG        55f93686c01528224f448c19128836e7df245f72
)
FetchContent_MakeAvailable(json_lib)

target_link_libraries(SmartList INTERFACE nlohmann_json::nlohmann_json GTest::gtest_main)
target_compile_options(SmartList INTERFACE --coverage)
target_link_options(SmartList INTERFACE --coverage)