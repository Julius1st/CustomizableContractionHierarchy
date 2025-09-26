add_library(cch_core
        Graph.cpp
        EliminationTree.cpp
)

target_include_directories(cch_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
