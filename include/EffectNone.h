#pragma once

#include "Effect.h"  // Derived from Effect

/**
 * @class EffectNone
 * @brief Renders nothing.
 */

class EffectNone : public Effect {
public:
    // Constructor
    EffectNone();
    
    // Destructor
    ~EffectNone();

    void render() override;
    void effectSettings() override;
};