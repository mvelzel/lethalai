include_directories(${GLUT_INCLUDE_DIR})

add_subdirectory(drawables)

add_library(
    Drawable
    drawable.cpp
    drawable.h)
add_library(
    GraphicsHandler
    graphics_handler.cpp
    graphics_handler.h)

target_link_libraries(GraphicsHandler ${GLUT_LIBRARY} ${OPENGL_LIBRARIES} Drawable)
