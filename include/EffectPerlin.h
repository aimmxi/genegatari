#pragma once

#include <GL/glew.h>
#include "Effect.h"     // Derived from Effect
#include <iostream>
#include <vector>
#include <GL/glut.h>
#include <algorithm>    // std::min

#define PERLIN_MATRIX_SIZE 512
#define PERM_TABLE_SIZE 256

#define GLITCHY_NOISE 1
#define RGBA 1
#define HSLA 2

#define RED 0
#define GREEN 1
#define BLUE 2
#define ALPHA 3

#define HUE 0
#define SATURATION 1
#define VALUE 2
#define ALPHA 3

/**
 * @class EffectPerlin
 * @brief Controllable 2D Perlin Noise effect.
 *
 * Original implementation by Ken Perlin. Additional modifications by Adrian Biagioli (https://adrianb.io/2014/08/09/perlinnoise.html) (https://gist.github.com/Flafla2/f0260a861be0ebdeef76) and Joseph Gentle (https://github.com/josephg/noisejs). C++ adaptation and Genegatari's port by AIMMXI (https://github.com/aimmxi/genegatari/).
 */

class EffectPerlin : public Effect {
private:
    // Variables
    const std::vector<std::string> COLOR_MODES = { "GLITCHY NOISE", "RGB", "HUE" };
    int p[PERLIN_MATRIX_SIZE];
    GLuint texture; 
    float zStep = 0.0f;             // Current Z coordinate. Gets incremented to simulate animation.

    // Controllable parameters
    float distance = 10.0f;         // "Distance" from the image.
    float animationSpeed = 0.02f;   // How fast zStep should be incremented per frame
    int pixelFactor = 2;            // How pixelated the image should look
    int quantizationFactor = 0;     // Applies quantization. The larger the value the more pronounced the discretization
    bool contrastOverflow = 0;      // Stops the colors from overflowing with high contrast ratios
    float contrastFactor = 1.0f;    // Contrast

    int colorMode = 1;              // Keeps record of the current color mode
    float redStrength = 1.0f;
    float blueStrength = 1.0f;
    float greenStrength = 1.0f;

    // Perlin functions
    float grad(int hash, float x, float y, float z);
    float fade(float t);
    float lerp(float a, float b, float t);
    float perlinNoise(float x, float y, float z);
	int inc(int num);

    // Texture processing and rendering functions
    unsigned int channelsToRGBA(unsigned char* c);
    unsigned int filter(unsigned char p);
    GLuint generateTexture(int sizeX, int sizeY);
    
public:
    // Constructor
    EffectPerlin();
    
    // Destructor
    ~EffectPerlin();

    void render() override;
    void effectSettings() override;
};