#pragma once

#include "Common.h"
#include <vector>

// Effects
#include "Effect.h"
#include "EffectNone.h"
#include "EffectTest.h"
#include "EffectPerlin.h"
#include "EffectCellAutomaton.h"

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
        SDL_GLContext gl_context;
        EffectType currentEffect;
        Effect* effect = nullptr;
        bool displayGui = true;
        bool hasEffectChanged = false;
        bool isFullscreen = false;
    
        // FPS counter
        float frameTime = 0.0f;
        unsigned int lastTime = 0;
        unsigned int fps = 0;
        void menuBar();
    
    public:
        GUI();
        ~GUI();
        void renderBackground();
        void renderMenus();
        void checkKeyPresses();
};
