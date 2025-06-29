#include "GUI.h"

GUI::GUI() {
    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        std::cerr << "Error: " << SDL_GetError() << "\n";
    }
    IMG_Init(IMG_INIT_PNG);

    // GLUT's nonsense for setup
    int argc = 1;
    char *argv[1];
    
    // GLUT setup
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

    // Create main window in a hidden state
    window = SDL_CreateWindow(NAME, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, INITIAL_WIDTH, INITIAL_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    // GLEW setuo
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW initialization error" << std::endl;
        exit(EXIT_FAILURE);
    }
    
    // Set SDL's icon
    // TODO Embed the icon in the application itself instead of using routes.
    // SDL_Surface* icon = IMG_Load("../resources/icon.png");
    // if (icon == nullptr) {
    //     std::cerr << "Error loading icon: " << IMG_GetError() << std::endl;
    //     exit(EXIT_FAILURE);
    // }
    // SDL_SetWindowIcon(window, icon);
    // SDL_FreeSurface(icon);
    
    // ImGui setup
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    // Additional setuo
    const char* glsl_version = "#version 130";
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);
    glEnable(GL_TEXTURE_2D);
}

GUI::~GUI () {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    // TODO add GLEW and GLUT ?
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
}


/**
 * @brief Renders the main window.
 */
void GUI::mainWindow() {
    // Calculate FPS
    unsigned int currentTime = SDL_GetTicks();
    frameTime = (currentTime - lastTime) / 1000.0f;     // ms to s
    lastTime = currentTime;
    fps = static_cast<int>(1.0f / frameTime);

    if (ImGui::Begin("General settings", nullptr, ImGuiWindowFlags_NoCollapse)) {
        ImGui::Text("Welcome to the general settings menu.");
        ImGui::Text(" > To toggle the GUI press 'g'");
        ImGui::Text(" > To toggle fullscreen press 'f'");

        ImGui::Text("");
        ImGui::Separator();
        ImGui::Text("");

        ImGui::Text("FPS: %d", fps);

        ImGui::Text("");
        ImGui::Separator();
        ImGui::Text("");

        // A drop down box gets inserted to select effects
        if (ImGui::BeginCombo("Effect", EFFECTS[currentEffect].c_str())) {
            // For each effect
            for (int i = 0; i < EFFECTS.size(); ++i) {
                // A new item in the drop down gets inserted
                if (ImGui::Selectable(EFFECTS[i].c_str(), (currentEffect == i))) {
                    // If the widget is selected, the effect gets changed
                    std::cout << "Switching to effect: " << EFFECTS[i].c_str() << std::endl;
                    currentEffect = i;
                    hasEffectChanged = true;
                }
            }
            ImGui::EndCombo();
        }
        
        // A button to open the effect's config is added
        if (ImGui::Button("Toggle effect's settings")) {
            if (e != nullptr) {
                displayEffectSettings = !displayEffectSettings;
            }
        }

        ImGui::End();
    }
}

/// Public functions
/**
 * @brief Renders the background according to the state of currentEffect.
 */
void GUI::renderBackground(){
    // If a different effect has been picked, it gets instantiated.
    if (hasEffectChanged) {
        // If there is an old instance of an effect, it gets deleted before creating a new one
        if (e != nullptr) {
            delete e;
        }

        // The new effect is instantiated
        switch (currentEffect) {
            case 0:
                break;
            case 1:
                e = new EffectTest();
                break;
            case 2:
                e = new EffectPerlin();
                break;
        }

        hasEffectChanged = false;
    }
    
    // And gets rendered
    if (e != nullptr){
        e->render();
    }
}

/**
 * @brief Renders all the ImGui windows that should be displayed.
 */
void GUI::renderMenus() {
    if (displayGui) {
        // Main config window
        if (displayMainWindow) {
            mainWindow();
        }
        // Effect settings menu
        if (displayEffectSettings) {
            e->effectSettings();
        }
    }
}


/**
 * @brief Checks for keypresses on the main window and modifies the GUI accordingly.
 */
void GUI::checkKeyPresses() {
    ImGuiIO& io = ImGui::GetIO();

    // Listen for G and toggle GUI windows
    if (ImGui::IsKeyPressed(ImGuiKey_G)) {
        std::cout << "Toggling GUI" << std::endl;
        displayGui = !displayGui;
    }

    // Listen for F1 and toggle fullscreen
    if (ImGui::IsKeyPressed(ImGuiKey_F)) {
        std::cout << "Toggling fullscreen" << std::endl;

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
}