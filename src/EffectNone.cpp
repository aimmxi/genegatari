#include "EffectNone.h" 

EffectNone::EffectNone() {
}

EffectNone::~EffectNone() {
}

// Override
void EffectNone::render() {
    // Set the background color and clear the previous buffer
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Swap buffers
    SDL_GL_SwapWindow(SDL_GetWindowFromID(1));
}

// Override
void EffectNone::effectSettings() {
}


