#define _USE_MATH_DEFINES

#include "larp.h"
#include "lib/mat4.h"
#include "lib/vec3.h"
#include "lib/model.h"
#include "lib/camera.h"
#include "lib/constants.h"
#include "lib/utils.h"
#include "lib/illumination.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <assert.h>
#include <string>
#include <cmath>

// Globals
SDL_Window *g_window = NULL;        // The window we'll be rendering to
SDL_Renderer *g_renderer = NULL;    // The window renderer
float g_buffer[SCREEN_WIDTH][SCREEN_HEIGHT]; // Z buffer

// Scene
Model g_model0;
Camera g_camera;
Light g_light;
Material g_material0;
#ifdef MODEL_1
Model g_model1;
Material g_material1;
#endif


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

    // Initialize PNG loading
    int img_flags = IMG_INIT_PNG;
    if (!(IMG_Init(img_flags) & img_flags)) {
        printf("SDL_image could not initialize. SDL_image Error: %s\n", IMG_GetError());
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
    IMG_Quit();
	SDL_Quit();
}

void initScene()
{
    // Init light
    vec3 light_position = vec3(-30.0, -30.0, -10.0);
    vec3 light_color = vec3(1.0, 1.0, 1.0);
    g_light = Light(light_position, light_color);

    // Init material
    vec3 material_color0 = vec3(1.0, 0.0, 0.0);
    float k_ambient, k_diffuse, k_specular, shininess;
    switch(MATERIAL_TYPE) {
        case METAL:
            k_ambient = 0.45;
            k_diffuse = 0.05;
            k_specular = 0.5;
            shininess = 30;
            break;
        case PLASTIC:
            k_ambient = 0.3;
            k_diffuse = 0.3;
            k_specular = 0.4;
            shininess = 30;
            break;
        case MATTE:
            k_ambient = 0.4;
            k_diffuse = 0.5;
            k_specular = 0.1;
            shininess = 3;
            break;
        case MARBLE:
            k_ambient = 0.4;
            k_diffuse = 0.3;
            k_specular = 0.3;
            shininess = 15;
            break;
    }
    assert((k_ambient + k_diffuse + k_specular) <= 1.0);
    g_material0 = Material(material_color0, k_ambient, k_diffuse, k_specular, shininess);
    if (RENDER_TYPE == TEXTURE || RENDER_TYPE == ENVIRONMENT) {
        if(!g_material0.LoadTexture(TEXTURE_0)) {
            printf("Error loading texture\n");
            exit(1);
        }
    }
    #ifdef MODEL_1
    vec3 material_color1 = vec3(0.0, 0.0, 1.0);
    g_material1 = Material(material_color1, k_ambient, k_diffuse, k_specular, shininess);
    if (RENDER_TYPE == TEXTURE || RENDER_TYPE == ENVIRONMENT) {
        g_material1.LoadTexture(TEXTURE_1);
    }
    #endif

    // Load objects
    g_model0 = Model();
    g_model0.LoadModel(MODEL_0);

    #ifdef MODEL_1
    g_model1 = Model();
    g_model1.LoadModel(MODEL_1);
    #endif

    
}

void renderScene()
{
    //Clear screen
    SDL_SetRenderDrawColor(g_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(g_renderer);

    // Clear the z buffer 
    for (int x = 0; x < SCREEN_WIDTH; x++) {
        for (int y = 0; y < SCREEN_HEIGHT; y++) {
            g_buffer[x][y] = 1.0;
        }
    }

    // Redraw models
    switch (RENDER_TYPE) {
        case WIREFRAME:
            g_model0.DrawEdges(g_camera, g_renderer);
            #ifdef MODEL_1
            g_model1.DrawEdges(g_camera, g_renderer);
            #endif
            break;
        case FACES:
            g_model0.DrawFaces(g_camera, g_renderer, g_buffer, false);
            #ifdef MODEL_1
            g_model1.DrawFaces(g_camera, g_renderer, g_buffer, false);
            #endif
            break;
        case DEPTH:
            g_model0.DrawFaces(g_camera, g_renderer, g_buffer, true);
            #ifdef MODEL_1
            g_model1.DrawFaces(g_camera, g_renderer, g_buffer, true);
            #endif
            break;
        case FLAT:
            g_model0.DrawFlat(g_camera, g_light, g_material0, g_renderer, g_buffer);
            #ifdef MODEL_1
            g_model1.DrawFlat(g_camera, g_light, g_material1, g_renderer, g_buffer);
            #endif
            break;
        case GOURAUD:
            g_model0.DrawGouraud(g_camera, g_light, g_material0, g_renderer, g_buffer);
            #ifdef MODEL_1
            g_model1.DrawGouraud(g_camera, g_light, g_material1, g_renderer, g_buffer);
            #endif
            break;
        case PHONG:
            g_model0.DrawPhong(g_camera, g_light, g_material0, g_renderer, g_buffer, false);
            #ifdef MODEL_1
            g_model1.DrawPhong(g_camera, g_light, g_material1, g_renderer, g_buffer, false);
            #endif
            break;
        case NORMAL:
            g_model0.DrawPhong(g_camera, g_light, g_material0, g_renderer, g_buffer, true);
            #ifdef MODEL_1
            g_model1.DrawPhong(g_camera, g_light, g_material1, g_renderer, g_buffer, true);
            #endif
            break;
        case ENVIRONMENT:
            g_model0.DrawEnvironment(g_camera, g_light, g_material0, g_renderer, g_buffer);
            #ifdef MODEL_1
            g_model1.DrawEnvironment(g_camera, g_light, g_material1, g_renderer, g_buffer);
            #endif
            break;
        case TEXTURE:
            g_model0.DrawTexture(g_camera, g_light, g_material0, g_renderer, g_buffer);
            #ifdef MODEL_1
            g_model1.DrawTexture(g_camera, g_light, g_material1, g_renderer, g_buffer);
            #endif
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

        #ifdef DEBUG
        Uint32 last_time = SDL_GetTicks();
        #endif

        float i = M_PI;     // rotate

        while (!quit) 
        {
            while (SDL_PollEvent(&e)) 
            {
                if (e.type == SDL_QUIT)
                {
                    quit = true;
                }
            }
            vec3 cam_pos = vec3(0.0, 0.0, -40.0);
            g_camera = Camera(cam_pos, vec3());

            if (ANIMATE || framecount == 0) {
                // rotate around Z-axis
                g_model0.Scale(16);
                g_model0.Rotate(0.0, i, M_PI); 
                #ifdef MODEL_1
                g_model0.Translate(vec3(10,0,0));
                #endif

                #ifdef MODEL_1
                g_model1.Scale(16);
                g_model1.Rotate(0.0, -i, M_PI); 
                g_model1.Translate(vec3(-10,0,0));
                #endif
                renderScene();

                i += ROTATION_SPEED;
                framecount++;
            }

            #ifdef FRAMES_PER_SECOND
            SDL_Delay(1000/FRAMES_PER_SECOND);
            #endif

            #ifdef DEBUG 
            Uint32 current_time = SDL_GetTicks();
            Uint32 diff = current_time - last_time;
            printf("Time: %d\n", diff);
            last_time = current_time;
            #endif
        }
	}
    // Free resources and close SDL
    end();
	return 0;
}