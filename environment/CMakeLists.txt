add_subdirectory(game_model)
add_subdirectory(events)

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
add_library(
    EventObserver
    event_observer.cpp
    event_observer.h)

target_link_libraries(GameState PRIVATE Player)
target_link_libraries(InputHandler PRIVATE Player)
target_link_libraries(GameState PRIVATE Ball)
target_link_libraries(GameState PRIVATE Math)
target_link_libraries(StateObserver PRIVATE GameState)
target_link_libraries(EventObserver PRIVATE Event)
target_link_libraries(EventObserver PRIVATE DeathEvent)
target_link_libraries(EventObserver PRIVATE BuntEvent)
target_link_libraries(EventObserver PRIVATE HitEvent)
