add_executable(graphplay-test GeometryTest.cpp
                              ../graphplay/Geometry.cpp
                              ../graphplay/Material.cpp)
target_link_libraries(graphplay-test ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES} ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES})
