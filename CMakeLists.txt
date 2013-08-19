cmake_minimum_required(VERSION 2.8.10)
project(graphplay)

message(STATUS "Configuring for build type: " ${CMAKE_BUILD_TYPE})

set(CMAKE_PREFIX_PATH ../graphplay-deps)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

if(WIN32 AND MINGW)
  add_definitions(-DGLEW_STATIC)
endif(WIN32 AND MINGW)

if(CMAKE_COMPILER_IS_GNUCXX)
  if(CMAKE_CXX_COMPILER_VERSION STRLESS "4.8")
    set(CMAKE_CXX_FLAGS "-std=gnu++11 -Wall")
  elseif(CMAKE_CXX_COMPILER_VERSION STRLESS "4.9")
    set(CMAKE_CXX_FLAGS "-std=c++11 -Wall")
  else()
    set(CMAKE_CXX_FLAGS "-Wall")
  endif(CMAKE_CXX_COMPILER_VERSION STRLESS "4.8")
  set(CMAKE_CXX_FLAGS_DEBUG "-ggdb -pg")
endif(CMAKE_COMPILER_IS_GNUCXX)

find_path(GLFW_INCLUDE_PATH GL/glfw3.h)
find_library(GLFW_LIBRARY NAMES glfw3 glfw)
set(GLFW_INCLUDE_DIRS ${GLFW_INCLUDE_DIR})
set(GLFW_LIBRARIES ${GLFW_LIBRARY})

find_path(GLM_INCLUDE_PATH glm/glm.hpp)
set(GLM_INCLUDE_DIRS ${GLM_INCLUDE_PATH})

find_path(GTEST_INCLUDE_PATH gtest/gtest.h)
find_library(GTEST_LIBRARY NAMES gtest)
find_library(GTEST_MAIN_LIBRARY gtest_main)
set(GTEST_INCLUDE_DIRS ${GTEST_INCLUDE_PATH})
set(GTEST_LIBRARIES ${GTEST_LIBRARY})
set(GTEST_MAIN_LIBRARIES ${GTEST_MAIN_LIBRARY})

include_directories(AFTER SYSTEM ${GLEW_INCLUDE_DIRS} ${GLFW_INCLUDE_DIRS} ${GLM_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS})

add_subdirectory(graphplay)
add_subdirectory(graphplay-test)

enable_testing()
add_test(NAME graphplay-test
         WORKING_DIRECTORY .
         COMMAND graphplay-test)
