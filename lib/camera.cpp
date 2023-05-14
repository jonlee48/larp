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
        U.x, U.y, U.z, 0.0,
        V.x, V.y, V.z, 0.0,
        N.x, N.y, N.z, 0.0,
        0.0, 0.0, 0.0, 1.0
    );
    mat4 T(
        1.0, 0.0, 0.0, -C.x,
        0.0, 1.0, 0.0, -C.y,
        0.0, 0.0, 1.0, -C.z,
        0.0, 0.0, 0.0, 1.0
    );
    return R*T;
}

mat4 Camera::GetPerspectiveMatrix()
{
    float d = z_near;
    float f = z_far;
    float doh = 1.0/tan(radians(fov_y/2.0)); // distance to near clipping plane/height of near clipping plane
    mat4 pers(
        doh/aspect_ratio, 0.0, 0.0, 0.0,
        0.0, doh, 0.0, 0.0,
        0.0, 0.0, f/(f-d), -d*f/(f-d),
        0.0, 0.0, 1.0, 0.0
    );

    return pers; 
}
