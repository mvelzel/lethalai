cmake_minimum_required(VERSION 3.10)
project(LethalAI)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(helpers)
add_subdirectory(environment)

add_library(LethalAI SHARED lethalai.cpp)

target_link_libraries(LethalAI PRIVATE StateObserver)
target_link_libraries(LethalAI PRIVATE InputHandler)
target_link_libraries(LethalAI PRIVATE Logger)
target_link_libraries(LethalAI PRIVATE CodeInjector)
