add_library(cch_core
        Graph.cpp
)

# Expose the project src/ folder so "core/Graph.hpp" resolves
target_include_directories(cch_core PUBLIC ${CMAKE_SOURCE_DIR}/src)