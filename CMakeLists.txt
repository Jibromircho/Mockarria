cmake_minimum_required(VERSION 3.10)
project(OpenGLProject VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)

cmake_policy(SET CMP0072 NEW)  # This sets the policy to prefer GLVND.
# cmake_policy(SET CMP0072 OLD)  # Uncomment this if you prefer the legacy GL library.
find_package(OpenGL REQUIRED)

# Add include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Add source files
set(SOURCES
    src/main.cpp
    src/world.cpp
    src/functions.cpp
    
    
)

# Add executable
add_executable(Mockarria ${SOURCES})

# Link libraries
target_link_libraries(Mockarria raylib dl GL)
