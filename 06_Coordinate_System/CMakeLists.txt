cmake_minimum_required(VERSION 3.10)
project(Project)

# Add glad manually from the top-level folder
add_library(glad STATIC
    ${CMAKE_SOURCE_DIR}/../include/glad/src/glad.c
)

target_include_directories(glad PUBLIC
    ${CMAKE_SOURCE_DIR}/../glad/include
)

# Now the actual app
add_executable(main main.cpp)

target_include_directories(main PRIVATE
    ${CMAKE_SOURCE_DIR}/../include/glad/include
    ${CMAKE_SOURCE_DIR}/../include/
)

target_link_libraries(main PRIVATE glad glfw)

# Copy shaders to build directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/texture.vs DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/texture.fs DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

#texture
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/container.jpg DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/mangekyou_sharingan.png DESTINATION ${CMAKE_CURRENT_BINARY_DIR})



