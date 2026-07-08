#include "EffectCellAutomaton.h" 
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
    ruleset.birthCriteria[2] = true;            // Dead cells with 3 neighbours are brought to life
    ruleset.survivalCriteria[1] = true;         // Cells with 2-3 neighbours are kept alive
    ruleset.survivalCriteria[2] = true;

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
 * Frees and rescales the board when requested.
 */
void EffectCellAutomaton::rescaleBoard() {
    // Allocate memory for the two main buffers
    for (int i = 0; i < NUM_BUFFERS; ++i) {
        // Free the memory if it was preallocated
        if (buffers[i] != nullptr) free(buffers[i]);
        buffers[i] = (Cell*) malloc(ruleset.cols * ruleset.rows * sizeof(Cell));
    }
    
    // Also, resize the color map
    if (colorMap != nullptr) free(colorMap);
    colorMap = (uint8_t*) malloc(ruleset.cols * ruleset.rows);

    // Init the board
    for (int i = 0; i < ruleset.rows; ++i) {
        for (int j = 0; j < ruleset.cols; ++j) {
            // If randomization is enabled, set the board with random values
            if (randomizeCells) buffers[0][i * ruleset.cols + j].isAlive = rand() % 2;
            else                buffers[0][i * ruleset.cols + j].isAlive = false;

            buffers[0][i * ruleset.cols + j].age = 0;

            // Init the colormap as well
            if (buffers[0][i * j].isAlive)  colorMap[i * ruleset.cols + j] = 128;
            else                            colorMap[i * ruleset.cols + j] = 0;
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

    glTexCoord2f(0,0); glVertex2f(-quadWidth, -quadHeight);
    glTexCoord2f(1,0); glVertex2f( quadWidth, -quadHeight);
    glTexCoord2f(1,1); glVertex2f( quadWidth,  quadHeight);
    glTexCoord2f(0,1); glVertex2f(-quadWidth,  quadHeight);

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

        // ImGui::SliderFloat("Distance", &distance, 1.0f, 100.0f);
        // ImGui::SliderFloat("Animation Speed", &animationSpeed, 0.0f, 0.5f);
        // ImGui::SliderInt("Pixel Factor", &pixelFactor, 1, 32);
        // ImGui::SliderInt("Quantization", &quantizationFactor, 1, 128);

        ImGui::End();
    }
}


