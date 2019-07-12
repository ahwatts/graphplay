add_library(graphplay_engine
    Driver.cpp
    Input.cpp
    fzx/BBox.cpp
    fzx/Body.cpp
    fzx/Constraint.cpp
    fzx/PhysicsSystem.cpp
    gfx/Camera.cpp
    gfx/Geometry.cpp
    gfx/Mesh.cpp
    gfx/OpenGLUtils.cpp
    gfx/Scene.cpp
    gfx/Shader.cpp
    load/PlyFile.cpp)
target_compile_features(graphplay_engine PUBLIC cxx_std_11)
target_link_libraries(graphplay_engine
    PUBLIC glad glfw Boost::filesystem)

add_executable(graphplay graphplay.cpp)
target_link_libraries(graphplay
    PUBLIC graphplay_engine)

if (WIN32)
    add_executable(graphplay-console
        graphplay.cpp)

    target_link_libraries(graphplay-console
        PUBLIC graphplay_engine)

    if(MINGW)
        set_target_properties(graphplay PROPERTIES
            LINK_FLAGS "-Wl,--subsystem,windows")
    endif(MINGW)

    if(MSVC)
        set_target_properties(graphplay PROPERTIES
            LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:\"mainCRTStartup\"")
    endif(MSVC)
endif(WIN32)
