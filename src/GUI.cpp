#include "GUI.h"

SDL_Window* window = nullptr;
int32_t windowWidth, windowHeight;

GUI::GUI() {
    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        fprintf(stderr,"Error: %s\n", SDL_GetError());
    }
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
    gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    // GLEW setuo
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "GLEW initialization error\n");
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

    // The none effect is created
    effect = new EffectNone();
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
        if (ImGui::BeginCombo("Effect", getEffectTypeName(currentEffect))) {
            // For each effect
            for (int e = 0; e < NUM_EFFECT_TYPES; ++e) {
                // A new item in the drop down gets inserted
                if (ImGui::Selectable(getEffectTypeName((EffectType) e), (currentEffect == (EffectType) e))) {
                    // If the widget is selected, the effect gets changed
                    printf("Switching to effect: %s\n", getEffectTypeName((EffectType) e));
                    currentEffect = (EffectType) e;
                    hasEffectChanged = true;
                }
            }
            ImGui::EndCombo();
        }
        
        // A button to open the effect's config is added
        if (ImGui::Button("Toggle effect's settings")) {
            if (effect != nullptr) {
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
void GUI::renderBackground() {
    // Get the current resolution of the main window
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);

    // If a different effect has been picked, it gets instantiated.
    if (hasEffectChanged) {
        printf("Effect changed to %d", currentEffect);
        // If there is an old instance of an effect, it gets deleted before creating a new one
        if (effect != nullptr) {
            delete effect;
        }

        // The new effect is instantiated
        switch (currentEffect) {
            case NONE:
                effect = new EffectNone();
                break;
            case TEST:
                effect = new EffectTest();
                break;
            case PERLIN:
                effect = new EffectPerlin();
                break;
            case CELLAUTOMATON:
                effect = new EffectCellAutomaton();
                break;
            default:
                fprintf(stderr, "Undefined EffectType\n");
                abort();
        }

        hasEffectChanged = false;
    }
    
    // And gets rendered
    if (effect != nullptr){
        effect->render();
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
            effect->effectSettings();
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
        printf("Toggling GUI\n");
        displayGui = !displayGui;
    }

    // Listen for F1 and toggle fullscreen
    if (ImGui::IsKeyPressed(ImGuiKey_F)) {
        printf("Toggling fullscreen\n");

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