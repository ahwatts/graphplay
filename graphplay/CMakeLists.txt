add_library(graphplay_engine
  Body.cpp
  Camera.cpp
  Collada.cpp
  Constraint.cpp
  Geometry.cpp
  Mesh.cpp
  OpenGLUtils.cpp
  PhysicsSystem.cpp
  PlyFile.cpp
  Scene.cpp
  Shader.cpp)

add_executable(graphplay WIN32
  graphplay.cpp)

target_link_libraries(graphplay
  tinyxml2
  ${GLFW_LIBRARIES}
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
    ${GLFW_LIBRARIES}
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
