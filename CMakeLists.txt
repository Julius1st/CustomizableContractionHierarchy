cmake_minimum_required(VERSION 3.26)
project(CustomizableContractionHierarchy)

set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_SOURCE_DIR}/src)

# Add subdirectories
add_subdirectory(src/core)
add_subdirectory(src/preprocessing)
add_subdirectory(src/customization)
add_subdirectory(src/query)
add_subdirectory(src/api)

# Demo executable
add_executable(cch_demo main.cpp)
target_link_libraries(cch_demo PRIVATE cch_api)
