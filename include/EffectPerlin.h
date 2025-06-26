#pragma once

#include <GL/glew.h>
#include "Effect.h"  // Derived from Effect
#include <iostream>
#include <vector>
#include <GL/glut.h>

#define PERLIN_MATRIX_SIZE 512
#define PERM_TABLE_SIZE 256

/**
 * @class EffectPerlin
 * @brief Controllable 2D Perlin Noise effect.
 *
 * Original implementation by Ken Perlin. Additional modifications by Adrian Biagioli (https://adrianb.io/2014/08/09/perlinnoise.html) (https://gist.github.com/Flafla2/f0260a861be0ebdeef76) and Joseph Gentle (https://github.com/josephg/noisejs). C++ adaptation and Genegatari's port by AIMMXI (https://github.com/aimmxi/genegatari/).
 */

class EffectPerlin : public Effect {
private:
    // Variables
    int p[PERLIN_MATRIX_SIZE];
    GLuint texture; 
    float zStep = 0.0f;             // Current Z coordinate. Gets incremented to simulate animation.

    // Controllable parameters
    float distance = 10.0f;         // "Distance" from the image.
    float animationSpeed = 0.02f;   // How fast zStep should be incremented per frame
    int pixelFactor = 2;            // How pixelated the image should look
    int quantizationFactor = 0;     // Applies quantization. The larger the value the more pronounced the discretization

    // Perlin functions
    float grad(int hash, float x, float y, float z);
    float fade(float t);
    float lerp(float a, float b, float t);
    float perlinNoise(float x, float y, float z);
	int inc(int num);

    // Texture processing and rendering functions
    unsigned int byteToRGBA(unsigned char c);
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