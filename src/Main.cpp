#include "GUI.h"

#include <iostream>

int main(int, char**) {
    // A GUI and renderer are created
    GUI* gui = new GUI();
    ImGuiIO& io = ImGui::GetIO();

    // Main loop
    bool running = true;
    while (running) {
        // Listen for window events and handle graceful window closing
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                running = false;
        }

        // Start frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // Listen for keypresses
        gui->checkKeyPresses();

        // Render whatever the background is configured to be
        gui->renderBackground();

        // Render ImGui menus
        gui->renderMenus();

        // Render and display the frame
        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(GUI::window);
    }

    return 0;
}
