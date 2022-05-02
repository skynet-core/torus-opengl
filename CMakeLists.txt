cmake_minimum_required(VERSION 3.20)

get_filename_component(ProjectId ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" ProjectId ${ProjectId})

project(${ProjectId} VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
endif()

find_package(OpenGL REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)


set(PROJECT_ROOT_FOLDER ${CMAKE_CURRENT_SOURCE_DIR})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config/config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/config/config.h)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests)
endif()

add_test(NAME TestUtils COMMAND torus_test)


file(GLOB COMPILER_SOURCES "src/*.c" "src/*.cpp")

add_executable(${ProjectId} main.cpp ${COMPILER_SOURCES})
target_include_directories(${ProjectId} PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/config"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/include")

target_link_libraries(${ProjectId} PRIVATE 
    OpenGL::GL 
    fmt::fmt-header-only
    imgui::imgui
    glad::glad
    glm::glm
    glfw)