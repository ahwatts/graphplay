# include_directories(AFTER include .)

add_library(gtest src/gtest-all.cc)
target_include_directories(gtest PUBLIC include .)
target_link_libraries(gtest PUBLIC Threads::Threads)

add_library(gtest_main src/gtest_main.cc)
target_include_directories(gtest_main PUBLIC include)
