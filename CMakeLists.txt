cmake_minimum_required(VERSION 3.12)
project(ArnoldSandbox VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(external/ArnoldEngine)

add_executable(${PROJECT_NAME}
    src/Sandbox.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE ArnoldEngine)