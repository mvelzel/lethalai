add_subdirectory(agents)

add_library(
    MainDqn
    main_dqn.cpp
    main_dqn.h)
add_library(
    DqnAgent
    dqn_agent.cpp
    dqn_agent.h)
add_library(
    MultiAgent
    multi_agent.cpp
    multi_agent.h)
add_library(
    ActionTranslator
    action_translator.cpp
    action_translator.h)

target_link_libraries(MainDqn "${TORCH_LIBRARIES}")
target_link_libraries(DqnAgent "${TORCH_LIBRARIES}")
target_link_libraries(MultiAgent "${TORCH_LIBRARIES}" ActionTranslator)
