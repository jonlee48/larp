#pragma once
#include "vec3.h"

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

public:
    Material();

    Material(vec3 color, float k_ambient, float k_diffuse, float k_specular, int shininess);

    ~Material() {};

    vec3 PhongIllumination(vec3 view, vec3 normal, vec3 light_direction, Light light);
};
