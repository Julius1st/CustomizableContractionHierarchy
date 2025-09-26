add_library(cch_query
        EliminationTreeQuery.cpp
)

target_link_libraries(cch_query PUBLIC cch_core)
target_include_directories(cch_query PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
