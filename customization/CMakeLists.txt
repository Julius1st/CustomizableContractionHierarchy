add_library(cch_customization
        BasicCustomizer.cpp
        PerfectCustomizer.cpp
)

target_link_libraries(cch_customization PUBLIC cch_core)
target_include_directories(cch_customization PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
