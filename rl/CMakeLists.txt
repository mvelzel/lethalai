set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")

add_library(
    MainDqn
    main_dqn.cpp
    main_dqn.h)
add_library(
    DqnAgent
    dqn_agent.cpp
    dqn_agent.h)

target_link_libraries(MainDqn "${TORCH_LIBRARIES}")
target_link_libraries(DqnAgent "${TORCH_LIBRARIES}")
