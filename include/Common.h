#pragma once

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include <GL/glew.h>
#include <GL/glut.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <iostream>
#include <string>

#define RED 0
#define GREEN 1
#define BLUE 2
#define ALPHA 3

extern SDL_Window* window;

unsigned int channelsToRGBA(unsigned char* c);
void RGBAToChannels(unsigned int p, unsigned char* c);