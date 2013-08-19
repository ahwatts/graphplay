add_executable(graphplay-test GeometryTest.cpp
                              MaterialTest.cpp
                              TestOpenGLContext.cpp
                              ../graphplay/Geometry.cpp
                              ../graphplay/Material.cpp)
target_link_libraries(graphplay-test ${GTEST_LIBRARIES}
                                     ${GTEST_MAIN_LIBRARIES}
                                     ${GLFW_LIBRARIES}
                                     ${GLEW_LIBRARIES}
                                     ${OPENGL_LIBRARIES})

if(MSVC AND CMAKE_CXX_COMPILER_VERSION STRLESS "18.0")
  set_target_properties(graphplay-test PROPERTIES COMPILE_DEFINITIONS _VARIADIC_MAX=10)
endif(MSVC AND CMAKE_CXX_COMPILER_VERSION STRLESS "18.0")

if(MSVC)
  set_target_properties(graphplay-test PROPERTIES COMPILE_FLAGS /MTd)
endif(MSVC)
