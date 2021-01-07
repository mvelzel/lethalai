add_library(
    LethalGym
    lethal_gym.cpp
    lethal_gym.h)

target_link_libraries(LethalGym PRIVATE StateObserver)
target_link_libraries(LethalGym PRIVATE InputHandler)
target_link_libraries(LethalGym PRIVATE CodeInjector)
target_link_libraries(LethalGym PRIVATE EventObserver)
