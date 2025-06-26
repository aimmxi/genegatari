#pragma once

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <iostream>
#include <vector>
#include <string>

// Effects
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


    void mainWindow();

public:
    static SDL_Window* window;
    
    GUI();
    ~GUI();
    void renderBackground();
    void renderMenus();
    void checkKeyPresses();
};
