cmake_minimum_required(VERSION 3.14)
project(joystick C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

include(FetchContent)

# Fetch SDL2
FetchContent_Declare(
        SDL2
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-2.30.11
)
FetchContent_MakeAvailable(SDL2)

# Glob all source files
file(GLOB_RECURSE SOURCE_FILES src/*.c)
file(GLOB_RECURSE HEADER_FILES include/*.h)

add_library(lib STATIC ${HEADER_FILES} ${SOURCE_FILES})
set_target_properties(lib PROPERTIES LINKER_LANGUAGE C)

# Define the executable
add_executable(${PROJECT_NAME}
        main.c
        ${SOURCE_FILES}
        ${HEADER_FILES}
)

# Include directories
include_directories(
        include
        ${SDL2_SOURCE_DIR}/include
)
target_include_directories(${PROJECT_NAME} PRIVATE
        include
        ${SDL2_SOURCE_DIR}/include
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE lib)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2-static)
