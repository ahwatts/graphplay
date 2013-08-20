add_executable(graphplay WIN32 Body.cpp Collada.cpp Geometry.cpp Material.cpp Mesh.cpp Scene.cpp graphplay.cpp tinyxml2.cpp)
target_link_libraries(graphplay ${GLFW_LIBRARIES} ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES})

if(CMAKE_COMPILER_IS_GNUCXX)
  set_target_properties(graphplay PROPERTIES
                        LINK_FLAGS "-Wl,--subsystem,windows")
endif(CMAKE_COMPILER_IS_GNUCXX)

if(MSVC)
  set(CMAKE_CREATE_WIN32_EXE "/SUBSYSTEM:WINDOWS /ENTRY:\"mainCRTStartup\"")
endif(MSVC)
