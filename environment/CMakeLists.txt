add_subdirectory(game_model)

add_library(
    StateObserver
    state_observer.cpp
    state_observer.h)
add_library(
    GameState
    game_state.cpp
    game_state.h)
add_library(
    InputHandler
    input_handler.cpp
    input_handler.h)

target_link_libraries(GameState PRIVATE Player)
target_link_libraries(InputHandler PRIVATE Player)
target_link_libraries(GameState PRIVATE Ball)
target_link_libraries(StateObserver PRIVATE GameState)
