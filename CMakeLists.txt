cmake_minimum_required(VERSION 3.10)
project(LethalAI)

find_package(Torch REQUIRED)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(helpers)
add_subdirectory(environment)
add_subdirectory(gym)
add_subdirectory(rl)

add_library(LethalAI SHARED lethalai.cpp)

target_link_libraries(LethalAI PRIVATE StateObserver)
target_link_libraries(LethalAI PRIVATE InputHandler)
target_link_libraries(LethalAI PRIVATE Logger)
target_link_libraries(LethalAI PRIVATE CodeInjector)
target_link_libraries(LethalAI PRIVATE EventObserver)
target_link_libraries(LethalAI PRIVATE LethalGym)
target_link_libraries(LethalAI PRIVATE MainDqn)
target_link_libraries(LethalAI PRIVATE DqnAgent)

if (MSVC)
  file(GLOB TORCH_DLLS "${TORCH_INSTALL_PREFIX}/lib/*.dll")
  add_custom_command(TARGET LethalAI
                     POST_BUILD
                     COMMAND ${CMAKE_COMMAND} -E copy_if_different
                     ${TORCH_DLLS}
                     $<TARGET_FILE_DIR:LethalAI>)
endif (MSVC)
