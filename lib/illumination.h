#pragma once
#include "vec3.h"
#include <SDL2/SDL.h>

class Light {
public:
    vec3 position;
    vec3 color;

public:
    Light();

    Light(vec3 position, vec3 color);

    ~Light() {}

    vec3 LightDirection(vec3 point);
};

class Material {
public:
    vec3 color;
    float k_ambient;
    float k_diffuse;
    float k_specular;
    int shininess;
    SDL_Surface *texture;

public:
    Material();

    Material(vec3 color, float k_ambient, float k_diffuse, float k_specular, int shininess);

    ~Material();

    bool LoadTexture(const char* path);

    vec3 GetTexture(vec3 normal);

    vec3 PhongIllumination(vec3 surface_color, vec3 view, vec3 normal, vec3 light_direction, Light light);
};
