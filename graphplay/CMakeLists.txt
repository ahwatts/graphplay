add_executable(graphplay WIN32
  Body.cpp
  Camera.cpp
  Collada.cpp
  Geometry.cpp
  Mesh.cpp
  OpenGLUtils.cpp
  PlyFile.cpp
  Scene.cpp
  Shader.cpp
  graphplay.cpp
  tinyxml2.cpp)

target_link_libraries(graphplay
  ${GLFW_LIBRARIES}
  glad
  ${OPENGL_LIBRARIES}
  Boost::filesystem
  ${CMAKE_DL_LIBS})

if (WIN32)
  add_executable(graphplay-console
    Body.cpp
    Camera.cpp
    Collada.cpp
    Geometry.cpp
    Mesh.cpp
    OpenGLUtils.cpp
    PlyFile.cpp
    Scene.cpp
    Shader.cpp
    graphplay.cpp
    tinyxml2.cpp)

  target_link_libraries(graphplay-console
    ${GLFW_LIBRARIES}
    glad
    ${OPENGL_LIBRARIES}
    Boost::filesystem)

  if(MINGW)
    set_target_properties(graphplay PROPERTIES
      LINK_FLAGS "-Wl,--subsystem,windows")
  endif(MINGW)

  if(MSVC)
    set_target_properties(graphplay PROPERTIES
      LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:\"mainCRTStartup\"")
  endif(MSVC)
endif(WIN32)
