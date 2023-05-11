#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string>
#include <cmath>
#include "05_surface.h"

// Constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const char *WINDOW_NAME = "Surface Sample";
const int FPS = 60;

// The window we'll be rendering to
SDL_Window *g_window = NULL;
	
// The window renderer
SDL_Renderer *g_renderer = NULL;

// The surface we'll be editing
SDL_Surface *g_surface = NULL;
// SDL_Surface *g_window_surface = NULL;

bool init(void)
{
	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return false;
	}

    // Set texture filtering to linear
    if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
        printf("Warning: Linear texture filtering not enabled");
    }

    // Create window
    g_window = SDL_CreateWindow(WINDOW_NAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (g_window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return false; 
    }

    // Surface - for testing only
    // g_window_surface = SDL_GetWindowSurface(g_window);

    // Create renderer for window
    g_renderer = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED);
    if (g_renderer == NULL) {
        printf("Renderer could not be created. SDL Error: %s\n", SDL_GetError());
        return false;
    }

    // Initialize renderer color
    SDL_SetRenderDrawColor(g_renderer, 0xFF, 0xFF, 0xFF, 0xFF);

    // Initialize PNG loading
    int img_flags = IMG_INIT_PNG;
    if (!(IMG_Init(img_flags) & img_flags)) {
        printf("SDL_image could not initialize. SDL_image Error: %s\n", IMG_GetError());
        return false;
    }

    // Initialize surface
    g_surface = SDL_CreateRGBSurfaceWithFormat(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_PIXELFORMAT_RGBA8888);

    return true;
}


void end(void) 
{
	// Destroy window
    SDL_DestroyRenderer(g_renderer);
	SDL_DestroyWindow(g_window);
    SDL_FreeSurface(g_surface);
    g_window = NULL;
    g_renderer = NULL;
    g_surface = NULL;

	// Quit SDL subsystems
    IMG_Quit();
	SDL_Quit();
}

void SetPixel(SDL_Surface *surface, int x, int y, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    SDL_LockSurface(surface);
    Uint32 *buffer = (Uint32*) surface->pixels;
    int offset = y * SCREEN_WIDTH + x;
    Uint32 pixel = SDL_MapRGBA(g_surface->format, r, g, b, a);
    buffer[offset] = pixel;
    SDL_UnlockSurface(surface);
}

void GetPixel(SDL_Surface *surface, int x, int y, Uint8 *r, Uint8 *g, Uint8 *b, Uint8 *a) {
    SDL_LockSurface(surface);
    Uint32 *buffer = (Uint32*) surface->pixels;
    int offset = y * SCREEN_WIDTH + x;
    Uint32 pixel = buffer[offset];
    SDL_GetRGBA(pixel, g_surface->format, r, g, b, a); 
    SDL_UnlockSurface(surface);
}

int main(int argc, char* args[])
{
    // Start up SDL and create window
    if (!init())
    {
        printf("Failed to initialize\n");
        return -1;
    }

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
        }

        //Clear screen
        SDL_SetRenderDrawColor(g_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderClear(g_renderer);

        //Clear the surface
        SDL_FillRect(g_surface, NULL, SDL_MapRGBA(g_surface->format, 0, 0, 0, 255));

        SetPixel(g_surface, 5, 5, 0, 255, 0, 255);
        SetPixel(g_surface, 6, 5, 255, 0, 0, 255);
        SetPixel(g_surface, 6, 5, 0, 0, 255, 255);
        SetPixel(g_surface, 5, 6, 0, 255, 0, 255);
        SetPixel(g_surface, 6, 6, 0, 255, 0, 255);

        Uint8 r, g, b, a;
        GetPixel(g_surface, 6, 5, &r, &g, &b, &a);
        if (r == 255) SetPixel(g_surface, 6, 5, 0, 255, 0, 255);
        GetPixel(g_surface, 6, 5, &r, &g, &b, &a);
        if (r == 255) SetPixel(g_surface, 6, 5, 0, 0, 255, 255);
        GetPixel(g_surface, 6, 5, &r, &g, &b, &a);
        if (r == 255) SetPixel(g_surface, 6, 5, 0, 0, 255, 255);
        GetPixel(g_surface, 6, 5, &r, &g, &b, &a);
        if (r == 255) SetPixel(g_surface, 6, 5, 0, 0, 255, 255);

        SDL_Texture *texture = SDL_CreateTextureFromSurface(g_renderer, g_surface);

        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_NONE);
        SDL_RenderCopy(g_renderer, texture, NULL, NULL);
        SDL_DestroyTexture(texture);

        //Check a value of surface and write to it
        //Write pixel to screen
        // SDL_BlitSurface(g_surface, 0, g_window_surface, 0);
        // SDL_UpdateWindowSurface(g_window);


        //Render green outlined quad
        SDL_Rect outlineRect = {SCREEN_WIDTH/6, SCREEN_HEIGHT/6, SCREEN_WIDTH*2/3, SCREEN_HEIGHT*2/3};
        SDL_SetRenderDrawColor(g_renderer, 0x00, 0xFF, 0x00, 0xFF);
        SDL_RenderDrawRect(g_renderer, &outlineRect);
        
        /*
        //Draw blue horizontal line
        SDL_SetRenderDrawColor(g_renderer, 0x00, 0x00, 0xFF, 0xFF);
        SDL_RenderDrawLine(g_renderer, 0, SCREEN_HEIGHT/2, SCREEN_WIDTH, SCREEN_HEIGHT/2);

        //Draw vertical line of yellow dots
        SDL_SetRenderDrawColor(g_renderer, 0xFF, 0xFF, 0x00, 0xFF);
        for(int i = 0; i < SCREEN_HEIGHT; i += 4)
        {
            SDL_RenderDrawPoint(g_renderer, SCREEN_WIDTH/2, i);
        }
        */

        //Update screen
        SDL_RenderPresent(g_renderer);

        SDL_Delay(1000/FPS);
        Uint32 current_time = SDL_GetTicks();
        printf("Time: %d\n", current_time-last_time);
        last_time = current_time;
    }

    // Free resources and close SDL
    end();

	return 0;
}

