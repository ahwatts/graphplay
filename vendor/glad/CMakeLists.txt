add_library(glad src/glad.c)

target_include_directories(glad PUBLIC include)

if(WIN32 OR MINGW)
else()
  target_link_libraries(glad INTERFACE ${CMAKE_DL_LIBS})
endif()
