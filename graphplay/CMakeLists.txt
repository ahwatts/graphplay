add_library(graphplay_engine
  PlyFile.cpp
  fzx/BBox.cpp
  fzx/Body.cpp
  fzx/Constraint.cpp
  fzx/PhysicsSystem.cpp
  gfx/Camera.cpp
  gfx/Geometry.cpp
  gfx/Mesh.cpp
  gfx/OpenGLUtils.cpp
  gfx/Scene.cpp
  gfx/Shader.cpp)

add_executable(graphplay WIN32
  graphplay.cpp)

target_link_libraries(graphplay
  glfw
  glad
  ${OPENGL_LIBRARIES}
  ${Boost_LIBRARIES}
  ${CMAKE_DL_LIBS}
  graphplay_engine)

if (WIN32)
  add_executable(graphplay-console
    graphplay.cpp)

  target_link_libraries(graphplay-console
    tinyxml2
    glfw
    glad
    ${OPENGL_LIBRARIES}
    ${Boost_LIBRARIES}
    graphplay_engine)

  if(MINGW)
    set_target_properties(graphplay PROPERTIES
      LINK_FLAGS "-Wl,--subsystem,windows")
  endif(MINGW)

  if(MSVC)
    set_target_properties(graphplay PROPERTIES
      LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:\"mainCRTStartup\"")
  endif(MSVC)
endif(WIN32)
