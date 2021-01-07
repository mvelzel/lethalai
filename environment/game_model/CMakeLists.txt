add_library(
    Player
    player.cpp
    player.h)
add_library(
    Ball
    ball.cpp
    ball.h)


target_link_libraries(Ball PRIVATE Math)
target_link_libraries(Player PRIVATE Math)
target_link_libraries(Ball PRIVATE GameState)
target_link_libraries(Player PRIVATE GameState)
