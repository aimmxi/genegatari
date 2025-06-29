#pragma once

#include "Common.h"
#include <vector>

// Effects
#include "EffectNone.h"
#include "EffectTest.h"
#include "EffectPerlin.h"

// Constants
// Window
#define NAME "Genegatari"
#define INITIAL_WIDTH 1280
#define INITIAL_HEIGHT 720

/**
 * @class GUI
 * @brief Class that handles the main window creation and general settings menu
 */
class GUI {
private:
    const std::vector<std::string> EFFECTS = { "NONE", "TEST", "PERLIN", "MATRIX", "ORBITS" };  // List of effects

    SDL_GLContext gl_context;
    Effect* e = nullptr;
    bool displayGui = true;
    bool displayMainWindow = true;
    bool displayEffectSettings = false;
    bool hasEffectChanged = false;
    bool isFullscreen = false;
    int currentEffect = 0;

    // FPS counter
    float frameTime = 0.0f;
    unsigned int lastTime = 0;
    unsigned int fps = 0;

    void mainWindow();

public:
    GUI();
    ~GUI();
    void renderBackground();
    void renderMenus();
    void checkKeyPresses();
};
