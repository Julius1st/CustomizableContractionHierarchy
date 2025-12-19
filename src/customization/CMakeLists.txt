add_library(cch_customization
        BasicCustomizer.cpp
        DistancePreprocessing.cpp
        PerfectCustomizer.cpp
        PerfectCustomizer.hpp
)

target_link_libraries(cch_customization PUBLIC cch_core)
target_include_directories(cch_customization PUBLIC .)
