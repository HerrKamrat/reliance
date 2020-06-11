
cmake_minimum_required(VERSION 3.0.0)

project(reliance VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(main 
src/main.cpp
src/react.cpp
)


add_subdirectory(3rdparty/arcana.cpp)
add_subdirectory(3rdparty/Catch2)

target_link_libraries(main arcana Catch2)
