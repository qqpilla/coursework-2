cmake_minimum_required(VERSION 3.20)
project(coursework2 LANGUAGES C CXX)

set(sources 
    ./src/main.cpp
)

add_subdirectory(./external/glfw)
add_subdirectory(./external/glad)
add_subdirectory(./external/glm)
add_subdirectory(./external/imgui)

add_executable(program ${sources})
target_include_directories(program
PRIVATE
    ./src
)
target_link_libraries(program
PRIVATE
    glm
    imgui
# Не нужно линковаться к glfw и glad, т.к. imgui уже линкуется к ним
)