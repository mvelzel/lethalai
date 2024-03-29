cmake_minimum_required(VERSION 3.10)
project(LethalAI)

set(CMAKE_PREFIX_PATH "D:/pytorch;D:/freeglut")

find_package(Torch REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

include_directories(${OPENGL_INCLUDE_DIR} ${TORCH_INCLUDE_DIR} ${GLUT_INCLUDE_DIR})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")


add_subdirectory(helpers)
add_subdirectory(environment)
add_subdirectory(gym)
add_subdirectory(rl)
add_subdirectory(graphics)

add_library(LethalAI SHARED lethalai.cpp)

target_link_libraries(LethalAI PRIVATE StateObserver)
target_link_libraries(LethalAI PRIVATE InputHandler)
target_link_libraries(LethalAI PRIVATE Logger)
target_link_libraries(LethalAI PRIVATE CodeInjector)
target_link_libraries(LethalAI PRIVATE EventObserver)
target_link_libraries(LethalAI PRIVATE LethalGym)
target_link_libraries(LethalAI PRIVATE MainDqn)
target_link_libraries(LethalAI PRIVATE DqnAgent)
target_link_libraries(LethalAI PRIVATE DuelingDqn)
target_link_libraries(LethalAI PRIVATE Categorical)
target_link_libraries(LethalAI PRIVATE ActorCritic)
target_link_libraries(LethalAI PRIVATE ReplayBuffer)
target_link_libraries(LethalAI PRIVATE MultiAgent)
target_link_libraries(LethalAI PRIVATE GraphicsHandler)
target_link_libraries(LethalAI PRIVATE InfoList)
target_link_libraries(LethalAI PRIVATE Graph)

if (MSVC)
  file(GLOB TORCH_DLLS "${TORCH_INSTALL_PREFIX}/lib/*.dll")
  add_custom_command(TARGET LethalAI
                     POST_BUILD
                     COMMAND ${CMAKE_COMMAND} -E copy_if_different
                     ${TORCH_DLLS}
                     $<TARGET_FILE_DIR:LethalAI>)
endif (MSVC)
