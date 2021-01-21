add_library(
    Text
    text.cpp
    text.h)
add_library(
    InfoList
    info_list.cpp
    info_list.h)
add_library(
    Graph
    graph.cpp
    graph.h)

target_link_libraries(InfoList PRIVATE Text)
target_link_libraries(Graph PRIVATE Text)
