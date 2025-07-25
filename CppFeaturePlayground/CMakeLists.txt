add_subdirectory ("lib")
add_executable (CppFeaturePlayground "src/CppFeaturePlayground.cpp" "src/CppFeaturePlayground.h")
target_link_libraries(CppFeaturePlayground SmartList)
target_include_directories(CppFeaturePlayground PRIVATE "lib")

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest
  GIT_TAG        52eb8108c5bdec04579160ae17225d66034bd723
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
add_executable(list_test "test/SmartListTest.cc")
target_link_libraries(list_test GTest::gtest_main SmartList)
target_include_directories(list_test PRIVATE "lib")
add_test(test_1 list_test)

