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
    #define INITIAL_COLS    25
    #define INITIAL_ROWS    INITIAL_COLS

    // The representation of a cell 
    struct Cell {
        bool        isAlive;
        uint32_t    age;
    };

    // Arrangement of rules and parameters of the simulation
    struct Rules {
        int32_t rows, cols;
        bool    birthCriteria[NUM_ADJ_CELLS + 1];           // + 1 To indicate having no neighbours
        bool    survivalCriteria[NUM_ADJ_CELLS + 1];
    };

    // The matrix of cells
    // Two buffers are kept, representing the current and future generation 
    Cell* buffers[NUM_BUFFERS] = {nullptr, nullptr};
    uint8_t currentBuffer;

    // Rules and settings of the simulation
    Rules ruleset;
    bool randomizeCells = true;

    // Stats
    uint64_t generation;
    uint64_t aliveCells;

    // Rendering variables
    uint8_t* colorMap = nullptr;                // Map representing the colors of each iteration
    GLuint texture = 0;                         // The texture that will get rendered
    bool boardChanged = false;                  // If a change has been made to the settings of the board and regeneration has to be made.
    bool runSimulation = false;                 // Runs the simulation indefinitely

    // Functions
    void rescaleBoard();
    void rescaleTexture();
    bool checkEvolution(int32_t row, int32_t col);
    void stepSimulation();

public:
    // Constructor
    EffectCellAutomaton();
    
    // Destructor
    ~EffectCellAutomaton();

    void render() override;
    void effectSettings() override;
};