#pragma once

/**
 * Init anything before main loop
 * Start up SDL and create new window
 */
bool init(void);


/**
 * Loads media
 */
bool load_media(void);

/**
 * Frees media and shuts down SDL
 */
bool close(void);

