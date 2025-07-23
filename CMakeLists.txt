cmake_minimum_required(VERSION 3.10)
project(OpenGL_Project)

set(CMAKE_CXX_STANDARD 17)

# Add glad
add_subdirectory(include/glad)

# Include GLFW (assumes installed via system or package manager)
find_package(glfw3 REQUIRED)

add_executable(main main.cpp)

# Glad includes
target_include_directories(main
    PRIVATE
    include/glad/include
    include/stb_image.h

)

# Link GLAD + GLFW + OpenGL
target_link_libraries(main
    glad
    glfw
    ${CMAKE_DL_LIBS}
)

