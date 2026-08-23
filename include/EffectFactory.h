#pragma once

// Effects
#include "Effects/EffectNone.h"
#include "Effects/EffectTest.h"
#include "Effects/EffectPerlin.h"
#include "Effects/EffectCellAutomaton.h"

// List of effects
typedef enum {
    NONE,
    TEST,
    PERLIN,
    CELLAUTOMATON,
    NUM_EFFECT_TYPES        // INSERT NEW EFFECTS <<BEFORE>> THIS ONE !!! 
} EffectType;

// Names of the effects
// Stored as a vector instead of a classic array because CLI11 likes it better
const std::vector<std::string> effectNames = {
    "NONE", 
    "TEST", 
    "PERLIN", 
    "CELLAUTOMATON"
};

// Functions
const char* getEffectTypeName(EffectType e);
EffectType getEffectTypeFromName(std::string name);
void switchEffect(Effect*& effect, EffectType type);
