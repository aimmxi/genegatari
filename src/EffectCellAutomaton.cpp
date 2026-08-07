#include "EffectCellAutomaton.h" 

using namespace std;
using namespace std::chrono;

EffectCellAutomaton::EffectCellAutomaton() {
    // Set the initial rules
    settings.rows = INITIAL_ROWS;
    settings.cols = INITIAL_COLS;

    // Start off with Conway's rules
    loadPreset(PRESET_CONWAY);

    // Set the inital state
    generation = 0;
    aliveCells = 0;
    currentBuffer = 0;

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
 * Returns the name of the selected preset
 * @return The name of the preset in std::string format.
 */
string EffectCellAutomaton::getPresetName(Preset p) {
    switch (p) {
        case PRESET_CONWAY:         return "Conway";
        case PRESET_HIGHLIFE:       return "Highlife";
        default:                    return "Error in preset";
    }
}

/**
 * Load the selected preset. 
 * @param p The preset to load.
 */
void EffectCellAutomaton::loadPreset(Preset p) {
    for (int i = 0; i <= NUM_ADJ_CELLS; ++i) {
        criteria.birth[i] = presets[p].birth[i];
        criteria.survival[i] = presets[p].survival[i];
    }
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
    for (int i = (row - 1); i <= (row + 1); ++i) {
        // If the row is out of bounds, jump to the next row.
        if (i < 0 || i >= settings.rows) continue;

        // Iterate over every cell (row and column)
        for (int j = (col - 1); j <= (col + 1); ++j) {
            // If the cell to check is out of bounds or it is the same cell we are trying to check, skip it
            if (j < 0 || j >= settings.cols || (i == row && j == col)) continue;

            // Check if the neighbouring cells are alive
            if (buffers[currentBuffer][i * settings.cols + j].isAlive) neighbours++;
        }
    }

    if (buffers[currentBuffer][row * settings.cols + col].isAlive) {
        // If the cell is alive, check survival against the survival criteria
        return criteria.survival[neighbours];
    } else {
        // If dead, check if it will be born based on the birth criteria
        return criteria.birth[neighbours];
    }
}

/**
 * Iterates over the board and creates a new generation on the new buffer. 
 */
void EffectCellAutomaton::stepSimulation() {
    bool willBeAlive = false;
    uint32_t newAliveCells = 0;

    // Calculate the next buffer
    uint8_t nextBuffer = (currentBuffer + 1) % NUM_BUFFERS;

    // Check for every cell, check how it progresses onto the next generation
    for (int i = 0; i < settings.rows; ++i) {
        for (int j = 0; j < settings.cols; ++j) {
            uint32_t cell = i * settings.cols + j;
            Cell* origCell = &buffers[currentBuffer][cell];
            Cell* destCell = &buffers[nextBuffer][cell];

            // Calculate the state of the cell for the next generation
            willBeAlive = checkEvolution(i, j);

            // Update the state and age
            destCell->isAlive = willBeAlive;

            // If the state is the same as previously, increase the age. Else reset it
            if (origCell->isAlive == willBeAlive)   destCell->age = origCell->isAlive + 1;
            else                                    destCell->age = 0;

            // Regenerate the colormap
            if (destCell->isAlive) {
                colorMap[cell] = 128;
                newAliveCells++;
            } else {
                colorMap[cell] = 0;
            }
        }
    }

    // Update the texture
    glBindTexture(GL_TEXTURE_2D, texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // if using 1-byte-per-pixel data
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, settings.cols, settings.rows, GL_RED, GL_UNSIGNED_BYTE, colorMap);

    // Advance the generation and swap the buffer
    generation++;
    aliveCells = newAliveCells;
    currentBuffer = nextBuffer;

    // Update the finish time of the step
    lastStepTimestamp = steady_clock::now();
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
    buffers[0] = (Cell*) malloc(settings.cols * settings.rows * sizeof(Cell));
    buffers[1] = (Cell*) malloc(settings.cols * settings.rows * sizeof(Cell));
    colorMap = (uint8_t*) malloc(settings.cols * settings.rows * sizeof(uint8_t));
    
    // Init the board
    for (int i = 0; i < settings.rows; ++i) {
        for (int j = 0; j < settings.cols; ++j) {
            uint32_t cell = i * settings.cols + j;

            // If randomization is enabled, set the board with random values
            if (settings.randomizeCells)    buffers[0][cell].isAlive = rand() % 2;
            else                            buffers[0][cell].isAlive = false;

            // Count the cells
            if (buffers[0][cell].isAlive) aliveCells++;

            // Reset the age of each cell
            buffers[0][cell].age = 0;

            // Init the colormap as well
            if (buffers[0][cell].isAlive)   colorMap[cell] = 128;
            else                            colorMap[cell] = 0;
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

    // On OpenGL, (-1, -1) represents the bottom-left corner by default, while on ImGUI and most other places, the origin is the TOP left corner. 
    // This results in the Y axis being mirrored in some situations and requires a lot of weird conversions excusive to the Y axis.
    // Instead of all that nonsense, set the origin to the top left corner to spare future headaches (i have already suffered a few because of this X_X)
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1, 1, 1, -1, -1, 1);

    // Generate and bind the new texture
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Set the texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Prevents textures from looping endlessly, which does not make sense when panning
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    
    // Use the same border color as the default ImGUI background
    GLfloat borderColor[] = {0.102f, 0.102f, 0.102f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    // Map the colormap to the texture that will be rendered
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, settings.cols, settings.rows, 0, GL_RED, GL_UNSIGNED_BYTE, colorMap);
}

// Override
void EffectCellAutomaton::render() {
    // If the board changed, regenerate it 
    if (boardChanged) {
        rescaleBoard();
        rescaleTexture();
        boardChanged = false;
        generation = 0;
    }
    
    // If run is set to true and the pause has elapsed, step the simulation
    if (runSimulation && (steady_clock::now() - lastStepTimestamp >= milliseconds(settings.pauseBetweenSteps))) {
        stepSimulation();
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
    float textureRatio = (float) settings.cols / (float) settings.rows;

    // If the texture is relatively wider than the window.
    if (textureRatio > windowRatio) {
        // Make the texture take up all the width and adjust the height
        quadHeight = windowRatio / textureRatio;
    } else {
        // Otherwise, scale the texture to take up all the vertical space and adapt it's width
        quadWidth = textureRatio / windowRatio;
    }

    // Draw the texture applying zoom and panning
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex2f(APPLY_PAN_ZOOM_X(-quadWidth), APPLY_PAN_ZOOM_Y(-quadHeight));       // Bottom left
    glTexCoord2f(1, 0); glVertex2f(APPLY_PAN_ZOOM_X( quadWidth), APPLY_PAN_ZOOM_Y(-quadHeight));       // Bottom right
    glTexCoord2f(1, 1); glVertex2f(APPLY_PAN_ZOOM_X( quadWidth), APPLY_PAN_ZOOM_Y( quadHeight));       // Top right
    glTexCoord2f(0, 1); glVertex2f(APPLY_PAN_ZOOM_X(-quadWidth), APPLY_PAN_ZOOM_Y( quadHeight));       // Top left
    glEnd();

    // Fetch io
    ImGuiIO& io = ImGui::GetIO();

    // Set a cell to alive or dead if there has been a click on the canvas (not on an ImGUI window)
    // This could be a function but requires much of the previous state; extraction is not practical.
    if ((ImGui::IsMouseDown(ImGuiMouseButton_Left) || ImGui::IsMouseDown(ImGuiMouseButton_Right)) && (!io.WantCaptureMouse)) {
        // Fetch the mouse info
        ImVec2 mouse = ImGui::GetMousePos();
        bool setAlive;

        // Get the bottom left and top right coordinates of the board
        float blX = APPLY_PAN_ZOOM_X(-quadWidth);
        float blY = APPLY_PAN_ZOOM_Y(-quadHeight);
        float trX = APPLY_PAN_ZOOM_X(quadWidth);
        float trY = APPLY_PAN_ZOOM_Y(quadHeight);

        // Calculate how far the click was from the center of the window
        // ImGUI provides the values wrt. the top left corner, so it has to be normalized first to the center of the window.
        // Additionally, the window is always 1:1 but the texture has different aspect ratio. The position must also account that
        float posX = (mouse.x - windowWidth / 2) / (windowWidth / 2);
        float posY = (mouse.y - windowHeight / 2) / (windowHeight / 2); 

        // If the click was within the board
        if (posX >= blX && posX <= trX && posY >= blY && posY <= trY) {
            // Calculate in what point of the board the click happened (in %) and translate to columns and rows 
            float column    = (posX - blX) / (trX - blX) * settings.cols;
            float row       = (posY - blY) / (trY - blY) * settings.rows;
            uint32_t cell   = (uint32_t) row * settings.cols + (uint32_t) column; 

            // Set the state of the cell and regenerate the colormap
            if (ImGui::IsMouseDown(ImGuiMouseButton_Left))  {
                buffers[currentBuffer][cell].isAlive = true;
                colorMap[cell] = 128;
            } else {
                buffers[currentBuffer][cell].isAlive = false;
                colorMap[cell] = 0;
            }

            // Update the texture
            glBindTexture(GL_TEXTURE_2D, texture);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // if using 1-byte-per-pixel data
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, settings.cols, settings.rows, GL_RED, GL_UNSIGNED_BYTE, colorMap);
        }
    }

    // If middle mouse is clicked and there has been some movement, pan the mouse
    if (ImGui::IsMouseDown(ImGuiMouseButton_Middle) && (io.MouseDelta.x != 0.0f || io.MouseDelta.y != 0.0f)) {
        // Delta gives you the difference in pixels, it has to be normalized to a percent first
        // Afterwards, it is linked with the aspect ratio of the texture, adapted to zoom and speed is added (if changed)
        panOffsetX += (io.MouseDelta.x / windowWidth) * (1 / quadWidth) / zoom * panSpeed;
        panOffsetY += (-io.MouseDelta.y / windowHeight) * (1 / quadHeight) / zoom * panSpeed;
    }

    glEnd();
}

// Override
void EffectCellAutomaton::effectSettings() {
    if (ImGui::Begin("Life-Like Cellular Automaton Effect", nullptr, ImGuiWindowFlags_NoCollapse)) {
        ImGui::Text("Life-Like Cellular Automaton Effect");
        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        ImGui::Separator();
        ImGui::Text(" > Controls");
        ImGui::Text("'Middle Mouse' to pan");
        ImGui::Text("'Right Mouse' to set cell to dead");
        ImGui::Text("'Left Mouse' to set cell to alive");

        ImGui::SliderFloat("Zoom", &zoom, 1.0f, 16.0f);
        ImGui::SliderFloat("Pan Speed", &panSpeed, 1.0f, 8.0f);
        ImGui::SliderFloat("Pan X", &panOffsetX, -1.0f, 1.0f);
        ImGui::SliderFloat("Pan Y", &panOffsetY, -1.0f, 1.0f);

        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        ImGui::Separator();
        ImGui::Text(" > Board Settings");
        ImGui::Dummy(ImVec2(0.0f, 10.0f));

        boardChanged = boardChanged || ImGui::InputInt("Board Width", &settings.cols);
        boardChanged = boardChanged || ImGui::InputInt("Board Height", &settings.rows);

        if (ImGui::Button("Fit to resolution")) {
            settings.cols = windowWidth;
            settings.rows = windowHeight;
            boardChanged = true;
        }
        ImGui::Checkbox("Randomize life on board change", &settings.randomizeCells);

        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        ImGui::Separator();
        ImGui::Text(" > Simulation rules");
        ImGui::Dummy(ImVec2(0.0f, 10.0f));

        // Preset loading
        if (ImGui::BeginCombo("Preset", getPresetName((EffectCellAutomaton::Preset) settings.selectedPreset).c_str())) {
            for (int i = 0; i < NUM_PRESETS; ++i) {
                bool isSelected = (settings.selectedPreset == i);

                if (ImGui::Selectable(getPresetName((EffectCellAutomaton::Preset) i).c_str() , isSelected)) {
                    settings.selectedPreset = i;
                    loadPreset((EffectCellAutomaton::Preset) i);
                }
            }
            ImGui::EndCombo();
        }

        ImGui::BeginGroup();
            ImGui::Text("Birth (B)");
            for (int i = 0; i <= NUM_ADJ_CELLS; ++i) {
                ImGui::PushID(i);
                ImGui::Checkbox(to_string(i).c_str(), &criteria.birth[i]);
                ImGui::PopID();
            }
        ImGui::EndGroup();

        ImGui::SameLine(0, 40);

        ImGui::BeginGroup();
            ImGui::Text("Survival (S)");
            for (int i = 0; i <= NUM_ADJ_CELLS; ++i) {
                ImGui::PushID(i + 10);
                ImGui::Checkbox(to_string(i).c_str(), &criteria.survival[i]);
                ImGui::PopID();
            }
        ImGui::EndGroup();

        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        ImGui::Separator();
        ImGui::Text(" > Simulation Control");
        ImGui::Dummy(ImVec2(0.0f, 10.0f));

        ImGui::SliderInt("Pause between steps (ms)", &settings.pauseBetweenSteps, 0, 1000);

        ImGui::Text("Alive Cells: %ld", aliveCells);
        ImGui::Text("Generation: %ld", generation);

        if (ImGui::Button("Single Step")) stepSimulation();
        ImGui::SameLine();
        if (ImGui::Button("Run")) runSimulation = !runSimulation;
        ImGui::SameLine();
        if (ImGui::Button("Reset")) boardChanged = true;

        ImGui::End();
    }
}


