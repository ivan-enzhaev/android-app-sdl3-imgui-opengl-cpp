cmake_minimum_required(VERSION 3.21)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

project(myapp CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -----------------------------------------------------------------------------
# Paths
# -----------------------------------------------------------------------------
set(SDL3_DIR "C:/libs/sdl3-3.4.18-android")
set(IMGUI_DIR "C:/libs/imgui-1.93.0")

# -----------------------------------------------------------------------------
# Native Shared Library Target
# -----------------------------------------------------------------------------
add_library(main SHARED
    src/main.cpp
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdl3.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

# -----------------------------------------------------------------------------
# Include Directories
# -----------------------------------------------------------------------------
target_include_directories(main PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${SDL3_DIR}/${ANDROID_ABI}/include
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

# -----------------------------------------------------------------------------
# Compile Definitions
# -----------------------------------------------------------------------------
# Enable OpenGL ES 3.0 path in imgui_impl_opengl3
target_compile_definitions(main PRIVATE
    IMGUI_IMPL_OPENGL_ES3
)

# -----------------------------------------------------------------------------
# Link SDL3 & Android System Libraries
# -----------------------------------------------------------------------------
target_link_libraries(main PRIVATE
    ${SDL3_DIR}/${ANDROID_ABI}/lib/libSDL3.so
    GLESv3
    EGL
    android
    log
)