#include "camera.h"
#include "mat4.h"
#include "utils.h"
#include <cmath>

mat4 Camera::GetViewMatrix()
{
    vec3 C = position;
    vec3 N = front;
    vec3 U = right;
    vec3 V = up;
    mat4 R(
        U.x, U.y, U.z, 0.0f,
        V.x, V.y, V.z, 0.0f,
        N.x, N.y, N.z, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
    mat4 T(
        1.0f, 0.0f, 0.0f, -C.x,
        0.0f, 1.0f, 0.0f, -C.y,
        0.0f, 0.0f, 1.0f, -C.z,
        0.0f, 0.0f, 0.0f, 1.0f
    );
    return R*T;
}

mat4 Camera::GetPerspectiveMatrix()
{
    float d = z_near;
    float f = z_far;
    float scale = tan(radians(fov_y/2));
    mat4 pers(
        1/(scale*aspect_ratio), 0.0f, 0.0f, 0.0f,
        0.0f, 1/scale, 0.0f, 0.0f,
        0.0f, 0.0f, f/(f-d), -d*f/(f-d),
        0.0f, 0.0f, 0.0f, 1.0f
    );
    return pers;
}

/*
void Camera::setCamera(const vec3 &look_at, const vec3 &position, float aspect_ratio, float fov_y, float z_near, float z_far) {
        // Set camera position and what it is looking at
        this->look_at = look_at;
        this->position = position;

        this->front = (position - look_at).normalize();
        this->right = (front.cross(world_up)).normalize();
        this->up = (right.cross(front)).normalize();

        this->aspect_ratio = aspect_ratio;
        this->fov_y = fov_y;
        this->z_near = z_near;
        this->z_far = z_far;
    }
*/