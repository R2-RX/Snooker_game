cmake_minimum_required(VERSION 3.22.1)

project(Billiard_Game)

include_directories(include)

add_library(Billiard_Lib 
    src/Ball.cpp
    src/Table.cpp
    src/Cue.cpp
    src/Renderer.cpp)

add_executable(Billiard_Game app/main.cpp)

set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER  
    "include/Game_components/Ball.h;include/Game_components/Table.h;include/Game_components/Cue.h;include/Graphic/Renderer.h")

#target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/build)

target_link_libraries(Billiard_Game PRIVATE Billiard_Lib 
    sfml-graphics 
    sfml-window 
    sfml-system 
    pthread)
