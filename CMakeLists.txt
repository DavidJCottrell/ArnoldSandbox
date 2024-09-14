cmake_minimum_required(VERSION 3.5)
project(ArnoldSandbox VERSION 0.0.1 LANGUAGES CXX)

SET(CMAKE_CXX_STANDARD 20)

add_subdirectory(external/ArnoldEngine)

add_executable(${PROJECT_NAME} 
    src/main.cpp
)

target_link_libraries(${PROJECT_NAME} ArnoldEngine)
target_include_directories(${PROJECT_NAME} PRIVATE ArnoldEngine)