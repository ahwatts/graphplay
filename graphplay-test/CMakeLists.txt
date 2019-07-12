add_executable(graphplay-test
    fzx/BodyTest.cpp
    gfx/CameraTest.cpp
    gfx/GeometryTest.cpp
    gfx/MeshTest.cpp
    gfx/SceneTest.cpp
    gfx/ShaderTest.cpp
    gfx/TestOpenGLContext.cpp
    load/PlyFileTest.cpp)
target_link_libraries(graphplay-test
    PUBLIC graphplay_engine gtest gtest_main)

# if(MSVC AND CMAKE_CXX_COMPILER_VERSION STRLESS "18.0")
#     set_target_properties(graphplay-test PROPERTIES COMPILE_DEFINITIONS _VARIADIC_MAX=10)
# endif(MSVC AND CMAKE_CXX_COMPILER_VERSION STRLESS "18.0")
