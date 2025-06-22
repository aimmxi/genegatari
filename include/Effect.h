#pragma once

#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include <SDL.h>
#include <SDL_opengl.h>
#include <iostream>

/**
 * @class Effect
 * @brief Abstract class that should be implemented by every effect.
 *
 * Abstract effect class. Both the render and effectSettings methods should be implemented by every effect.
 *     - render(): Handles drawing whatever to the background.
 *     - effectSettings(): Displays some menus that control local effect parameters.
 *
 * This abstract method is generic on purpose. If any further variables or methods are required, they should be private to each effect and only be modified through the effectSettings window. This is done to preserve the modularity of the program and decoupling from the main program.
 *
 * If you wish to implement your own effect, implement this abstract class. (You can copy a simple one like EffectTest.h/EffectTest.cpp and modify that).
 */
class Effect {
public:
    // Virtual destructor to ensure proper cleanup of derived classes
    virtual ~Effect() {}
    virtual void render() = 0;
    virtual void effectSettings() = 0;
};

