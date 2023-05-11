#define _USE_MATH_DEFINES

#include "larp.h"
#include "utils/mat4.h"
#include "utils/vec3.h"
#include "utils/model.h"
#include "utils/camera.h"
#include "utils/constants.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <string>
#include <cmath>

// Globals
SDL_Window *g_window = NULL;        // The window we'll be rendering to
SDL_Renderer *g_renderer = NULL;    // The window renderer
SDL_Surface *g_buffer = NULL;       // Surface used as Z-buffer

// Scene
Model g_model;
Camera g_camera;


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
    if(g_window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    // Create renderer for window
    g_renderer = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED);
    if (g_renderer == NULL) {
        printf("Renderer could not be created. SDL Error: %s\n", SDL_GetError());
        return false;
    }
    
    // Initialize renderer color
    SDL_SetRenderDrawColor(g_renderer, 0xFF, 0xFF, 0xFF, 0xFF);


    // Initialize surface
    g_buffer = SDL_CreateRGBSurfaceWithFormat(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_PIXELFORMAT_RGBA8888);
    if (g_buffer == NULL) {
        printf("Surface could not be created. SDL Error: %s\n", SDL_GetError());
        return false;
    }

    return true;
}

void end(void) 
{
	// Destroy window
    SDL_DestroyRenderer(g_renderer);
	SDL_DestroyWindow(g_window);
	SDL_FreeSurface(g_buffer);
    g_window = NULL;
    g_renderer = NULL;
    g_buffer = NULL;

	// Quit SDL subsystems
	SDL_Quit();
}

void initScene()
{
    // Load objects
    g_model = Model();
    g_model.LoadModel(MODEL_PATH);
}

void renderScene()
{
    //Clear screen
    SDL_SetRenderDrawColor(g_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(g_renderer);

    //Set to blank screen
    SDL_Rect screen_rect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_SetRenderDrawColor(g_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderDrawRect(g_renderer, &screen_rect);

    // Clear the z buffer 
    // Init alpha (z) channel to max value
    SDL_FillRect(g_buffer, NULL, SDL_MapRGBA(g_buffer->format, 0, 0, 0, 255));

    // Redraw models
    // SDL_SetRenderDrawColor(g_renderer, 0x00, 0x00, 0xFF, 0xFF);
    // g_model.DrawEdges(g_camera, g_renderer);
    g_model.DrawFaces(g_camera, g_renderer, g_buffer);

    // Update screen
    SDL_Texture *texture = SDL_CreateTextureFromSurface(g_renderer, g_buffer);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_NONE);
    SDL_RenderCopy(g_renderer, texture, NULL, NULL);
    SDL_DestroyTexture(texture);
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
        int framecount = 0;

        Uint32 last_time = SDL_GetTicks();

        float i = 0.0f;     // rotate

        while (!quit) 
        {
            while (SDL_PollEvent(&e)) 
            {
                if (e.type == SDL_QUIT)
                {
                    quit = true;
                }
            }
            vec3 cam_pos = vec3(4.5, 0, 0);
            g_camera = Camera(cam_pos, vec3());

            if (ANIMATE || framecount == 0) {
                // rotate around Z-axis
                g_model.Rotate(0.0f, i, M_PI); 
                // g_model.Scale(2);

                renderScene();

                i += 0.01;
                framecount++;
            }

            SDL_Delay(1000/FRAMES_PER_SECOND);

            Uint32 current_time = SDL_GetTicks();
            Uint32 diff = current_time - last_time;
            if (ANIMATE)
                printf("Time: %d\n", diff);
            last_time = current_time;
        }
	}
    // Free resources and close SDL
    end();
	return 0;
}