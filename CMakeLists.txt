cmake_minimum_required(VERSION 2.8)

project(adrenaline)
set (CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS -O3)

include_directories(source third_party)

set(SOURCE_FILES
    "source/main.cpp"
    "source/importor.h"
    "source/importor.cpp"
    "source/helper"
    "source/window.h"
    "source/window.cpp"
    "source/sdlwindow.h"
    "source/sdlwindow.cpp"
    "source/sdlglcontext.h"
    "source/sdlglcontext.cpp"
    "source/input.h"
    "source/input.cpp"
    "source/application.h"
    "source/application.cpp"
    "source/model.h"
    "source/model.cpp"
    "source/mpart.h"
    "source/header.h"
    "source/shader.h"
    "source/shader.cpp"
    "source/texture.h"
    "source/texture.cpp"
    "source/context.h"
    "source/actions.h"
    "source/random_generator.h"
    "source/random_generator.cpp"
    "source/mouse_controller.h"
    "source/mouse_controller.cpp"
    "source/camera.h"
    "source/camera.cpp"
    "source/maze.cpp"
    "source/maze.h"
    "source/geometrics.cpp"
    source/geometrics.h 
    source/collision.cpp 
    source/collision.h
    source/PrimitiveManager.cpp
    source/PrimitiveManager.h
    source/opengl_objects.cpp
    source/opengl_objects.h
    source/ParticleManager.cpp
    source/ParticleManager.h
    source/particle_tick_functions.cpp
    source/particle_tick_functions.h
    source/CameraManager.h
    source/CameraManager.cpp
    source/ShaderManager.cpp        
    source/ShaderManager.h
    source/context.cpp
    source/player.cpp
    source/player.h
    source/crp_functions.h
    source/ctv_functions.h
    source/crp_functions.cpp
    source/ctv_functions.cpp
    source/view.h
    source/play_view.cpp
    source/play_view.h
    source/home_view.cpp
    source/home_view.h
    source/bullet.h
    source/weapon_manager.cpp
    source/weapon_manager.h
    source/weapon.cpp
    source/weapon.h
    source/bullet_functions.cpp
    source/bullet_functions.h
    source/AI.cpp
    source/AI.h
    source/AIModel.cpp
    source/AIModel.h
    source/AIManager.cpp
    source/AIManager.h
    source/drawing_obj.h
    source/MedKitManager.cpp
    source/MedKitManager.h
    source/MiniMap.cpp
    source/MiniMap.h)

set(SHADERS
        shader/weapon_v.glsl
        shader/weapon_f.glsl
        )

set(THIRD_PARTY_SOURCE
    "third_party/glad/glad.c"
    )

find_package(SDL2 REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${THIRD_PARTY_SOURCE} ${SHADERS})

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} ${CMAKE_DL_LIBS})
