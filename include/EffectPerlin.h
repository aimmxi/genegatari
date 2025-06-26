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
    int pixelFactor = 1;            // How pixelated the image should look

    // Texture calculation functions
    double grad(int hash, double x, double y);
    double fade(double t);
    double lerp(double a, double b, double t);
    double perlinNoise(double x, double y);
	int inc(int num);

    // Texture rendering functions
    GLuint generateTexture(int sizeX, int sizeY);
public:
    // Constructor
    EffectPerlin();
    
    // Destructor
    ~EffectPerlin();

    void render() override;
    void effectSettings() override;
};