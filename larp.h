#include <SDL2/SDL.h>
#pragma once

/**
 * Start up SDL and create new window
 */
bool init(void);

/**
 * Setup scene
 */
bool initScene(void);


/**
 * Frees media and shuts down SDL
 */
void end(void);

