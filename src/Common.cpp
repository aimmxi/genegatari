#include "Common.h"

SDL_Window* window = nullptr;

/**
 * @brief Converts an array of 4 RGBA bytes to an RGBA pixel.
 * 
 * @param c The array of bytes.
 * @return unsigned int The byte's value repeated on the 4 bytes of an integer.
 */
unsigned int channelsToRGBA(unsigned char* c) {
    // Bit shifts the byte and concatenates the result
    return (c[ALPHA] << 24) | (c[BLUE] << 16) | (c[GREEN] << 8) | c[RED];
}

/**
 * @brief Converts a RGBA pixel to an array of 4 RGBA bytes.
 * 
 * @param p The 32-bit RGBA value.
 * @param c The array to store the bytes in.
 * @return unsigned int The byte's value repeated on the 4 bytes of an integer.
 */
void RGBAToChannels(unsigned int p, unsigned char* c) {
    c[ALPHA] = (p >> 24) & 0xFF;
    c[BLUE]  = (p >> 16) & 0xFF;
    c[GREEN] = (p >> 8)  & 0xFF;
    c[RED]   = p         & 0xFF;
}