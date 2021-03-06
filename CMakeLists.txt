# Copyright (c)
# 2021 Sharjeel Ali <sharjeelali@cmail.carleton.ca>
# 2018 David Mould <mould@scs.carleton.ca>

cmake_minimum_required(VERSION 2.6)

#Set the target plaform of the project to be win32
if(WIN32)
	#set(CMAKE_GENERATOR_PLATFORM win32)
endif(WIN32)

# Name of project
set(PROJ_NAME COMP3009_FinalProject)
project(${PROJ_NAME})

#Include both the src (.cpp) and include (.h) directories and the resource (shaders)
include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/resource)
include_directories(${PROJECT_SOURCE_DIR}/assets)

# Specify project source files
set(SRCS
    ${PROJECT_SOURCE_DIR}/main.cpp

    ${PROJECT_SOURCE_DIR}/src/camera.cpp
    ${PROJECT_SOURCE_DIR}/src/quad.cpp
    ${PROJECT_SOURCE_DIR}/src/fpsCamera.cpp
    ${PROJECT_SOURCE_DIR}/src/GameTime.cpp
    ${PROJECT_SOURCE_DIR}/src/GameInput.cpp
    ${PROJECT_SOURCE_DIR}/src/Scene.cpp
    ${PROJECT_SOURCE_DIR}/src/SceneManager.cpp
    ${PROJECT_SOURCE_DIR}/src/TextRenderer.cpp

    ${PROJECT_SOURCE_DIR}/include/camera.h
    ${PROJECT_SOURCE_DIR}/include/geometry.h
    ${PROJECT_SOURCE_DIR}/include/quad.h
    ${PROJECT_SOURCE_DIR}/include/fpsCamera.h
    ${PROJECT_SOURCE_DIR}/include/GameTime.h
    ${PROJECT_SOURCE_DIR}/include/GameInput.h
    ${PROJECT_SOURCE_DIR}/include/Scene.h
    ${PROJECT_SOURCE_DIR}/include/SceneManager.h
    ${PROJECT_SOURCE_DIR}/include/TextRenderer.h
	
    ${PROJECT_SOURCE_DIR}/resource/rayMarchShader.vert
    ${PROJECT_SOURCE_DIR}/resource/rayMarchShader.frag
    ${PROJECT_SOURCE_DIR}/resource/textShader.frag
    ${PROJECT_SOURCE_DIR}/resource/textShader.vert
)

# Add executable based on the source files
add_executable(${PROJ_NAME} ${SRCS})

# Require OpenGL library
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
target_link_libraries(${PROJ_NAME} ${OPENGL_gl_LIBRARY})

#Set the path to the libraries folder downloaded from culearn.
set(LIBRARY_PATH C:/libraries)

#Include the header files
include_directories(${LIBRARY_PATH}/include)

#find libraries for linux/Mac OS users
if(NOT WIN32)
    find_library(GLEW_LIBRARY GLEW)
    find_library(GLFW_LIBRARY GLFW)
    find_library(SOIL_LIBRARY SOIL)

#Find libraries for windows users
elseif(WIN32)
    find_library(GLEW_LIBRARY glew32s HINTS ${LIBRARY_PATH}/lib)
    find_library(GLFW_LIBRARY glfw3 HINTS ${LIBRARY_PATH}/lib)
    find_library(SOIL_LIBRARY SOIL HINTS ${LIBRARY_PATH}/lib)

endif(NOT WIN32)

target_link_libraries(${PROJ_NAME} ${GLEW_LIBRARY})
target_link_libraries(${PROJ_NAME} ${GLFW_LIBRARY})
target_link_libraries(${PROJ_NAME} ${SOIL_LIBRARY})

# This will use the proper libraries in debug mode in Visual Studio
if(WIN32)
    set_target_properties(${PROJ_NAME} PROPERTIES DEBUG_POSTFIX _d)
endif(WIN32)
