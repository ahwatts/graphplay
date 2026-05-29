add_library(gtest src/gtest-all.cc)
target_include_directories(gtest
    PUBLIC include
    PRIVATE .)
target_link_libraries(gtest INTERFACE Threads::Threads)
target_compile_definitions(gtest PUBLIC -DGTEST_LANG_CXX11 1)
target_compile_features(gtest PUBLIC cxx_std_11)

add_library(gtest_main src/gtest_main.cc)
target_compile_definitions(gtest_main PUBLIC -DGTEST_LANG_CXX11 1)
target_compile_features(gtest_main PUBLIC cxx_std_11)
target_include_directories(gtest_main PUBLIC include)
