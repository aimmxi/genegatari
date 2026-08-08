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
    #define INITIAL_COLS    50
    #define INITIAL_ROWS    INITIAL_COLS

    // Formulas
    #define APPLY_PAN_ZOOM_X(original) ((original + quadWidth * panOffsetX) * zoom)
    #define APPLY_PAN_ZOOM_Y(original) ((original + quadHeight * panOffsetY) * zoom)

    // Preset types
    enum Preset {
        PRESET_CONWAY,
        PRESET_HIGHLIFE,
        PRESET_DANCE,
        PRESET_BACTERIA,
        PRESET_VOTE,
        PRESET_STAINS,
        PRESET_LIFEGUARD2,
        PRESET_DAYANDNIGHT,
        PRESET_CORAL,
        PRESET_WALLEDCITIES,
        NUM_PRESETS,
    };

    // The representation of a cell 
    struct Cell {
        uint32_t    age;
        bool        isAlive;
    };

    // Criteria of survival/birth
    struct Criteria {
        bool    birth[NUM_ADJ_CELLS + 1];           // + 1 To indicate having no neighbours
        bool    survival[NUM_ADJ_CELLS + 1];
    };

    // Misc simulation settings
    struct Settings {
        int32_t rows, cols;
        int32_t pauseBetweenSteps = 0;                                      // Determines the pause between steps of the simulation to make it appear slower
        uint16_t selectedPreset = 0;                                        // The selected preset
        bool randomizeCells = true;                                         // If cells should be randomized upon regenerating the board
    };

    // Presets
    Criteria presets[NUM_PRESETS] = {
        {   // Conway
            //  0  1  2  3  4  5  6  7  8
            {   0, 0, 0, 1, 0, 0, 0, 0, 0},
            {   0, 0, 1, 1, 0, 0, 0, 0, 0},
        },
        {   // HighLife
            {   0, 0, 0, 1, 0, 0, 1, 0, 0},
            {   0, 0, 1, 1, 0, 0, 0, 0, 0},
        },
        {   // Dance
            {   0, 0, 0, 1, 1, 0, 0, 0, 0},
            {   0, 0, 0, 1, 0, 1, 0, 0, 0},
        },
        {   // Bacteria
            {   0, 0, 0, 1, 1, 0, 0, 0, 0},
            {   0, 0, 0, 0, 1, 1, 1, 0, 0},
        },
        {   // Vote
            {   0, 0, 0, 0, 0, 1, 1, 1, 1},
            {   0, 0, 0, 0, 1, 1, 1, 1, 1},
        },
        {   // Stains
            {   0, 0, 0, 1, 0, 0, 1, 1, 1},
            {   0, 0, 1, 1, 0, 1, 1, 1, 1},
        },
        {   // Lifeguard2 (1 is boring)
            {   0, 0, 0, 1, 0, 0, 0, 0, 0},
            {   0, 0, 0, 0, 1, 1, 1, 1, 0},
        },
        {   // Day and Night
            {   0, 0, 0, 1, 0, 0, 1, 1, 1},
            {   0, 0, 0, 1, 1, 0, 1, 1, 1},
        },
        {   // Coral
            //  0  1  2  3  4  5  6  7  8
            {   0, 0, 0, 1, 0, 0, 0, 0, 0},
            {   0, 0, 0, 0, 1, 1, 1, 1, 1},
        },
        {   // Walled Cities
            //  0  1  2  3  4  5  6  7  8
            {   0, 0, 0, 0, 1, 1, 1, 1, 1},
            {   0, 0, 1, 1, 1, 1, 0, 0, 0},
        }
    };

    // The matrix of cells
    // Two buffers are kept, representing the current and future generation 
    Cell* buffers[NUM_BUFFERS] = {nullptr, nullptr};
    uint8_t currentBuffer;

    // Criteria of survival and settings of the simulator
    Criteria criteria;
    Settings settings;

    // Stats
    uint64_t generation;
    uint64_t aliveCells;

    // Rendering variables
    GLuint texture = 0;                                                 // The texture that will get rendered
    uint8_t* colorMap = nullptr;                                        // Map representing the colors of each iteration
    std::chrono::steady_clock::time_point lastStepTimestamp;            // When the last simulation step happened
    bool boardChanged = false;                                          // If a change has been made to the settings of the board and regeneration has to be made.
    bool runSimulation = false;                                         // Runs the simulation indefinitely
    float zoom = 1.0f;                                                  // Canvas zoom
    float zoomSpeed = 0.5f;                                             // The speed at which scrolling zooms in and out
    float panSpeed = 1.0f;                                              // The speed at which panning moves the board
    float panOffsetX = 0.0f;                                            // How offset is the texture in X and Y
    float panOffsetY = 0.0f;

    // Functions
    std::string getPresetName(Preset p);
    void loadPreset(Preset p);
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