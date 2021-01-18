add_library(
    ActorCritic
    actor_critic.h
    actor_critic.cpp)
add_library(
    DuelingDqn
    dueling_dqn.h
    dueling_dqn.cpp)
add_library(
    ReplayBuffer
    replay_buffer.h
    replay_buffer.cpp)

target_link_libraries(ActorCritic "${TORCH_LIBRARIES}")
target_link_libraries(DuelingDqn "${TORCH_LIBRARIES}")
