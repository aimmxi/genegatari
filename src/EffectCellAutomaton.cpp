#include "EffectCellAutomaton.h" 
#include "imgui.h"
#include <cstdint>

EffectCellAutomaton::EffectCellAutomaton() {
    // Set the initial rules
    ruleset.rows = INITIAL_ROWS;
    ruleset.cols = INITIAL_COLS;

    for (int i = 0; i < NUM_ADJ_CELLS; ++i ) {
        ruleset.birthCriteria[i] = false; 
        ruleset.survivalCriteria[i] = false; 
    }

    // Start off with Conway's rules
    ruleset.birthCriteria[3] = true;            // Dead cells with 3 neighbours are brought to life
    ruleset.survivalCriteria[2] = true;         // Cells with 2-3 neighbours are kept alive
    ruleset.survivalCriteria[3] = true;

    // Set the inital state
    generation = 0;
    aliveCells = 0;

    // Init the random number generator
    srand(time(NULL));

    // Init the board
    rescaleBoard();

    // After generating the colormap, generate the texture
    rescaleTexture();

    printf("EffectCellAutomaton created!\n");
}

EffectCellAutomaton::~EffectCellAutomaton() {
    printf("EffectCellAutomaton destroyed!\n");
}

/**
 * For a given cell, checks if it will be alive on the next generation. Represents the stencil operation of the simulation.
 * 
 * @param row The row of the cell
 * @param col The column of the cell
 * @return True if the next generation will be alive, false if dead.
 */
bool EffectCellAutomaton::checkEvolution(int32_t row, int32_t col) {
    uint8_t neighbours = 0;

    // Iterate over every row
    for (int i =  (row - 1); i <= (row + 1); ++i) {
        // If the row is out of bounds, jump to the next row.
        if (i < 0 || i >= ruleset.rows) continue;

        // Iterate over every cell (row and column)
        for (int j = (col - 1); j <= (col + 1); ++j) {
            // If the cell to check is out of bounds or it is the same cell we are trying to check, skip it
            if (j < 0 || j >= ruleset.cols || (i == row && j == col)) continue;

            // Check if the neighbouring cells are alive
            if (buffers[currentBuffer][i * ruleset.cols + j].isAlive) neighbours++;
        }
    }

    if (buffers[currentBuffer][row * ruleset.cols + col].isAlive) {
        // If the cell is alive, check survival against the survival criteria
        return ruleset.survivalCriteria[neighbours];
    } else {
        // If dead, check if it will be born based on the birth criteria
        return ruleset.birthCriteria[neighbours];
    }
}

/**
 * Iterates over the board and creates a new generation on the new buffer. 
 */
void EffectCellAutomaton::stepSimulation() {
    Cell* origCell;
    Cell* destCell;             
    bool willBeAlive = false;
    uint32_t newAliveCells = 0;

    // Calculate the next buffer
    uint8_t nextBuffer = (currentBuffer + 1) % NUM_BUFFERS;

    // Check for every cell, check how it progresses onto the next generation
    for (int i = 0; i < ruleset.rows; ++i) {
        for (int j = 0; j < ruleset.cols; ++j) {
            origCell = &buffers[currentBuffer][i * ruleset.cols + j];
            destCell = &buffers[nextBuffer][i * ruleset.cols + j];

            // Calculate the state of the cell for the next generation
            willBeAlive = checkEvolution(i, j);

            // Update the state and age
            destCell->isAlive = willBeAlive;

            // If the state is the same as previously, increase the age. Else reset it
            if (origCell->isAlive == willBeAlive)   destCell->age = origCell->isAlive + 1;
            else                                    destCell->age = 0;

            // Regenerate the colormap
            if (destCell->isAlive) {
                colorMap[i * ruleset.cols + j] = 128;
                newAliveCells++;
            } else {
                colorMap[i * ruleset.cols + j] = 0;
            }
        }
    }

    // Update the texture
    glBindTexture(GL_TEXTURE_2D, texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // if using 1-byte-per-pixel data
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, ruleset.cols, ruleset.rows, GL_RED, GL_UNSIGNED_BYTE, colorMap);

    // Advance the generation and swap the buffer
    generation++;
    aliveCells = newAliveCells;
    currentBuffer = nextBuffer;
}

/**
 * Frees and rescales the board when requested.
 */
void EffectCellAutomaton::rescaleBoard() {
    // Free the memory if it was preallocated
    if (buffers[0] != nullptr) free(buffers[0]);
    if (buffers[1] != nullptr) free(buffers[1]);
    if (colorMap != nullptr) free(colorMap);

    // Allocate memory for the two main buffers and colormap
    buffers[0] = (Cell*) malloc(ruleset.cols * ruleset.rows * sizeof(Cell));
    buffers[1] = (Cell*) malloc(ruleset.cols * ruleset.rows * sizeof(Cell));
    colorMap = (uint8_t*) malloc(ruleset.cols * ruleset.rows * sizeof(uint8_t));
    
    // Init the board
    for (int i = 0; i < ruleset.rows; ++i) {
        for (int j = 0; j < ruleset.cols; ++j) {
            // If randomization is enabled, set the board with random values
            if (randomizeCells) buffers[0][i * ruleset.cols + j].isAlive = rand() % 2;
            else                buffers[0][i * ruleset.cols + j].isAlive = false;

            // Count the cells
            if (buffers[0][i * ruleset.cols + j].isAlive) aliveCells++;

            // Reset the age of each cell
            buffers[0][i * ruleset.cols + j].age = 0;

            // Init the colormap as well
            if (buffers[0][i * ruleset.cols + j].isAlive)   colorMap[i * ruleset.cols + j] = 128;
            else                                            colorMap[i * ruleset.cols + j] = 0;
        }
    }
}

/**
 * Frees and rescales the texture when requested.
 */
void EffectCellAutomaton::rescaleTexture() {
    // Free the previous texture
    // The initial value is 0, so this is fine the first time
    glDeleteTextures(1, &texture);

    // Generate and bind the new texture
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Set the texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Map the colormap to the texture that will be rendered
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, ruleset.cols, ruleset.rows, 0, GL_RED, GL_UNSIGNED_BYTE, colorMap);
}

// Override
void EffectCellAutomaton::render() {
    // If the board changed, regenerate it 
    if (boardChanged) {
        rescaleBoard();
        rescaleTexture();
        boardChanged = false;
    }
    
    // If run is set to true, step the simulation
    if (runSimulation) stepSimulation();

    // Clear the previous buffer
    glClear(GL_COLOR_BUFFER_BIT);

    // Create and draw the texture
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Calculate the aspect ratio and map the texture accordingly
    float quadWidth = 1.0f;
    float quadHeight = 1.0f;
    float windowRatio = (float) windowWidth / (float) windowHeight;
    float textureRatio = (float) ruleset.cols / ruleset.rows;

    glBegin(GL_QUADS);

    if (textureRatio > windowRatio) {
        // Texture is relatively wider than the window.
        quadHeight = windowRatio / textureRatio;
    } else {
        // Texture is relatively taller than the window.
        quadWidth = textureRatio / windowRatio;
    }

    glTexCoord2f(0,1); glVertex2f(-quadWidth, -quadHeight);
    glTexCoord2f(1,1); glVertex2f( quadWidth, -quadHeight);
    glTexCoord2f(1,0); glVertex2f( quadWidth,  quadHeight);
    glTexCoord2f(0,0); glVertex2f(-quadWidth,  quadHeight);

    glEnd();
}

// Override
void EffectCellAutomaton::effectSettings() {
    if (ImGui::Begin("Life-Like Cellular Automaton Effect", nullptr, ImGuiWindowFlags_NoCollapse)) {
        ImGui::Text("Life-Like Cellular Automaton Effect");
        
        ImGui::Separator();
        ImGui::Text(" > Basic Settings");
        ImGui::Separator();
        boardChanged = boardChanged || ImGui::InputInt("Board Width", &ruleset.cols);
        boardChanged = boardChanged || ImGui::InputInt("Board Height", &ruleset.rows);
        ImGui::Checkbox("Randomize on board change", &randomizeCells);
        if (ImGui::Button("Fit to resolution")) {
            ruleset.cols = windowWidth;
            ruleset.rows = windowHeight;
            boardChanged = true;
        }

        ImGui::Separator();
        ImGui::Text(" > Simulation");
        ImGui::Separator();
        ImGui::Text("Alive Cells: %ld", aliveCells);
        ImGui::Text("Generation: %ld", generation);

        if (ImGui::Button("Step simulation")) stepSimulation();
        if (ImGui::Button("Run simulation")) runSimulation = !runSimulation;
        

        // ImGui::SliderFloat("Distance", &distance, 1.0f, 100.0f);
        // ImGui::SliderFloat("Animation Speed", &animationSpeed, 0.0f, 0.5f);
        // ImGui::SliderInt("Pixel Factor", &pixelFactor, 1, 32);
        // ImGui::SliderInt("Quantization", &quantizationFactor, 1, 128);

        ImGui::End();
    }
}


