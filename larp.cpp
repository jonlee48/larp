#define _USE_MATH_DEFINES

#include "larp.h"
#include "utils/mat4.h"
#include "utils/vec3.h"
#include "utils/model.h"
#include "utils/camera.h"
#include "utils/constants.h"
#include "utils/utils.h"
#include "utils/illumination.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <assert.h>
#include <string>
#include <cmath>

// Globals
SDL_Window *g_window = NULL;        // The window we'll be rendering to
SDL_Renderer *g_renderer = NULL;    // The window renderer
float g_buffer[SCREEN_WIDTH][SCREEN_HEIGHT][4]; // RGB plus Z buffer

// Scene
Model g_model0;
Model g_model1;
Camera g_camera;
Light g_light;
Material g_material0;
Material g_material1;


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

    return true;
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
    // Init light
    vec3 light_position = vec3(0.0, 0.0, 40.0);
    vec3 ambient_color = vec3(0.0, 1.0, 0.0);
    vec3 parallel_color = vec3(1.0, 1.0, 1.0);
    g_light = Light(light_position, ambient_color, parallel_color);

    // Init material
    vec3 material_color = vec3(1.0, 0.0, 0.0);
    float k_ambient = 0.0;
    float k_diffuse = 0.5;
    float k_specular = 0.0;
    float shininess = 50;
    g_material0 = Material(material_color, k_ambient, k_diffuse, k_specular, shininess);
    g_material1 = Material(material_color, k_ambient, k_diffuse, k_specular, shininess);

    // Load objects
    g_model0 = Model();
    g_model0.LoadModel(MODEL_PATH);

    g_model1 = Model();
    g_model1.LoadModel(MODEL_PATH);

    
}

void renderScene()
{
    //Clear screen
    SDL_SetRenderDrawColor(g_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(g_renderer);

    // Clear the z buffer 
    for (int x = 0; x < SCREEN_WIDTH; x++) {
        for (int y = 0; y < SCREEN_HEIGHT; y++) {
            g_buffer[x][y][0] = 0.0;
            g_buffer[x][y][1] = 0.0;
            g_buffer[x][y][2] = 0.0;
            g_buffer[x][y][3] = 1.0;
        }
    }

    // Redraw models
    switch (RENDER_TYPE) {
        case WIREFRAME:
            g_model0.DrawEdges(g_camera, g_renderer);
            g_model1.DrawEdges(g_camera, g_renderer);
            break;
        case FACES:
            g_model0.DrawFaces(g_camera, g_renderer, g_buffer, false);
            g_model1.DrawFaces(g_camera, g_renderer, g_buffer, false);
            break;
        case DEPTH:
            g_model0.DrawFaces(g_camera, g_renderer, g_buffer, true);
            g_model1.DrawFaces(g_camera, g_renderer, g_buffer, true);
            break;
        case FLAT:
            g_model0.DrawFlat(g_camera, g_light, g_material0, g_renderer, g_buffer);
            // g_model1.DrawFlat(g_camera, g_light, g_material1, g_renderer, g_buffer);
            break;
        case GOURAUD:
        case PHONG:
            break;
    }

    // Update screen
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

        float i = 0.0;     // rotate

        while (!quit) 
        {
            while (SDL_PollEvent(&e)) 
            {
                if (e.type == SDL_QUIT)
                {
                    quit = true;
                }
            }
            vec3 cam_pos = vec3(0.0, 0.0, 40.0);
            g_camera = Camera(cam_pos, vec3());

            if (ANIMATE || framecount == 0) {
                // rotate around Z-axis
                g_model0.Scale(16);
                g_model0.Rotate(0.0, i, M_PI); 
                g_model0.Translate(vec3(10,0,0));

                g_model1.Scale(16);
                g_model1.Rotate(0.0, -i, M_PI); 
                g_model1.Translate(vec3(-10,0,0));
                renderScene();

                i += ROTATION_SPEED;
                framecount++;
            }

            #ifdef FRAMES_PER_SECOND
                SDL_Delay(1000/FRAMES_PER_SECOND);
            #endif

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