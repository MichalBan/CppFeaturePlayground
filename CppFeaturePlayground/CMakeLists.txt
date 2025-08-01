add_subdirectory ("lib")
add_executable (CppFeaturePlayground "src/CppFeaturePlayground.cpp" "src/CppFeaturePlayground.h")
target_link_libraries(CppFeaturePlayground SmartList)
target_include_directories(CppFeaturePlayground PRIVATE "lib")

enable_testing()
add_executable(list_test "test/SmartListTest.cc")
target_link_libraries(list_test  SmartList)
target_include_directories(list_test PRIVATE "lib")
add_test(test_1 list_test)

