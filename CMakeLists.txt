cmake_minimum_required(VERSION 3.14.0)
project(graphplay)

set(Boost_USE_STATIC_LIBS On)
set(OpenGL_GL_PREFERENCE "GLVND")

find_package(Threads REQUIRED)
find_package(Boost REQUIRED COMPONENTS filesystem)
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

add_subdirectory(vendor/glad)
add_subdirectory(vendor/gtest)
# add_subdirectory(vendor/tinyxml2)

add_subdirectory(graphplay)
add_subdirectory(graphplay-test)

# enable_testing()
# add_test(NAME graphplay-test
#   WORKING_DIRECTORY .
#   COMMAND graphplay-test)
