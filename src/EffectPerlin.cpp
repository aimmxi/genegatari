#include "EffectPerlin.h" 

// Ken's original permutation table
static const int permutation[PERM_TABLE_SIZE] = {
    151, 160, 137,  91,  90,  15, 131,  13, 201,  95,  96,  53, 194, 233,   7, 225,
    140,  36, 103,  30,  69, 142,   8,  99,  37, 240,  21,  10,  23, 190,   6, 148,
    247, 120, 234,  75,   0,  26, 197,  62,  94, 252, 219, 203, 117,  35,  11,  32,
    57, 177,  33,  88, 237, 149,  56,  87, 174,  20, 125, 136, 171, 168,  68, 175,
    74, 165,  71, 134, 139,  48,  27, 166,  77, 146, 158, 231,  83, 111, 229, 122,
    60, 211, 133, 230, 220, 105,  92,  41,  55,  46, 245,  40, 244, 102, 143,  54,
    65,  25,  63, 161,   1, 216,  80,  73, 209,  76, 132, 187, 208,  89,  18, 169,
    200, 196, 135, 130, 116, 188, 159,  86, 164, 100, 109, 198, 173, 186,   3,  64,
    52, 217, 226, 250, 124, 123,   5, 202,  38, 147, 118, 126, 255,  82,  85, 212,
    207, 206,  59, 227,  47,  16,  58,  17, 182, 189,  28,  42, 223, 183, 170, 213,
    119, 248, 152,   2,  44, 154, 163,  70, 221, 153, 101, 155, 167,  43, 172,   9,
    129,  22,  39, 253,  19,  98, 108, 110,  79, 113, 224, 232, 178, 185, 112, 104,
    218, 246,  97, 228, 251,  34, 242, 193, 238, 210, 144,  12, 191, 179, 162, 241,
    81,  51, 145, 235, 249,  14, 239, 107,  49, 192, 214,  31, 181, 199, 106, 157,
    184,  84, 204, 176, 115, 121,  50,  45, 127,   4, 150, 254, 138, 236, 205,  93,
    222, 114,  67,  29,  24,  72, 243, 141, 128, 195,  78,  66, 215,  61, 156, 180
};

/**
 * @brief Construct a new Effect Perlin:: Effect Perlin object
 */
EffectPerlin::EffectPerlin() {
    // Populates the perlin matrix with the permutation table
	for (int x = 0; x < PERLIN_MATRIX_SIZE; x++) {
		p[x] = permutation[x % PERM_TABLE_SIZE];
	}
}

/**
 * @brief Destroy the Effect Perlin:: Effect Perlin object
 */
EffectPerlin::~EffectPerlin() {}


// Private functions
/**
 * @brief Computes the gradient of the noise at a certain point. The gradient is a vector that indicates direction and magnitude at that point.
 * 
 * @param hash Hash of the two coordinates
 * @param x The X coordinate
 * @param y The Y coordinate
 * @param z The Z coordinate
 * @return double Representation of the gradient at (X,Y).
 */
double EffectPerlin::grad(int hash, double x, double y, double z) {
    switch(hash & 0xF)
    {
        case 0x0: return  x + y;
        case 0x1: return -x + y;
        case 0x2: return  x - y;
        case 0x3: return -x - y;
        case 0x4: return  x + z;
        case 0x5: return -x + z;
        case 0x6: return  x - z;
        case 0x7: return -x - z;
        case 0x8: return  y + z;
        case 0x9: return -y + z;
        case 0xA: return  y - z;
        case 0xB: return -y - z;
        case 0xC: return  y + x;
        case 0xD: return -y + z;
        case 0xE: return  y - x;
        case 0xF: return -y - z;
        default: return 0;
    }
}

/**
 * @brief Applies a smooth ease-in curve to the value. Uses the polynomial 6t^5-15t^4+10t^3.
 * 
 * @param t The parameter to smooth out.
 * @return double The smoothed value
 */
double EffectPerlin::fade(double t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

/**
 * @brief Linear intERPolation function. Interpolates two points smoothy based on another parameter.
 * 
 * @param a The first point to interpolate
 * @param b The second point to interpolate.
 * @param t The weight between A and B
 * @return double The interpolated value between A and B
 */
double EffectPerlin::lerp(double a, double b, double t) {
    return a + t * (b - a);
}

/**
 * @brief Increments the number checking that it doesn't overflow out of PERLIN_MATRIX_SIZE
 * 
 * @param num The number to increment.
 * @return int The incremented number.
 */
int EffectPerlin::inc(int num) {
    num++;
    if (num >= PERLIN_MATRIX_SIZE) { 
        return num % PERLIN_MATRIX_SIZE;
    } 
    return num;
}

/**
 * @brief Calculates the noise factor of a certain coordinate of the grid. The Z axis will be used to animate the noise smoothly.
 * 
 * @param x The X coordinate.
 * @param y The Y coordinate.
 * @param y The Z coordinate.
 * @return double The noise value.
 */
double EffectPerlin::perlinNoise(double x, double y, double z) {
        // Converts x and y coordinates to integers and limit their range to 0-255 
		int xi = (int)x & 255;
		int yi = (int)y & 255;
		int zi = (int)z & 255;

        // Calculates the fractional parts of x and y for interpolation
		double xf = x-(int)x;
		double yf = y-(int)y;
		double zf = z-(int)z;

        // Interpolates the fractional part to get smoother results. This will get used as a weight for the final lerp
		double u = fade(xf);
		double v = fade(yf);
		double w = fade(zf);
															
        // The aab, aba, aab ... are the 8 corners of the cube that surrounds x, y, z
        // They store the hashed index of those 8 corners
		int aaa, aba, aab, abb, baa, bba, bab, bbb;
		aaa = p[p[p[    xi ]+    yi ]+    zi ];
		aba = p[p[p[    xi ]+inc(yi)]+    zi ];
		aab = p[p[p[    xi ]+    yi ]+inc(zi)];
		abb = p[p[p[    xi ]+inc(yi)]+inc(zi)];
		baa = p[p[p[inc(xi)]+    yi ]+    zi ];
		bba = p[p[p[inc(xi)]+inc(yi)]+    zi ];
		bab = p[p[p[inc(xi)]+    yi ]+inc(zi)];
		bbb = p[p[p[inc(xi)]+inc(yi)]+inc(zi)];
	

        // For each coordinate, a new prime value is obtained. 
        // This prime value corresponds to a linear interpolation made of two gradients and the fractional part of x
        // Each gradient is calculated with the corner's hash and the fractional parts of x and y
		double x1, x2, y1, y2;
		x1 = lerp(	grad (aaa, xf  , yf  , zf),
					grad (baa, xf-1, yf  , zf),
					u);
		x2 = lerp(	grad (aba, xf  , yf-1, zf),
					grad (bba, xf-1, yf-1, zf),
			          u);
		y1 = lerp(x1, x2, v);

		x1 = lerp(	grad (aab, xf  , yf  , zf-1),
					grad (bab, xf-1, yf  , zf-1),
					u);
		x2 = lerp(	grad (abb, xf  , yf-1, zf-1),
		          	grad (bbb, xf-1, yf-1, zf-1),
		          	u);
		y2 = lerp (x1, x2, v);
		
		// return (lerp (y1, y2, w)+1)/2;	    // TODO mangle this return value to add some effects
		return (lerp (y1, y2, w));	
}


/**
 * @brief Generates a texture with the perlin noise.
 * 
 * @param width 
 * @param height 
 * @return GLuint 
 */
GLuint EffectPerlin::generateTexture(int width, int height) {
    // An array of "pixels" is created
    std::vector<unsigned char> pixels(width * height);

    // The pixels get filled with the perlin values. If pixelFactor is greater than 1, some gaps will be left.
    // The gaps will get filled afterwards. This also speeds up calculations as less perlin noise is generated
    for (int y = 0; y < height; y += pixelFactor) {
        for (int x = 0; x < width; x += pixelFactor) {
            double nx = x / (double)width, ny = y / (double)height;
            double value = (perlinNoise(nx * distance, ny * distance, zStep) + 1.0) / 2.0 * 255;
            pixels[y * width + x] = static_cast<unsigned char>(value);
        }
    }

    // The gaps get filled if the pixel factor is greater than 1
    if (pixelFactor > 1) {
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                // The gaps are filled with the top-leftmost pixel that has noise
                 pixels[y * width + x] = pixels[(y - (y % pixelFactor)) * width + (x - (x % pixelFactor))];
            }
        }
    }

    // Increment the Z axis step with whatever speed is set
    zStep = zStep + animationSpeed;

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, pixels.data());

    // Set filtering to nearest neighbor (no smoothing)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // // Wrap texture coordinates
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);



    return texture;
}


//Public functions
// Override
void EffectPerlin::render() {
    glEnable(GL_TEXTURE_2D);
    // Create the texture
    texture = generateTexture(1280, 720);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0f); // Disable anisotropic filtering


    // Set the background color and clear the previous buffer
    glClear(GL_COLOR_BUFFER_BIT);

    // Bind the texture to the background
    glBindTexture(GL_TEXTURE_2D, texture);

    // Cover the entire screen with a quadrilateral that takes up the whole window
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex2f( 1.0f, -1.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex2f( 1.0f,  1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f,  1.0f);
    glEnd();

    // Swap buffers
    SDL_GL_SwapWindow(SDL_GetWindowFromID(1));
    SDL_Delay(16);  // ~60 FPS

    // TODO make this fancier
    
}

// Override
void EffectPerlin::effectSettings() {
    if (ImGui::Begin("Perlin Effect", nullptr, ImGuiWindowFlags_NoCollapse)) {
        ImGui::Text("Perlin");
        ImGui::SliderFloat("Distance", &distance, 1.0f, 100.0f);
        ImGui::SliderFloat("Animation Speed", &animationSpeed, 0.0f, 0.5f);
        ImGui::SliderInt("Pixel Factor", &pixelFactor, 1, 32);
        ImGui::End();
    }
}


