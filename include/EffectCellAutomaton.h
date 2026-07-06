#pragma once

#include "Effect.h"  // Derived from Effect
#include <cstddef>


/**
 * @class EffectCellAutomaton
 * @brief Life-Like Cell Automaton simulation
 */
class EffectCellAutomaton : public Effect {
private:
    // Constants
    #define NUM_BUFFERS     2
    #define NUM_ADJ_CELLS   8
    #define INITIAL_COLS    20
    #define INITIAL_ROWS    INITIAL_COLS

    // The representation of a cell 
    struct Cell {
        bool        isAlive;
        uint16_t    age;
    };

    // Arrangement of rules and parameters of the simulation
    struct Rules {
        uint16_t rows, cols;
        bool    birthCriteria[NUM_ADJ_CELLS];
        bool    survivalCriteria[NUM_ADJ_CELLS];
    };

    // The matrix of cells
    // Two buffers are kept, representing the current and future generation 
    Cell* buffers[NUM_BUFFERS] = {nullptr, nullptr};
    uint8_t currentBuffer;

    // Rules of the simulation
    Rules ruleset;

    // Stats
    uint64_t generation;
    uint64_t totalCells;

    // Rendering variables
    uint8_t* colorMap = nullptr;                // Map representing the colors of each iteration
    GLuint texture = 0;                         // The texture that will get rendered


    // Functions
    void rescaleBoard();
    void rescaleTexture();
    void stepSimulation();
    bool isBorn(uint16_t x, uint16_t y);
    bool isSurvivor(uint16_t x, uint16_t y);

public:
    // Constructor
    EffectCellAutomaton();
    
    // Destructor
    ~EffectCellAutomaton();

    void render() override;
    void effectSettings() override;
};