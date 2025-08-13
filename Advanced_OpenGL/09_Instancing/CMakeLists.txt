cmake_minimum_required(VERSION 3.10)
project(Project)

# Add glad manually from the top-level folder
add_library(glad STATIC
    ${CMAKE_SOURCE_DIR}/../../include/glad/src/glad.c
)

target_include_directories(glad PUBLIC
    ${CMAKE_SOURCE_DIR}/../../glad/include
)

# Now the actual app
add_executable(main main.cpp)

find_package(assimp REQUIRED)

target_include_directories(main PRIVATE
    ${CMAKE_SOURCE_DIR}../../include/glad/include
    ${CMAKE_SOURCE_DIR}/../../include/
)

# Link everything
target_link_libraries(main
    PRIVATE
        glad
        glfw
        assimp::assimp   
)

