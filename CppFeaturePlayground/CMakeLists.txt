# CMakeList.txt : CMake project for CppFeaturePlayground, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (CppFeaturePlayground "CppFeaturePlayground.cpp" "CppFeaturePlayground.h" "SmartList.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET CppFeaturePlayground PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.

include(FetchContent)
FetchContent_Declare(
  json_lib
  GIT_REPOSITORY https://github.com/nlohmann/json
  GIT_TAG        55f93686c01528224f448c19128836e7df245f72
)
FetchContent_MakeAvailable(json_lib)

target_link_libraries(CppFeaturePlayground PRIVATE nlohmann_json::nlohmann_json)