#pragma once

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include <GL/glut.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <omp.h>
#include <stdint.h>
#include <string>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <algorithm>
#include <cstdio>
#include <cstdarg>

#define RED 0
#define GREEN 1
#define BLUE 2
#define ALPHA 3

extern SDL_Window* window;
extern int32_t windowWidth, windowHeight;

enum PrintType {
    INFO,
    WARNING,
    ERROR,
    NUM_PRINT_TYPES
};

// Prints
void print(PrintType t, const char* f, ...);

// Colors
unsigned int channelsToRGBA(unsigned char* c);
void RGBAToChannels(unsigned int p, unsigned char* c);