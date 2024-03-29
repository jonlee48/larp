#include <SDL2/SDL.h>

int main()
{
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window *window = SDL_CreateWindow("Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1600, 900, 0);

    SDL_Surface *window_surface = SDL_GetWindowSurface(window);

    SDL_Surface *canvas = SDL_CreateRGBSurfaceWithFormat(
        0, 1600, 900, 32, SDL_PIXELFORMAT_RGBA8888
    );

    Uint32 *buffer = (Uint32*) canvas->pixels;
    int row    = 0;
    int column = 0;
    int offset;
    Uint32 color;
    SDL_LockSurface(canvas);
    while (row < 90)
    {
        column = 0;
        while (column < 160)
        {
            offset = row * 1600 + column;
            color = SDL_MapRGBA(canvas->format, 255, 0, 0, 255);
            buffer[offset] = color;
            column++;
        }
        row++;
    }
    SDL_UnlockSurface(canvas);

    int quit = 0;
    SDL_Event event;
    while (!quit)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                quit = 1;
            }
        }

        SDL_BlitSurface(canvas, 0, window_surface, 0);
        SDL_UpdateWindowSurface(window);

        SDL_Delay(10);
    }

    SDL_Quit();

    return 0;
}
