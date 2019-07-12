add_library(gtest src/gtest-all.cc)
target_include_directories(gtest
    PUBLIC include
    PRIVATE .)
target_link_libraries(gtest INTERFACE Threads::Threads)

add_library(gtest_main src/gtest_main.cc)
target_include_directories(gtest_main PUBLIC include)
