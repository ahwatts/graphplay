add_executable(graphplay-test
  CameraTest.cpp
  GeometryTest.cpp
  MeshTest.cpp
  PlyFileTest.cpp
  SceneTest.cpp
  ShaderTest.cpp
  TestOpenGLContext.cpp
  ../graphplay/Body.cpp
  ../graphplay/Camera.cpp
  ../graphplay/Collada.cpp
  ../graphplay/Geometry.cpp
  ../graphplay/Mesh.cpp
  ../graphplay/OpenGLUtils.cpp
  ../graphplay/PlyFile.cpp
  ../graphplay/PhysicsSystem.cpp
  ../graphplay/Scene.cpp
  ../graphplay/Shader.cpp
  ../graphplay/tinyxml2.cpp)

include_directories(AFTER SYSTEM ../vendor/gtest/include)

target_link_libraries(graphplay-test
  ${GLFW_LIBRARIES}
  glad
  ${OPENGL_LIBRARIES}
  ${CMAKE_DL_LIBS}
  gtest gtest_main)

if(MSVC AND CMAKE_CXX_COMPILER_VERSION STRLESS "18.0")
  set_target_properties(graphplay-test PROPERTIES COMPILE_DEFINITIONS _VARIADIC_MAX=10)
endif(MSVC AND CMAKE_CXX_COMPILER_VERSION STRLESS "18.0")
