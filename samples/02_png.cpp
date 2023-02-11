#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string>
#include "02_png.h"

// Constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const char *WINDOW_NAME = "PNG Sample";

// The window we'll be rendering to
SDL_Window *g_window = NULL;
	
// The surface contained by the window
SDL_Surface *g_screen_surface = NULL;

// The image shown on the screen
SDL_Surface *g_splash_image = NULL;


bool init(void)
{
    bool success = true;

	// Initialize SDL
	if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        success = false;
	}
	else
    {
		// Create window
		g_window = SDL_CreateWindow(WINDOW_NAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if(g_window == NULL)
        {
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
            success = false;
		}
		else 
        {
            // Initialize PNG loading
            int img_flags = IMG_INIT_PNG;
            if (!(IMG_Init(img_flags) & img_flags))
            {
                printf("SDL_image could not initialize. SDL_image Error: %s\n", IMG_GetError());
                success = false;
            }
            else 
            {
                // Get window surface
                g_screen_surface = SDL_GetWindowSurface(g_window);
            }
        }
    }

    return success;
}


bool load_media(void) 
{
    bool success = true;

    // Load splash image
    std::string splash_image_path = "assets/jl_logo.png";
    g_splash_image = load_surface(splash_image_path);
    if (g_splash_image == NULL) 
    {
        printf("Failed to load PNG image\n");
        success = false;
    }

    return success;
}


void close(void) 
{
    // Deallocate surface
    SDL_FreeSurface(g_splash_image);
    g_splash_image = NULL;

	// Destroy window
	SDL_DestroyWindow(g_window);
    g_window = NULL;

	// Quit SDL subsystems
    IMG_Quit();
	SDL_Quit();
}

SDL_Surface *load_surface(std::string path)
{
    // Final optimized image
    SDL_Surface *optimized_surface = NULL;

    // Load image at path
    SDL_Surface *loaded_surface = IMG_Load(path.c_str());
    if (loaded_surface == NULL)
    {
        printf("Unable to load image %s. SDL_image Error: %s\n", path.c_str(), IMG_GetError());
    }
    else
    {
        // Convert surface to screen format
        optimized_surface = SDL_ConvertSurface(loaded_surface, g_screen_surface->format, 0);
        if (optimized_surface == NULL)
        {
            printf("Unable to optimize image %s. SDL Error %s\n", path.c_str(), SDL_GetError());
        }

        // Get rid of old loaded surface
        SDL_FreeSurface(loaded_surface);
    }
    return optimized_surface;
}


int main(int argc, char* args[])
{
    // Start up SDL and create window
    if (!init())
    {
        printf("Failed to initialize\n");
    }
    else {
        // Load media
        if (!load_media())
        {
            printf("Failed to load media\n");
        }
        else {

            // Keep the window alive
            SDL_Event e; 
            bool quit = false; 

            while (quit == false) 
            {
                while (SDL_PollEvent(&e)) 
                {
                    if (e.type == SDL_QUIT)
                    {
                        quit = true;
                    }
                }

                // Apply the PNG image
                SDL_BlitSurface(g_splash_image, NULL, g_screen_surface, NULL);

                // Update the surface (from back buffer)
                SDL_UpdateWindowSurface(g_window);

            }
	
		
	}
	}

    // Free resources and close SDL
    close();

	return 0;
}

