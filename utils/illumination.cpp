#include "vec3.h"
#include "illumination.h"
#include "stdio.h"
#include <cmath>

Light::Light() {
    this->position = vec3(0.0, -10.0, 0.0);
    this->ambient_color = vec3(1.0, 1.0, 1.0);
    this->parallel_color = vec3(1.0, 1.0, 1.0);
}

Light::Light(vec3 position, vec3 ambient_color, vec3 parallel_color) {
    this->position = position;
    this->ambient_color = ambient_color;
    this->parallel_color = parallel_color;
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
    vec3 V = view.normalize();
    vec3 N = normal.normalize();
    vec3 L = light_direction.normalize();
    vec3 R = 2 * (N.dot(L)) * N - L;

    vec3 i_ambient = this->k_ambient * light.ambient_color;
    printf("i_ambient: (x: %f\ty: %f\tz: %f)\n", i_ambient.x, i_ambient.y, i_ambient.z);

    vec3 i_diffuse = this->k_diffuse * (N.dot(L)) * light.parallel_color;
    printf("i_diffuse: (x: %f\ty: %f\tz: %f)\n", i_diffuse.x, i_diffuse.y, i_diffuse.z);

    vec3 i_specular = this->k_specular * pow(V.dot(R),this->shininess) * light.parallel_color;
    // printf("i_specular: (x: %f\ty: %f\tz: %f)\n", i_specular.x, i_specular.y, i_specular.z);
    
    vec3 i_total = i_ambient + i_diffuse + i_specular;
    return i_total;
}