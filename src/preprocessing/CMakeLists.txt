add_library(cch_preprocessing
        RankOrderer.cpp
        ContractionBuilder.cpp
)

target_link_libraries(cch_preprocessing PUBLIC cch_core)
target_include_directories(cch_preprocessing PUBLIC .)
