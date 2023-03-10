#define _USE_MATH_DEFINES

#include "larp.h"
#include "utils/mat4.h"
#include "utils/vec3.h"
#include "utils/model.h"
#include "utils/camera.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <string>
#include <cmath>

// Constants
const int SCREEN_WIDTH = 800;//640;
const int SCREEN_HEIGHT = 600;//480;
const char *WINDOW_NAME = "Perspective Vector Display System";
const int FPS = 60;

// Globals
SDL_Window *g_window = NULL;        // The window we'll be rendering to
SDL_Renderer *g_renderer = NULL;    // The window renderer

// Scene
Model g_model;
Camera g_camera;    // Camera


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
            // Create renderer for window
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
	SDL_Quit();
}

void initScene()
{
    // Load objects
    g_model = Model();
    // g_model.LoadModel("assets/dfiles/biplane.d");
    // g_model.LoadModel("assets/dfiles/house.d");
    g_model.LoadModel("assets/dfiles/nteapot6.d");

}

void renderScene()
{
    //Clear screen
    SDL_SetRenderDrawColor(g_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(g_renderer);

    // Redraw models
    SDL_SetRenderDrawColor(g_renderer, 0x00, 0x00, 0xFF, 0xFF);
    g_model.DrawEdges(g_camera, g_renderer);

    //Update screen
    SDL_RenderPresent(g_renderer);

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
        // Setup the scene
        initScene();

        // Begin the event loop
        SDL_Event e; 
        bool quit = false; 

        //Uint32 last_time = SDL_GetTicks();
        float increment = 0.0f;

        while (!quit) 
        {
            while (SDL_PollEvent(&e)) 
            {
                if (e.type == SDL_QUIT)
                {
                    quit = true;
                }
            }

            renderScene();

            //g_model.Rotate(increment, 0.0f, 0.0f);
            g_model.Rotate(0.0f, increment, 0.0f);
            increment += 0.01;
            //g_model.Translate(vec3(0.0f, -0.7, 0.0f));
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

