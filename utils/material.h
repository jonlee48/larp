#pragma once
#include "vec3.h"

class Material {
public:
    // Intrinsic material properties
    vec3 color;
    float k_ambient;
    float k_diffuse;
    float k_specular;

    // Extrinsic material properties
    vec3 ambient_color;


public:
    Material(float k_ambient, float k_diffuse, float k_specular);

    ~Material() {};

    vec3 PhongIllumination(vec3 light, vec3 normal, vec3 view);

}