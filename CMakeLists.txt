cmake_minimum_required(VERSION 3.16)
project(mybot)

set(CMAKE_CXX_STANDARD 17)

# Include directories
include_directories(include libs/json/include)

# Add source files
file(GLOB SOURCES "srcs/*.cpp")

# Link DPP and CURL libraries
add_executable(mybot ${SOURCES})
target_link_libraries(mybot dpp curl)
