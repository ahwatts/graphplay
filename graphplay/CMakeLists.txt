add_executable(graphplay Body.cpp Collada.cpp Geometry.cpp Material.cpp Mesh.cpp Scene.cpp graphplay.cpp tinyxml2.cpp)
target_link_libraries(graphplay ${GLFW_LIBRARIES} ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES})
