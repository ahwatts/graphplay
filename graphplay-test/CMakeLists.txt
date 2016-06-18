add_executable(graphplay-test
  BodyTest.cpp
  CameraTest.cpp
  GeometryTest.cpp
  MeshTest.cpp
  PlyFileTest.cpp
  SceneTest.cpp
  ShaderTest.cpp
  TestOpenGLContext.cpp)

include_directories(AFTER SYSTEM ../vendor/gtest/include)

target_link_libraries(graphplay-test
  tinyxml2
  ${GLFW_LIBRARIES}
  glad
  ${OPENGL_LIBRARIES}
  ${CMAKE_DL_LIBS}
  graphplay_engine
  gtest gtest_main)

if(MSVC AND CMAKE_CXX_COMPILER_VERSION STRLESS "18.0")
  set_target_properties(graphplay-test PROPERTIES COMPILE_DEFINITIONS _VARIADIC_MAX=10)
endif(MSVC AND CMAKE_CXX_COMPILER_VERSION STRLESS "18.0")
