#include "EffectTest.h" 

EffectTest::EffectTest() {
    std::cout << "EffectTest created!" << std::endl;
}

EffectTest::~EffectTest() {
    std::cout << "EffectTest destroyed!" << std::endl;
}

// Override
void EffectTest::render() {
    // Set the background color and clear the previous buffer
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Push the current matrix onto the stack
    glPushMatrix();

    // Apply rotation
    rotationAngle = rotationAngle + rotationSpeed;
    std::cout << "Angle: " << rotationAngle << std::endl;

    glRotatef(rotationAngle, 0.0f, 0.0f, 1.0f);

    // Draw a square (simple OpenGL immediate mode)
    glColor3f(1.0f, 0.0f, 0.0f);  // Color red
    glBegin(GL_QUADS);
        glVertex2f(-0.5f, -0.5f); // Bottom-left corner
        glVertex2f( 0.5f, -0.5f); // Bottom-right corner
        glVertex2f( 0.5f,  0.5f); // Top-right corner
        glVertex2f(-0.5f,  0.5f); // Top-left corner
    glEnd();

    // Pop the matrix to return to the previous state
    glPopMatrix();

    // Swap buffers
    SDL_GL_SwapWindow(SDL_GetWindowFromID(1));
}

// Override
void EffectTest::effectSettings() {
    if (ImGui::Begin("Test Effect", nullptr, ImGuiWindowFlags_NoCollapse)) {
        ImGui::Text("Test");
        ImGui::SliderFloat("Rotation speed", &rotationSpeed, 0.0f, 10.0f);  // Slider that controls the rotation speed
        ImGui::End();
    }
}


