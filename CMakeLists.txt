cmake_minimum_required(VERSION 3.16)

project(genegatari)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)


include_directories(include)
include_directories(lib/imgui)
include_directories(lib/imgui/backends)

# Project source files
set(PROJECT_SOURCES
    src/Common.cpp
    src/Effect.cpp
    src/EffectNone.cpp
    src/EffectTest.cpp
    src/EffectPerlin.cpp
    src/EffectCellAutomaton.cpp
    src/GUI.cpp
    src/Main.cpp
)

# ImGui source files
set(IMGUI_SOURCES
    lib/imgui/imgui.cpp
    lib/imgui/imgui_draw.cpp
    lib/imgui/imgui_tables.cpp
    lib/imgui/imgui_widgets.cpp
    lib/imgui/backends/imgui_impl_sdl2.cpp
    lib/imgui/backends/imgui_impl_opengl3.cpp
)

# Combine both
add_executable(genegatari
    ${PROJECT_SOURCES}
    ${IMGUI_SOURCES}
)

# Flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

# Link SDL2 and OpenGL
find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(GLUT REQUIRED)
find_package(OpenMP REQUIRED)
target_link_libraries(genegatari
    SDL2::SDL2
    SDL2_image::SDL2_image
    OpenGL::GL
    GLUT::GLUT
    OpenMP::OpenMP_CXX
)
