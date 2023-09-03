cmake_minimum_required (VERSION 3.12+)

project ("inpdl")

file(GLOB_RECURSE SOURCE_FILES src/*.cpp src/*.h src/*.hpp)
add_executable (${CMAKE_PROJECT_NAME} ${SOURCE_FILES})

set_property(TARGET ${CMAKE_PROJECT_NAME} PROPERTY CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)

# TODO: Add tests and install targets if needed.
install(TARGETS inpdl RUNTIME DESTINATION bin)