#include "Main.h"

int main(int argc, char** argv) {
    std::string requestedEffect = "NONE";

    // Parse the arguments
    CLI::App app{NAME " " VERSION "\n" DESCRIPTION};
    argv = app.ensure_utf8(argv);

    // TODO
    app.add_flag("-e,--effect", requestedEffect, "Name of the effect to load upon startup")
        ->check(CLI::IsMember(effectNames));

    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError &e) {
        std::exit(app.exit(e));
    }

    // A GUI and renderer are created
    GUI* gui = new GUI(getEffectTypeFromName(requestedEffect));
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
        SDL_GL_SwapWindow(window);

        // Handle quit signals
        if (gui->quit) break;
    }

    return 0;
}
