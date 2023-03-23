#include "camera.h"
#include "mat4.h"
#include "utils.h"
#include <cmath>

mat4 Camera::GetViewMatrix()
{
    vec3 C = position;
    vec3 N = normal;
    vec3 U = right;
    vec3 V = up;
    mat4 R(
        U.x, U.y, U.z, 0.0f,
        V.x, V.y, V.z, 0.0f,
        N.x, N.y, N.z, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
    // original working
    mat4 T(
        1.0f, 0.0f, 0.0f, -C.x,
        0.0f, 1.0f, 0.0f, -C.y,
        0.0f, 0.0f, 1.0f, -C.z,
        0.0f, 0.0f, 0.0f, 1.0f
    );

    // desperate
    // mat4 T(
    //     1.0f, 0.0f, 0.0f, 0.0f,
    //     0.0f, 1.0f, 0.0f, 0.0f,
    //     0.0f, 0.0f, 1.0f, 0.0f,
    //     -C.x, -C.y, -C.z, 1.0f
    // );

    return R*T;
}

mat4 Camera::GetPerspectiveMatrix()
{
    float d = z_near;
    float f = z_far;
    //float scale = tan(radians(fov_y/2.0));
    float doh = 1.0/tan(radians(fov_y/2.0)); // distance to near clipping plane/height of near clipping plane
    // matches lecture slides
    mat4 pers(
        doh/aspect_ratio, 0.0f, 0.0f, 0.0f,
        0.0f, doh, 0.0f, 0.0f,
        0.0f, 0.0f, f/(f-d), -d*f/(f-d),
        0.0f, 0.0f, 1.0f, 0.0f
    );

    // discussion
    // mat4 pers(
    //     doh, 0.0f, 0.0f, 0.0f,
    //     0.0f, doh, 0.0f, 0.0f,
    //     0.0f, 0.0f, f/(f-d), -2*d*f/(f-d),
    //     0.0f, 0.0f, 1.0f, 0.0f
    // );

    // from tutorial
    float near = 1.0;
    float far = 100.0;

    // mat4 pers(
    //     2.0*near, 0.0f, 0.0f, 0.0f,
    //     0.0f, 2.0*near, 0.0f, 0.0f,
    //     0.0f, 0.0f, -(far+near)/(far-near), 2*far*near/(near-far),
    //     0.0f, 0.0f, -1.0f, 0.0f
    // );
    return pers;
}
