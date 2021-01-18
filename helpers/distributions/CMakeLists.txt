add_library(
    Distribution
    distribution.cpp
    distribution.h)
add_library(
    Categorical
    categorical.cpp
    categorical.h)

target_link_libraries(Distribution "${TORCH_LIBRARIES}")
target_link_libraries(Categorical PRIVATE Distribution)
