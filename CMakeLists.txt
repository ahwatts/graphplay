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
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static")
endif(WIN32 AND MINGW)

find_package(Boost REQUIRED COMPONENTS filesystem system)
message(STATUS "Found Boost version: " ${Boost_VERSION})
message(STATUS "Found Boost headers: " ${Boost_INCLUDE_DIRS})
message(STATUS "Found Boost libs:    " ${Boost_LIBRARIES})

find_package(Threads REQUIRED)
message(STATUS "Found thread libs: " ${CMAKE_THREAD_LIBS_INIT})
message(STATUS "Using pthreads: " ${CMAKE_USE_PTHREADS_INIT})

find_package(OpenGL REQUIRED)
message(STATUS "Found OpenGL include: " ${OPENGL_INCLUDE_DIRS})
message(STATUS "Found OpenGL header: " ${OPENGL_HEADER_PATH})
message(STATUS "Found OpenGL libs: " ${OPENGL_LIBRARIES})

# Find GLFW.
find_package(glfw3 QUIET)
if(NOT ${glfw3_FOUND})
  find_path(__GLFW_INCLUDE_DIRS NAMES GL/glfw3.h GLFW/glfw3.h)

  if(MSVC)
    find_library(__GLFW_LIBRARY NAMES glfw3dll)
  else()
    find_library(__GLFW_LIBRARY NAMES glfw3 glfw glfw3dll glfwdll)
  endif(MSVC)
  set(__GLFW_LIBRARIES ${__GLFW_LIBRARY})

  add_library(glfw STATIC IMPORTED)
  set_property(TARGET glfw PROPERTY IMPORTED_LOCATION ${__GLFW_LIBRARY})
  set_property(TARGET glfw PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${__GLFW_INCLUDE_DIRS})
  set_property(TARGET glfw PROPERTY INTERFACE_LINK_LIBRARIES ${__GLFW_LIBRARIES})
endif(NOT ${glfw3_FOUND})

# Report where we found GLFW.
get_target_property(GLFW_LOCATION glfw IMPORTED_LOCATION)
get_target_property(GLFW_INCLUDE_DIRS glfw INTERFACE_INCLUDE_DIRECTORIES)
get_target_property(GLFW_LIBRARIES glfw INTERFACE_LINK_LIBRARIES)
message(STATUS "Found GLFW: " ${GLFW_LOCATION})
message(STATUS "Found GLFW include: " ${GLFW_INCLUDE_DIRS})
message(STATUS "Found GLFW libs: " ${GLFW_LIBRARIES})

# Find GLM.
find_package(glm QUIET)
if(NOT ${glm_FOUND})
  find_path(GLM_INCLUDE_PATH glm/glm.hpp PATHS ${CMAKE_PREFIX_PATH}/include NO_DEFAULT_PATH)
  find_path(GLM_INCLUDE_PATH glm/glm.hpp)
  add_library(glm STATIC IMPORTED)
  set_property(TARGET glm PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${GLM_INCLUDE_PATH})
endif(NOT ${glm_FOUND})

# Report where we found GLM.
get_target_property(GLM_INCLUDE_DIRS glm INTERFACE_INCLUDE_DIRECTORIES)
message(STATUS "Found glm include: " ${GLM_INCLUDE_DIRS})

# Generate config.h.
find_file(GLFW_FULL_HEADER_PATH NAMES GL/glfw3.h GLFW/glfw3.h)
file(RELATIVE_PATH GLFW_HEADER_PATH ${GLFW_INCLUDE_DIRS} ${GLFW_FULL_HEADER_PATH})
configure_file(config.h.in config.h)

include_directories(AFTER SYSTEM
  ${Boost_INCLUDE_DIRS}
  glfw
  glm
  vendor/glad/include
  vendor/tinyxml2/include
  ${graphplay_BINARY_DIR})

add_subdirectory(graphplay)
add_subdirectory(graphplay-test)
add_subdirectory(vendor/gtest)
add_subdirectory(vendor/glad)
add_subdirectory(vendor/tinyxml2)

enable_testing()
add_test(NAME graphplay-test
  WORKING_DIRECTORY .
  COMMAND graphplay-test)
