cmake_minimum_required(VERSION 3.10)
project(3DTerrain)

set(CMAKE_CXX_STANDARD 17)

# Include directories
include_directories(include)

# GLFW - sử dụng thư viện local
set(GLFW_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include/GLFW")
set(GLFW_LIB_DIR "${CMAKE_SOURCE_DIR}/lib")

if(WIN32)
    set(OPENGL_LIB opengl32)
else()
    find_package(OpenGL REQUIRED)
    set(OPENGL_LIB ${OPENGL_LIBRARIES})
endif()

# Source files
file(GLOB_RECURSE SOURCES "src/*.cpp")

set(GLAD_C_FILE "${CMAKE_SOURCE_DIR}/src/glad.c")
if(EXISTS "${GLAD_C_FILE}")
    add_executable(3DTerrain ${SOURCES} "${GLAD_C_FILE}")
    message(STATUS "Found glad.c - GLAD loader will be included")
else()
    message(WARNING 
        "glad.c not found! Creating a minimal stub.\n"
        "For full functionality, please generate glad.c from:\n"
        "  https://glad.dav1d.de/#profile=compatibility&language=c&specification=gl&loader=on&api=gl%3D3.3\n"
        "Download glad.c and khrplatform.h, place them in src/ and include/KHR/ respectively"
    )
    add_executable(3DTerrain ${SOURCES})
endif()

# Link libraries
target_include_directories(3DTerrain PRIVATE ${GLFW_INCLUDE_DIR})
target_link_directories(3DTerrain PRIVATE ${GLFW_LIB_DIR})
if(EXISTS "${GLFW_LIB_DIR}/libglfw3.a")
    target_link_libraries(3DTerrain PRIVATE glfw3 ${OPENGL_LIB})
elseif(EXISTS "${GLFW_LIB_DIR}/libglfw3dll.a")
    target_link_libraries(3DTerrain PRIVATE glfw3dll ${OPENGL_LIB})
    add_custom_command(TARGET 3DTerrain POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${GLFW_LIB_DIR}/glfw3.dll"
        $<TARGET_FILE_DIR:3DTerrain>)
else()
    message(FATAL_ERROR "GLFW library not found in ${GLFW_LIB_DIR}")
endif()
file(COPY "${CMAKE_SOURCE_DIR}/assets" DESTINATION "${CMAKE_BINARY_DIR}")