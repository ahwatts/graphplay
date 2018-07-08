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

target_include_directories(graphplay_engine PUBLIC
  ${GLM_INCLUDE_DIRS}
  ${graphplay_BINARY_DIR})

target_link_libraries(graphplay_engine PUBLIC
  glad)

add_executable(graphplay WIN32
  graphplay.cpp)

target_link_libraries(graphplay
  graphplay_engine
  glfw
  OpenGL::GL
  Boost::filesystem
  Boost::system)

if (WIN32)
  add_executable(graphplay-console
    graphplay.cpp)

  target_link_libraries(graphplay-console
    graphplay_engine
    glfw
    OpenGL::GL
    Boost::filesystem
    Boost::system)

  if(MINGW)
    set_target_properties(graphplay PROPERTIES
      LINK_FLAGS "-Wl,--subsystem,windows")
  endif(MINGW)

  if(MSVC)
    set_target_properties(graphplay PROPERTIES
      LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:\"mainCRTStartup\"")
  endif(MSVC)
endif(WIN32)
