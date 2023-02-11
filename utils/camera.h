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
    vec3 right;     // Y-direction of camera (positive right)
    vec3 up;        // Camera up vector
    vec3 world_up;  // Up direction of world
    

    vec3 look_at;     // Reference point to orbit around
    // Euler angles (radians)
    float yaw;
    float pitch;

public:
    Camera() {
        // Set camera position and what it is looking at
        look_at.set(0,0,0);
        position.set(0,1,-1);

        front = position - look_at;



    }

    ~Camera() {
    }

    bool UpdatePosition(float yaw, float pitch);
    bool UpdatePosition(vec3 &position);

    mat4 GetViewMatrix();
};
