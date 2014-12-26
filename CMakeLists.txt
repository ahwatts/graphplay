cmake_minimum_required(VERSION 2.8.10)
project(graphplay)

message(STATUS "Configuring for build type: " ${CMAKE_BUILD_TYPE})

set(CMAKE_PREFIX_PATH ../graphplay-deps)

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
  set(CMAKE_EXE_LINKER_FLAGS "-Wl,--as-needed")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "-std=c++11 -Wall")
endif(CMAKE_COMPILER_IS_GNUCXX)

find_package(OpenGL REQUIRED)
# find_package(GLEW REQUIRED)

find_path(GLFW_INCLUDE_DIRS NAMES GL/glfw3.h GLFW/glfw3.h)
find_file(GLFW_FULL_HEADER_PATH NAMES GL/glfw3.h GLFW/glfw3.h)
if(MSVC)
  find_library(GLFW_LIBRARY NAMES glfw3dll)
else()
  find_library(GLFW_LIBRARY NAMES glfw3 glfw)
endif(MSVC)
set(GLFW_LIBRARIES ${GLFW_LIBRARY})
file(RELATIVE_PATH GLFW_HEADER_PATH ${GLFW_INCLUDE_DIRS} ${GLFW_FULL_HEADER_PATH})
message(STATUS "Found glfw include: " ${GLFW_INCLUDE_DIRS})
message(STATUS "Found glfw header: " ${GLFW_HEADER_PATH})
message(STATUS "Found glfw libs: " ${GLFW_LIBRARIES})

find_path(GLM_INCLUDE_PATH glm/glm.hpp)
set(GLM_INCLUDE_DIRS ${GLM_INCLUDE_PATH})
message(STATUS "Found glm include: " ${GLM_INCLUDE_DIRS})

configure_file(config.h.in config.h)

include_directories(AFTER SYSTEM ${GLFW_INCLUDE_DIRS} ${GLM_INCLUDE_DIRS} ${graphplay_BINARY_DIR})

add_subdirectory(graphplay)
add_subdirectory(graphplay-test)
add_subdirectory(vendor/gtest)

enable_testing()
add_test(NAME graphplay-test
  WORKING_DIRECTORY .
  COMMAND graphplay-test)
