add_library(
    Event
    event.cpp
    event.h)
add_library(
    DeathEvent
    death_event.cpp
    death_event.h)
add_library(
    BuntEvent
    bunt_event.cpp
    bunt_event.h)
add_library(
    HitEvent
    hit_event.cpp
    hit_event.h)

target_link_libraries(DeathEvent PRIVATE Event)
target_link_libraries(BuntEvent PRIVATE Event)
target_link_libraries(HitEvent PRIVATE Event)
