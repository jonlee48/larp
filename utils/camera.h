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
    Camera() {
        look_at= vec3();
        position = vec3(0.0f,1.0f,1.0f);
        world_up = vec3(0.0f,1.0f,0.0f);

        front = (position - look_at).normalize();
        right = (front.cross(world_up)).normalize();
        up = (right.cross(front)).normalize();

        aspect_ratio = 800.0/600.0;
        fov_y = 45;
        z_near = -2;
        z_far = -40;

    }

    ~Camera() {
    }


    //void setCamera(const vec3 &look_at, const vec3 &position, float aspect_ratio, float fov_y, float z_near, float z_far);
    // bool UpdateOrientation(float yaw, float pitch);
    // bool UpdateLookAt(vec3 &look_at);
    // bool UpdatePosition(vec3 &position);

    // Returns camera matrix (world to camera transformation)
    mat4 GetViewMatrix();

    // Returns perspective matrix (projects from camera frame to )
    mat4 GetPerspectiveMatrix();
};
