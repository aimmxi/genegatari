#include "EffectFactory.h"

/**
 * @brief Get the name of the EffectType
 * 
 * @param e The effect type 
 * @return const char* The name
 */
const char* getEffectTypeName(EffectType e) {
    uint32_t effectId = (uint32_t) e;

    // Check that the requested effect is within bounds
    if (effectId > (uint32_t) NUM_EFFECT_TYPES) {
        print(ERROR, "Undefined EffectType\n");
        abort();
    }

    return effectNames[effectId].c_str();
}

/**
 * @brief Get the name of the EffectType based on it's name.
 * 
 * @param name The name of the EffectType, as per the effectNames array
 * @return EffectType The EffectType to which it corresponds, NUM_EFFECT_TYPES if there was no match
 */
EffectType getEffectTypeFromName(std::string name) {
    // Iterate over all names and try to find a match
    for (int i = 0; i < NUM_EFFECT_TYPES; ++i) {
        if (name == effectNames[i]) return (EffectType) i;
    }

    return NUM_EFFECT_TYPES;
}


/**
 * @brief Creates a new effect and deletes the old one.
 * @param e The EffectType to switch to 
 */
void switchEffect(Effect*& effect, EffectType type) {
    // If there is an old instance of an effect, it gets deleted before creating a new one
    if (effect != nullptr) delete effect;

    // The new effect is instantiated
    switch (type) {
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
}