#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string>
#include "03_texture.h"

// Constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const char *WINDOW_NAME = "Texture Sample";
const int FPS = 60;

// The window we'll be rendering to
SDL_Window *g_window = NULL;
	
// The window renderer
SDL_Renderer *g_renderer = NULL;

// The image shown on the screen
SDL_Texture *g_texture = NULL;


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
        // Set texture filtering to linear
        if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
        {
            printf("Warning: Linear texture filtering not enabled");
        }

		// Create window
		g_window = SDL_CreateWindow(WINDOW_NAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if(g_window == NULL)
        {
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
            success = false;
		}
		else 
        {
            // Crreate renderer for window
            g_renderer = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED);
            if (g_renderer == NULL)
            {
                printf("Renderer could not be created. SDL Error: %s\n", SDL_GetError());
                success = false;
            }
            else
            {
                // Initialize renderer color
                SDL_SetRenderDrawColor(g_renderer, 0xFF, 0xFF, 0xFF, 0xFF);

                // Initialize PNG loading
                int img_flags = IMG_INIT_PNG;
                if (!(IMG_Init(img_flags) & img_flags))
                {
                    printf("SDL_image could not initialize. SDL_image Error: %s\n", IMG_GetError());
                    success = false;
                }
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
    g_texture = load_texture(splash_image_path);
    if (g_texture == NULL) 
    {
        printf("Failed to load texture image\n");
        success = false;
    }

    return success;
}


void end(void) 
{
    // Free loaded image
    SDL_DestroyTexture(g_texture);
    g_texture = NULL;

	// Destroy window
    SDL_DestroyRenderer(g_renderer);
	SDL_DestroyWindow(g_window);
    g_window = NULL;
    g_renderer = NULL;

	// Quit SDL subsystems
    IMG_Quit();
	SDL_Quit();
}

SDL_Texture *load_texture(std::string path)
{
    // Final texture 
    SDL_Texture *new_texture = NULL;

    // Load image at path
    SDL_Surface *loaded_surface = IMG_Load(path.c_str());
    if (loaded_surface == NULL)
    {
        printf("Unable to load image %s. SDL_image Error: %s\n", path.c_str(), IMG_GetError());
    }
    else
    {
        // Create texture from surface pixels
        new_texture = SDL_CreateTextureFromSurface(g_renderer, loaded_surface);
        if (new_texture == NULL)
        {
            printf("Unable to create texture from %s. SDL Error %s\n", path.c_str(), SDL_GetError());
        }

        // Get rid of old loaded surface
        SDL_FreeSurface(loaded_surface);
    }
    return new_texture;
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

            Uint32 last_time = SDL_GetTicks();

            while (!quit) 
            {
                while (SDL_PollEvent(&e)) 
                {
                    if (e.type == SDL_QUIT)
                    {
                        quit = true;
                    }

                    SDL_RenderClear(g_renderer);
                    SDL_RenderCopy(g_renderer, g_texture, NULL, NULL);
                    SDL_RenderPresent(g_renderer);
                    SDL_Delay(0);            

                }

                SDL_Delay(1000/FPS);
                Uint32 current_time = SDL_GetTicks();
                printf("Time: %d\n", current_time-last_time);
                last_time = current_time;
            }
	    }
	}

    // Free resources and close SDL
    end();

	return 0;
}

