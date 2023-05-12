#define _USE_MATH_DEFINES

#include "vec3.h"
#include "illumination.h"
#include "utils.h"
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <assert.h>
#include <cmath>

Light::Light() {
    this->position = vec3(0.0, 0.0, 40.0);
    this->color = vec3(1.0, 1.0, 1.0);
}

Light::Light(vec3 position, vec3 color) {
    this->position = position;
    this->color = color;
}

vec3 Light::LightDirection(vec3 point) {
    return (position - point).normalize();
}

Material::Material() {
    this->color = vec3(1.0, 0.0, 0.0);
    this->k_ambient = 0.3;
    this->k_diffuse = 0.4;
    this->k_specular = 0.3;
    this->shininess = 20;
    this->texture = NULL;
}

Material::Material(vec3 color, float k_ambient, float k_diffuse, float k_specular, int shininess) {
    this->color = color;
    this->k_ambient = k_ambient;
    this->k_diffuse = k_diffuse;
    this->k_specular = k_specular;
    this->shininess = shininess;
    this->texture = NULL;
}

Material::~Material() {
    SDL_FreeSurface(this->texture);
    this->texture = NULL;
}

bool Material::LoadTexture(const char* path) {
    // Load image at path
    this->texture = IMG_Load(path);
    if (this->texture == NULL) {
        printf("Unable to load image %s. SDL_image Error: %s\n", path, IMG_GetError());
        return false;
    }

    return true;
}

vec3 Material::GetTexture(vec3 normal) {
    // Return a vec3 corresponding to rgb intensity 0 to 1

    // Map normal into latitude longitude

    // Get a longitude wrapping eastward from x-, in the range 0-1.
    float longitude = 0.5 - atan2(normal.z, normal.x) / (2.0 * M_PI);
    // Get a latitude wrapping northward from y-, in the range 0-1.
    float latitude = 0.5 + asin(normal.y) / M_PI;

    // Scale to integer between texture width and height
    int x = (int)round((this->texture->w - 1) * longitude);
    int y = (int)round((this->texture->h - 1) * latitude);

    if (!(x >= 0 && x < this->texture->w)) {
        printf("x: %d\n",x);
    }
    if (!(y >= 0 && y < this->texture->h)) {
        printf("y: %d\n",y);
    }
    assert(x >= 0 && x < this->texture->w);
    assert(y >= 0 && y < this->texture->h);
    
    Uint8 _r, _g, _b, _a;
    GetPixel(this->texture, x, y, &_r, &_g, &_b, &_a);

    // scale between 0 and 1
    float r = (float)_r / 256.0;
    float g = (float)_g / 256.0;
    float b = (float)_b / 256.0;
    // printf("color at (x: %d,y: %d): (r: %f\t g: %f\t b: %f\t)\n", x, y, r, g, b);

    return vec3(r, g, b);
}

vec3 Material::PhongIllumination(vec3 surface_color, vec3 view, vec3 normal, vec3 light_direction, Light light) {
    // Assume light and surface color are between 0 and 1
    // Assume V, N, L are normalized
    vec3 V = view;
    vec3 N = normal;
    vec3 L = light_direction;
    vec3 R = 2 * (N.dot(L)) * N - L;

    vec3 i_ambient = this->k_ambient * surface_color;

    vec3 i_diffuse = this->k_diffuse * (N.dot(L)) * surface_color;

    vec3 i_specular = vec3();
    if (V.dot(R) > 0)
        i_specular = this->k_specular * pow(V.dot(R),this->shininess) * light.color;
    
    vec3 i_total = i_ambient + i_diffuse + i_specular;
    return i_total;
}