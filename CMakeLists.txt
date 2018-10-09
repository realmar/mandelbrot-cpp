cmake_minimum_required(VERSION 3.8)
set(CMAKE_CXX_STANDARD 17)
project(mandlebrot)
include_directories(GPU)
add_executable(mandlebrot   GPU/main.cpp
                            GPU/shader_util.cpp
                            GPU/shader_util.hpp
                            GPU/vertex_object.cpp
                            GPU/vertex_object.hpp)

find_package(GLEW REQUIRED)
target_link_libraries(mandlebrot PRIVATE GLEW::GLEW)

find_package(glfw3 REQUIRED)
target_link_libraries(mandlebrot PRIVATE glfw)

find_package(opengl REQUIRED)
target_link_libraries(mandlebrot PRIVATE opengl32)

file(COPY GPU/shader.frag DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY GPU/rect.frag DESTINATION ${CMAKE_CURRENT_BINARY_DIR})