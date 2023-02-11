#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string>
#include <cmath>
#include "larp.h"
#include "utils/mat4.h"

// Constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const char *WINDOW_NAME = "Geometry Sample";
const int FPS = 60;

// The window we'll be rendering to
SDL_Window *g_window = NULL;
	
// The window renderer
SDL_Renderer *g_renderer = NULL;

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


void end(void) 
{
	// Destroy window
    SDL_DestroyRenderer(g_renderer);
	SDL_DestroyWindow(g_window);
    g_window = NULL;
    g_renderer = NULL;

	// Quit SDL subsystems
    IMG_Quit();
	SDL_Quit();
}


int main(int argc, char* args[])
{
    // Start up SDL and create window
    if (!init())
    {
        printf("Failed to initialize\n");
    }
    else 
    {
        // matrix test
        mat4 a = mat4 ({1,1,0,0,1,1,0,0,0,0,1,0,0,0,0,1});
        mat4 b = mat4({0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15});
        a * b;



        // Keep the window alive
        SDL_Event e; 
        bool quit = false; 

        //Uint32 last_time = SDL_GetTicks();

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

            //Render red filled quad
            SDL_Rect fillRect = {SCREEN_WIDTH/4, SCREEN_HEIGHT/4, SCREEN_WIDTH/2, SCREEN_HEIGHT/2};
            SDL_SetRenderDrawColor(g_renderer, 0xFF, 0x00, 0x00, 0xFF );		
            SDL_RenderFillRect(g_renderer, &fillRect);

            //Render green outlined quad
            SDL_Rect outlineRect = {SCREEN_WIDTH/6, SCREEN_HEIGHT/6, SCREEN_WIDTH*2/3, SCREEN_HEIGHT*2/3};
            SDL_SetRenderDrawColor(g_renderer, 0x00, 0xFF, 0x00, 0xFF);
            SDL_RenderDrawRect(g_renderer, &outlineRect);
            
            //Draw blue horizontal line
            SDL_SetRenderDrawColor(g_renderer, 0x00, 0x00, 0xFF, 0xFF);
            SDL_RenderDrawLine(g_renderer, 0, SCREEN_HEIGHT/2, SCREEN_WIDTH, SCREEN_HEIGHT/2);

            //Draw vertical line of yellow dots
            SDL_SetRenderDrawColor(g_renderer, 0xFF, 0xFF, 0x00, 0xFF);
            for(int i = 0; i < SCREEN_HEIGHT; i += 4)
            {
                SDL_RenderDrawPoint(g_renderer, SCREEN_WIDTH/2, i);
            }

            //Update screen
            SDL_RenderPresent(g_renderer);

            SDL_Delay(1000/FPS);
            //Uint32 current_time = SDL_GetTicks();
            //Uint32 diff = current_time - last_time;
            //printf("Time: %d\n", diff);
            //last_time = current_time;
        }
	}

    // Free resources and close SDL
    end();

	return 0;
}

