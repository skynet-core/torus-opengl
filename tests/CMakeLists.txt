cmake_minimum_required(VERSION 3.20)
project(torus_test VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)

find_package(Catch2 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

file(GLOB COMPILER_SOURCES 
"${CMAKE_CURRENT_SOURCE_DIR}/../src/*.c" 
"${CMAKE_CURRENT_SOURCE_DIR}/../src/*.cpp" 
"${CMAKE_CURRENT_SOURCE_DIR}/*.c" 
"${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_executable(torus_test ${COMPILER_SOURCES} "utils_test.cpp" "catch_main.h")

target_include_directories(torus_test PRIVATE 
"${CMAKE_CURRENT_SOURCE_DIR}/../config" 
"${CMAKE_CURRENT_SOURCE_DIR}/../src/include")
target_link_libraries(torus_test PRIVATE 
    OpenGL::GL 
    fmt::fmt-header-only
    imgui::imgui
    glad::glad
    glm::glm
    glfw
    Catch2::Catch2)