#pragma once

#include "Effect.h"  // Derived from Effect

/**
 * @class EffectTest
 * @brief Simple effect that just displays a square on the screen.
 */

class EffectTest : public Effect {
private:
    float rotationAngle = 0.0f;
    float rotationSpeed = 0.1f;
public:
    // Constructor
    EffectTest();
    
    // Destructor
    ~EffectTest();

    void render() override;
    void effectSettings() override;
};