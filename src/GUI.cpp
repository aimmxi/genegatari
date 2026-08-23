#include "GUI.h"
#include "Icon.cpp"

SDL_Window* window = nullptr;
int32_t windowWidth, windowHeight;

/**
 * @brief Construct a new GUI::GUI object
 * @param e The effect to load initially 
 */
GUI::GUI(EffectType newEffect) {
    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) print(ERROR, "%s\n", SDL_GetError());
    IMG_Init(IMG_INIT_PNG);

    // GLUT's nonsense for setup
    int argc = 1;
    char *argv[2];

    argv[0] = NULL;
    argv[1] = NULL;
    
    // GLUT setup
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

    // Create main window in a hidden state
    window = SDL_CreateWindow(NAME, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, INITIAL_WIDTH, INITIAL_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    glContext = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, glContext);
    SDL_GL_SetSwapInterval(1); // Enable vsync
    
    // Create a surface with the icon data and set the icon
    SDL_Surface* iconSurface = SDL_CreateRGBSurfaceFrom((void*) iconData.pixelData, iconData.width, iconData.height, 32, iconData.width * 4, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);

    if (iconSurface) {
        SDL_SetWindowIcon(window, iconSurface);
        SDL_FreeSurface(iconSurface);
    }

    // Also load the image to a texture for future use
    glGenTextures(1, &iconTexture);
    glBindTexture(GL_TEXTURE_2D, iconTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iconData.width, iconData.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, iconData.pixelData);
    
    // ImGui setup
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    
    // Change the accent color and apply more styling
    applyAccentColor(accentColor);
    ImGuiStyle &style = ImGui::GetStyle();
    style.FrameBorderSize = true;               // Adds borders to inputs, checkboxes...
    style.ScrollbarRounding = 0.0f;             // Removes ugly rounding from the scrollbar

    // Additional setup
    ImGui_ImplSDL2_InitForOpenGL(window, glContext);
    ImGui_ImplOpenGL3_Init("#version 130");
    glEnable(GL_TEXTURE_2D);

    // Create the new effect
    currentEffect = newEffect;
    switchEffect(effect, currentEffect);
}

GUI::~GUI() {}

/*
 *  Windows
 */
/**
 * @brief Renders the main window.
 */
void GUI::menuBar() {
    // Calculate FPS
    unsigned int currentTime = SDL_GetTicks();
    frameTime = (currentTime - lastTime) / 1000.0f;     // ms to s
    lastTime = currentTime;
    fps = static_cast<int>(1.0f / frameTime);

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Reload Effect", "Ctrl+R")) switchEffect(effect, currentEffect);

            ImGui::Separator();

            // For each effect
            for (int e = 0; e < NUM_EFFECT_TYPES; ++e) {
                // A new item in the drop down gets inserted
                if (ImGui::MenuItem(getEffectTypeName((EffectType) e))) {
                    // If the widget is selected, the effect gets changed
                    switchEffect(effect, (EffectType) e);
                    currentEffect = (EffectType) e;
                }
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Quit", "Ctrl+Q"))              quit = true;

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View")) {
            if (ImGui::MenuItem("Toggle GUI", "Ctrl+G"))        toggleGUI();
            if (ImGui::MenuItem("Toggle Fullscreen", "Crtl+F")) toggleFullscreen();
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Help")) {
            ImGui::MenuItem("About This Effect", "Crtl+H");
            if (ImGui::MenuItem("About Genegatari", "Ctrl+A"))  renderAboutGenegatari = true;
            ImGui::EndMenu();
        }

        // Send the FPS counter to the right of the top bar
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() - ImGui::GetStyle().ItemSpacing.x - ImGui::CalcTextSize("000 FPS").x);
        ImGui::Text("%d FPS", fps);

        ImGui::EndMainMenuBar();
    }
}

/**
 * @brief Displays a window showing help and information about Genegatari.
 */
void GUI::aboutGenegatari() {
    // Position the about window on the center and change it's size
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(600.0f, 475.0f));

    // Open the about window
    ImGui::OpenPopup("About Genegatari");

    if (ImGui::BeginPopupModal("About Genegatari", nullptr, ImGuiWindowFlags_MenuBar)) {
        // Center and draw the logo
        centerNextItem(iconData.width / 2);
        ImGui::Image((ImTextureID) iconTexture, ImVec2(iconData.width / 2, iconData.height / 2));

        ImGui::SetWindowFontScale(2.0f);
        centerNextItem(ImGui::CalcTextSize(NAME).x);
        ImGui::Text(NAME);
        ImGui::SetWindowFontScale(1.0f);

        ImGui::SameLine();
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 12.0);
        ImGui::Text(VERSION);

        ImGui::Dummy(ImVec2(0.0f, 20.0f));

        centerNextItem(ImGui::CalcTextSize(DESCRIPTION).x);
        ImGui::Text(DESCRIPTION);
        ImGui::Dummy(ImVec2(0.0f, 20.0f));

        // Scrollable effect list
        ImGui::BeginGroup();
            ImGui::BeginChild("EffectList", ImVec2(ImGui::GetContentRegionAvail().x, 100.0f), ImGuiChildFlags_Borders, ImGuiWindowFlags_MenuBar);

            // Add the title inside of a menu bar
            if (ImGui::BeginMenuBar()) {
                ImGui::TextUnformatted("Available effects:");
                ImGui::EndMenuBar();
            }

            // Add all the effects
            for (int i = 1; i < NUM_EFFECT_TYPES; i++) {
                ImGui::Text("%s", getEffectTypeName((EffectType) i));
            }

            ImGui::EndChild();
        ImGui::EndGroup();

        ImGui::Dummy(ImVec2(0.0f, 20.0f));
        centerNextItem(ImGui::CalcTextSize("Close").x);
        if (ImGui::Button("Close")) renderAboutGenegatari = false;

        ImGui::EndPopup();
    }
}

/*
 *  Helpers
 */
void GUI::centerNextItem(float width) {
    float availSpace = ImGui::GetContentRegionAvail().x;
    float pos = (availSpace - width) * 0.5f;
    if (pos > 0.0f) ImGui::SetCursorPosX(ImGui::GetCursorPosX() + pos);
}

/**
 * @brief Changes the accent color universally. Code by HAMM3REXTREME on GitHub (https://github.com/ocornut/imgui/issues/707#issuecomment-4646409022)
 * @param accent_color
 */
void GUI::applyAccentColor(const ImVec4 &accent) {
    ImGuiStyle &style = ImGui::GetStyle();

    // clamp values between 0.0f and 1.0f
    auto clampColor = [](float val) { return std::clamp(val, 0.0f, 1.0f); };

    // Calculate slightly darker/shifted versions for hovered/active states
    ImVec4 hovered_color = ImVec4(clampColor(accent.x * 0.85f), clampColor(accent.y * 0.85f), clampColor(accent.z * 0.85f), accent.w);
    ImVec4 active_color = ImVec4(clampColor(accent.x * 0.70f), clampColor(accent.y * 0.70f), clampColor(accent.z * 0.70f), accent.w);
    ImVec4 muted_color = ImVec4(clampColor(accent.x * 0.15f), clampColor(accent.y * 0.15f), clampColor(accent.z * 0.15f), 0.45f);

    // Core interactive elements
    style.Colors[ImGuiCol_Header] = accent;
    style.Colors[ImGuiCol_HeaderHovered] = hovered_color;
    style.Colors[ImGuiCol_HeaderActive] = active_color;

    style.Colors[ImGuiCol_Button] = accent;
    style.Colors[ImGuiCol_ButtonHovered] = hovered_color;
    style.Colors[ImGuiCol_ButtonActive] = active_color;

    // Background fields (checkboxes, inputs, drop-downs)
    style.Colors[ImGuiCol_FrameBg] = muted_color; // Gives a subtle hint
    style.Colors[ImGuiCol_FrameBgHovered] = hovered_color;
    style.Colors[ImGuiCol_FrameBgActive] = active_color;

    // Sliders and Selectables
    style.Colors[ImGuiCol_SliderGrab] = active_color;
    style.Colors[ImGuiCol_SliderGrabActive] = accent;
    style.Colors[ImGuiCol_CheckMark] = accent;

    // Tabs & Tab Overline
    style.Colors[ImGuiCol_Tab] = hovered_color;
    style.Colors[ImGuiCol_TabHovered] = accent;
    style.Colors[ImGuiCol_TabActive] = active_color;
    style.Colors[ImGuiCol_TabUnfocused] = ImVec4(hovered_color.x, hovered_color.y, hovered_color.z, 0.4f);
    style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(active_color.x, active_color.y, active_color.z, 0.6f);
    style.Colors[ImGuiCol_TabSelectedOverline] = accent; // The thin active accent bar over the selected tab

    // Window / Docking Highlights
    style.Colors[ImGuiCol_TitleBgActive] = active_color;
    style.Colors[ImGuiCol_SeparatorHovered] = hovered_color;
    style.Colors[ImGuiCol_SeparatorActive] = accent;
    style.Colors[ImGuiCol_ResizeGrip] = hovered_color;
    style.Colors[ImGuiCol_ResizeGripHovered] = accent;
    style.Colors[ImGuiCol_ResizeGripActive] = active_color;

    // Navigation & Selection Highlighting
    style.Colors[ImGuiCol_NavHighlight] = accent;                                                  // The keyboard/gamepad focus indicator line
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(accent.x, accent.y, accent.z, 0.35f); // Selected text backdrop

    // Plot lines / Histograms
    style.Colors[ImGuiCol_PlotLines] = accent;
    style.Colors[ImGuiCol_PlotLinesHovered] = hovered_color;
}


void GUI::toggleGUI() {
    print(INFO, "Toggling GUI\n");
    displayGui = !displayGui;
}

void GUI::toggleFullscreen() {
    print(INFO, "Toggling fullscreen\n");
    if (isFullscreen) {
        // Switch to windowed mode
        SDL_SetWindowFullscreen(window, 0); 
        SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);  // Reposition to center
    } else {
        // Switch to fullscreen mode
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP); 
    }

    isFullscreen = !isFullscreen;
}

/*
 *  Public functions
 */
/**
 * @brief Renders the background according to the state of currentEffect.
 */
void GUI::renderBackground() {
    // Get the current resolution of the main window
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);

    
    // Render the effect
    if (effect != nullptr) effect->render();
}

/**
 * @brief Renders all the ImGui windows that should be displayed.
 */
void GUI::renderMenus() {
    // ImGui::ShowDemoWindow(&renderAboutGenegatari);

    if (displayGui) {
        // Always shown items, main top menu bar and effect settings
        menuBar();
        if (effect != nullptr)      effect->effectSettings();

        // Other menus
        if (renderAboutGenegatari)  aboutGenegatari();
    }
}


/**
 * @brief Checks for keypresses on the main window and modifies the GUI accordingly.
 */
void GUI::checkKeyPresses() {
    ImGuiIO& io = ImGui::GetIO();

    // File
    if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_R))  switchEffect(effect, currentEffect);
    if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Q))  quit = true;

    // View
    if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_G))  toggleGUI();       
    if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_F))  toggleFullscreen();
    
    // Help
    if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_A))  renderAboutGenegatari = !renderAboutGenegatari;
}