#pragma once
#include "vec3.h"
#include "mat4.h"

//================================
// Camera
//================================
class Camera {
public:
    
    vec3 position;  // Center of projection (position of camera)
    vec3 front;     // Viewing direction (normal to view plane)
    vec3 right;     // X-direction of camera (positive right)
    vec3 up;        // Camera up vector
    vec3 world_up;  // Up direction of world
    

    vec3 look_at;   // Reference point to orbit around
    // Euler angles (radians)
    //float yaw;
    //float pitch;

    // Viewing Frustrum
    float aspect_ratio; // Display width/height
    float fov_y;        // Field of View for Y axis in degrees
    float z_near;       // Near clipping plane
    float z_far;        // Far clipping plane

public:
    Camera(const vec3 &look_at, const vec3 &position, float aspect_ratio, float fov_y, float z_near, float z_far) {
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

    ~Camera() {
    }

    //bool UpdateOrientation(float yaw, float pitch);
    //bool UpdateLookAt(vec3 &look_at);
    //bool UpdatePosition(vec3 &position);

    // Returns camera matrix (world to camera transformation)
    mat4 GetViewMatrix();

    // Returns perspective matrix (projects from camera frame to )
    mat4 GetPerspectiveMatrix();
};
