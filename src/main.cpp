#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <GLES3/gl3.h>

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"

// Application state structure
struct AppState {
    SDL_Window* window = nullptr;
    SDL_GLContext gl_context = nullptr;
    float scale = 1.0f;
    bool show_demo_window = true;
    ImVec4 clear_color = ImVec4(0.12f, 0.14f, 0.18f, 1.00f);
    int click_counter = 0;
};

// Called once when the application starts
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    // Optional: lock screen orientation to landscape
    SDL_SetHint(SDL_HINT_ORIENTATIONS, "LandscapeLeft LandscapeRight");

    // Initialize SDL3 video subsystem
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
    {
        SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // Allocate application state
    AppState* state = new AppState();
    *appstate = state;

    // Configure OpenGL ES 3.0 context attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    // Create SDL window
    SDL_WindowFlags window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY;
    state->window = SDL_CreateWindow("Dear ImGui SDL3 GLES3", 0, 0, window_flags);
    if (!state->window)
    {
        SDL_Log("Failed to create SDL window: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // Create OpenGL ES context
    state->gl_context = SDL_GL_CreateContext(state->window);
    if (!state->gl_context)
    {
        SDL_Log("Failed to create OpenGL ES context: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_GL_MakeCurrent(state->window, state->gl_context);
    SDL_GL_SetSwapInterval(1); // Enable V-Sync

    // Initialize Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_IsTouchScreen;

    // Apply scaling for High-DPI mobile touch screens
    state->scale = SDL_GetDisplayContentScale(SDL_GetDisplayForWindow(state->window));
    if (state->scale <= 0.0f)
    {
        state->scale = 1.0f;
    }
    ImGui::GetStyle().ScaleAllSizes(state->scale);
    io.FontGlobalScale = state->scale;

    // Set dark theme style
    ImGui::StyleColorsDark();

    // Initialize platform and OpenGL ES 3.0 renderers
    ImGui_ImplSDL3_InitForOpenGL(state->window, state->gl_context);
    const char* glsl_version = "#version 300 es";
    ImGui_ImplOpenGL3_Init(glsl_version);

    return SDL_APP_CONTINUE;
}

// Called for every incoming SDL event
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
    AppState* state = (AppState*)appstate;

    ImGui_ImplSDL3_ProcessEvent(event);

    if (event->type == SDL_EVENT_QUIT)
    {
        return SDL_APP_SUCCESS; // Request application to terminate cleanly
    }

    if (event->type == SDL_EVENT_WINDOW_CLOSE_REQUESTED &&
        event->window.windowID == SDL_GetWindowID(state->window))
    {
        return SDL_APP_SUCCESS;
    }

    return SDL_APP_CONTINUE;
}

// Called once per frame to update and render
SDL_AppResult SDL_AppIterate(void* appstate)
{
    AppState* state = (AppState*)appstate;
    ImGuiIO& io = ImGui::GetIO();

    // Start Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    // 1. Show the Dear ImGui demo window
    if (state->show_demo_window)
    {
        ImGui::ShowDemoWindow(&state->show_demo_window);
    }

    // 2. Custom touch-friendly window
    {
        ImGui::Begin("Dear ImGui with SDL3 & GLES3 on Android");

        ImGui::Text("Application average: %.3f ms/frame (%.1f FPS)",
                    1000.0f / io.Framerate, io.Framerate);
        ImGui::Text("Screen scale factor: %.2f", state->scale);

        ImGui::Separator();

        ImGui::Checkbox("Show ImGui Demo Window", &state->show_demo_window);

        // Large button easy to tap on mobile screens
        if (ImGui::Button("Tap Me!", ImVec2(180 * state->scale, 60 * state->scale)))
        {
            state->click_counter++;
        }
        ImGui::SameLine();
        ImGui::Text("Taps count: %d", state->click_counter);

        ImGui::Separator();
        ImGui::ColorEdit3("Background Color", (float*)&state->clear_color);

        ImGui::End();
    }

    // Rendering
    ImGui::Render();

    int display_w, display_h;
    SDL_GetWindowSizeInPixels(state->window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);

    glClearColor(state->clear_color.x * state->clear_color.w,
                 state->clear_color.y * state->clear_color.w,
                 state->clear_color.z * state->clear_color.w,
                 state->clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(state->window);

    return SDL_APP_CONTINUE;
}

// Called once when the application is shutting down
void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
    (void)result;
    AppState* state = (AppState*)appstate;

    if (state)
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();

        if (state->gl_context)
        {
            SDL_GL_DestroyContext(state->gl_context);
        }
        if (state->window)
        {
            SDL_DestroyWindow(state->window);
        }

        delete state;
    }

    SDL_Quit();
}
