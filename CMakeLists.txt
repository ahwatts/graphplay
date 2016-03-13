cmake_minimum_required(VERSION 2.8.10)
project(graphplay)

message(STATUS "Configuring for build type: " ${CMAKE_BUILD_TYPE})

set(CMAKE_PREFIX_PATH ../graphplay-deps)

if(CMAKE_COMPILER_IS_GNUCXX)
  if(CMAKE_CXX_COMPILER_VERSION STRLESS "4.8")
    set(CMAKE_CXX_FLAGS "-std=gnu++11 -Wall")
  else()
    set(CMAKE_CXX_FLAGS "-std=c++11 -Wall")
  endif(CMAKE_CXX_COMPILER_VERSION STRLESS "4.8")

  set(CMAKE_CXX_FLAGS_DEBUG "-ggdb -pg")

  if(NOT APPLE)
    set(CMAKE_EXE_LINKER_FLAGS "-Wl,--as-needed")
  endif(NOT APPLE)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "-std=c++11 -Wall")
endif(CMAKE_COMPILER_IS_GNUCXX)

if(WIN32 AND MINGW)
  add_definitions(-DGLEW_STATIC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static")
endif(WIN32 AND MINGW)

find_package(Boost REQUIRED)
message(STATUS "Found Boost version: " ${Boost_VERSION})
message(STATUS "Found Boost headers: " ${Boost_INCLUDE_DIRS})

find_package(Threads REQUIRED)
message(STATUS "Found thread libs: " ${CMAKE_THREAD_LIBS_INIT})
message(STATUS "Using pthreads: " ${CMAKE_USE_PTHREADS_INIT})

find_package(OpenGL REQUIRED)
message(STATUS "Found OpenGL include: " ${OPENGL_INCLUDE_DIRS})
message(STATUS "Found OpenGL header: " ${OPENGL_HEADER_PATH})
message(STATUS "Found OpenGL libs: " ${OPENGL_LIBRARIES})

if(NOT ("${CMAKE_SYSTEM_NAME}" MATCHES "Darwin"))
  find_package(GLEW REQUIRED)
  message(STATUS "Found GLEW include: " ${GLEW_INCLUDE_DIRS})
  message(STATUS "Found GLEW header: " ${GLEW_HEADER_PATH})
  message(STATUS "Found GLEW libs: " ${GLEW_LIBRARIES})
endif(NOT ("${CMAKE_SYSTEM_NAME}" MATCHES "Darwin"))

find_path(GLFW_INCLUDE_DIRS NAMES GL/glfw3.h GLFW/glfw3.h)
find_file(GLFW_FULL_HEADER_PATH NAMES GL/glfw3.h GLFW/glfw3.h)
if(MSVC)
  find_library(GLFW_LIBRARY NAMES glfw3dll)
else()
  find_library(GLFW_LIBRARY NAMES glfw3 glfw glfw3dll glfwdll)
endif(MSVC)
set(GLFW_LIBRARIES ${GLFW_LIBRARY})
file(RELATIVE_PATH GLFW_HEADER_PATH ${GLFW_INCLUDE_DIRS} ${GLFW_FULL_HEADER_PATH})
message(STATUS "Found GLFW include: " ${GLFW_INCLUDE_DIRS})
message(STATUS "Found GLFW header: " ${GLFW_HEADER_PATH})
message(STATUS "Found GLFW libs: " ${GLFW_LIBRARIES})

find_path(GLM_INCLUDE_PATH glm/glm.hpp PATHS ${CMAKE_PREFIX_PATH}/include NO_DEFAULT_PATH)
find_path(GLM_INCLUDE_PATH glm/glm.hpp)
set(GLM_INCLUDE_DIRS ${GLM_INCLUDE_PATH})
message(STATUS "Found glm include: " ${GLM_INCLUDE_DIRS})

configure_file(config.h.in config.h)

include_directories(AFTER SYSTEM ${Boost_INCLUDE_DIRS} ${GLFW_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} ${GLM_INCLUDE_DIRS} ${graphplay_BINARY_DIR})

add_subdirectory(graphplay)
add_subdirectory(graphplay-test)
add_subdirectory(vendor/gtest)

enable_testing()
add_test(NAME graphplay-test
  WORKING_DIRECTORY .
  COMMAND graphplay-test)
