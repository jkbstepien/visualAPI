cmake_minimum_required(VERSION 3.22)
project(visualAPI)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.10.5/json.tar.xz)
FetchContent_MakeAvailable(json)

add_executable(visualAPI main.cpp)
target_link_libraries(visualAPI PRIVATE nlohmann_json::nlohmann_json curl)
