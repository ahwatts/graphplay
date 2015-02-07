add_executable(graphplay WIN32
  Body.cpp
  Camera.cpp
  Collada.cpp
  Geometry.cpp
  Material.cpp
  Mesh.cpp
  Scene.cpp
  Shader.cpp
  graphplay.cpp
  tinyxml2.cpp)

target_link_libraries(graphplay
  ${GLFW_LIBRARIES}
  ${GLEW_LIBRARIES}
  ${OPENGL_LIBRARIES})

if (WIN32)
  add_executable(graphplay-console
    Body.cpp
    Camera.cpp
    Collada.cpp
    Geometry.cpp
    Material.cpp
    Mesh.cpp
    Scene.cpp
    graphplay.cpp
    tinyxml2.cpp)

  target_link_libraries(graphplay-console
    ${GLFW_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${OPENGL_LIBRARIES})

  if(MINGW)
    set_target_properties(graphplay PROPERTIES
      LINK_FLAGS "-Wl,--subsystem,windows")
  endif(MINGW)

  if(MSVC)
    set_target_properties(graphplay PROPERTIES
      LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:\"mainCRTStartup\"")
  endif(MSVC)
endif(WIN32)
