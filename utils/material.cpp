#include "material.h"

Material::Material(vec3 color, float k_ambient, float k_diffuse, float k_specular) {
    this->color = color;
    this->k_ambient = k_ambient;
    this->k_diffuse = k_diffuse;
    this->k_specular = k_specular;

    this->ambient_color = ambient_color;
}

vec3 Material::PhongIllumination(vec3 light, vec3 normal, vec3 view) {
    vec3 i_ambient = this->k_ambient*this->ambient_color;
    vec3 i_diffuse = this->k_diffuse*(normal.dot(light))*i_light;
    vec3 i_total = i_ambient + i_diffuse + i_specular;
    return i_total;
}