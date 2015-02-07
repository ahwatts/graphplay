add_executable(graphplay-test
  CameraTest.cpp
  GeometryTest.cpp
  MaterialTest.cpp
  MeshTest.cpp
  SceneTest.cpp
  TestOpenGLContext.cpp
  ../graphplay/Camera.cpp
  ../graphplay/Geometry.cpp
  ../graphplay/Material.cpp
  ../graphplay/Mesh.cpp
  ../graphplay/Scene.cpp
  ../graphplay/Shader.cpp)

include_directories(AFTER SYSTEM ../vendor/gtest/include)

target_link_libraries(graphplay-test
  ${GLFW_LIBRARIES}
  ${GLEW_LIBRARIES}
  ${OPENGL_LIBRARIES}
  gtest gtest_main)

if(MSVC AND CMAKE_CXX_COMPILER_VERSION STRLESS "18.0")
  set_target_properties(graphplay-test PROPERTIES COMPILE_DEFINITIONS _VARIADIC_MAX=10)
endif(MSVC AND CMAKE_CXX_COMPILER_VERSION STRLESS "18.0")
