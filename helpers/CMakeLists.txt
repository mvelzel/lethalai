add_subdirectory(distributions)

add_library(
    Logger
    logger.cpp
    logger.h)
add_library(
    CodeInjector
    code_injector.cpp
    code_injector.h)
add_library(
    Math
    math_helpers.cpp
    math_helpers.h)
