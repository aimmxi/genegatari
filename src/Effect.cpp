#include "Effect.h"

/**
 * @brief Get the name of the EffectType
 * 
 * @param e The effect type 
 * @return const char* The name
 */
const char* getEffectTypeName(EffectType e) {
    switch (e) {
        case NONE:              return "NONE";
        case TEST:              return "TEST";
        case PERLIN:            return "PERLIN";
        case CELLAUTOMATON:     return "CELLAUTOMATON";
        default:
            fprintf(stderr, "Undefined EffectType\n");
            abort();
    }
}