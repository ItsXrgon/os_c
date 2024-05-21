cmake_minimum_required(VERSION 3.12)
project(main)

# Enable C11
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

# Assuming your header files are in a folder named "headers"
include_directories(Headers)


# Add source files
set(SOURCES
    src/Main/Main.c
    src/queue/Queue.c
    # Add more source files here if needed
)

# Add executable target
add_executable(main  ${SOURCES})

# Add include directories
target_include_directories(main PUBLIC include)