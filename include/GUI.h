#pragma once

#include "Common.h"
#include "EffectFactory.h"
#include <vector>

// Constants
// Window
#define NAME "Genegatari"
#define VERSION "(0.1r0)"
#define DESCRIPTION "An interactive playground for graphical effects and visual demos."
#define INITIAL_WIDTH 1280
#define INITIAL_HEIGHT 720

/**
* @class GUI
* @brief Class that handles the main window creation and general settings menu
*/
class GUI {
    private:
        GLuint iconTexture;
        ImVec4 accentColor = {0.012, 0.49, 0.506, 1};
        EffectType currentEffect;
        Effect* effect = nullptr;
        bool displayGui = true;
        bool isFullscreen = false;

        bool renderAboutGenegatari = false;
    
        // FPS counter
        float frameTime = 0.0f;
        unsigned int lastTime = 0;
        unsigned int fps = 0;

        // Windows
        void menuBar();
        void aboutGenegatari();

        // Helper functions
        void centerNextItem(float width);
        void applyAccentColor(const ImVec4 &accent);
        void toggleGUI();
        void toggleFullscreen();

    public:
        SDL_GLContext glContext;
        bool quit = false;

        GUI(EffectType newEffect);
        ~GUI();
        void renderBackground();
        void renderMenus();
        void checkKeyPresses();
};
