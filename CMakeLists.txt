cmake_minimum_required (VERSION 3.12+)

project ("inpdl" LANGUAGES C)
include_directories(${CMAKE_SOURCE_DIR}/lib)
file(GLOB_RECURSE LIB_SOURCE_FILES lib/*.c lib/*.h)
file(GLOB_RECURSE SOURCE_FILES src/*.c src/*.h)
add_executable (${CMAKE_PROJECT_NAME} ${SOURCE_FILES} ${LIB_SOURCE_FILES})

set_property(TARGET ${CMAKE_PROJECT_NAME} PROPERTY C_STANDARD 11)

# TODO: Add tests and install targets if needed.
install(TARGETS inpdl RUNTIME DESTINATION bin)