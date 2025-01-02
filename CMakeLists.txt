cmake_minimum_required(VERSION 3.12)
project(ArnoldSandbox VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(USE_ITERM "Use iTerm2 for running the application" ON)

add_subdirectory(external/ArnoldEngine)

add_executable(${PROJECT_NAME}
    src/Sandbox.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE ArnoldEngine)

if(USE_ITERM)
    add_custom_target(run
        COMMAND open -a iTerm "${CMAKE_BINARY_DIR}/ArnoldSandbox"
        DEPENDS ArnoldSandbox
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
else()
    add_custom_target(run
        COMMAND ${CMAKE_BINARY_DIR}/ArnoldSandbox
        DEPENDS ArnoldSandbox
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        USES_TERMINAL
    )
endif()