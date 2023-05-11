#include "vec3.h"
#include "illumination.h"
#include "stdio.h"
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
}

Material::Material(vec3 color, float k_ambient, float k_diffuse, float k_specular, int shininess) {
    this->color = color;
    this->k_ambient = k_ambient;
    this->k_diffuse = k_diffuse;
    this->k_specular = k_specular;
    this->shininess = shininess;
}

vec3 Material::PhongIllumination(vec3 view, vec3 normal, vec3 light_direction, Light light) {
    // Assume V, N, L are normalized
    vec3 V = view;
    vec3 N = normal;
    vec3 L = light_direction;
    vec3 R = 2 * (N.dot(L)) * N - L;

    vec3 i_ambient = this->k_ambient * this->color;

    vec3 i_diffuse = this->k_diffuse * (N.dot(L)) * this->color;

    vec3 i_specular = vec3();
    if (V.dot(R) > 0)
        i_specular = this->k_specular * pow(V.dot(R),this->shininess) * light.color;
    
    vec3 i_total = i_ambient + i_diffuse + i_specular;
    return i_total;
}