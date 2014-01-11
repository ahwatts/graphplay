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
  ../graphplay/Scene.cpp)

target_link_libraries(graphplay-test
  ${GTEST_LIBRARIES}
  ${GTEST_MAIN_LIBRARIES}
  ${GLFW_LIBRARIES}
  ${GLEW_LIBRARIES}
  ${OPENGL_LIBRARIES})

if(MSVC AND CMAKE_CXX_COMPILER_VERSION STRLESS "18.0")
  set_target_properties(graphplay-test PROPERTIES COMPILE_DEFINITIONS _VARIADIC_MAX=10)
endif(MSVC AND CMAKE_CXX_COMPILER_VERSION STRLESS "18.0")
