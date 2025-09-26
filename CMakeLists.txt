cmake_minimum_required(VERSION 3.26)
project(CustomizableContractionHierarchy)

set(CMAKE_CXX_STANDARD 17)

# Add subdirectories
add_subdirectory(core)
add_subdirectory(preprocessing)
add_subdirectory(customization)
add_subdirectory(query)
add_subdirectory(api)

# Demo executable
add_executable(cch_demo main.cpp)
target_link_libraries(cch_demo PRIVATE cch_api)
