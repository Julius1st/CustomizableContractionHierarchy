add_library(cch_api
        CCH.cpp
)

target_link_libraries(cch_api
        PUBLIC cch_core
        cch_preprocessing
        cch_customization
        cch_query
)

target_include_directories(cch_api PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
