#pragma once
#include "vec3.h"
#include "mat4.h"
#include "constants.h"

//================================
// Camera
//================================

class Camera {
public:
    
    vec3 position;  // Position of camera (center of projection)
    vec3 normal;    // Viewing direction normal to view plane (Z-axis)
    vec3 right;     // Positive right vector (X-axis)
    vec3 up;        // Camera up vector (Y-axis)
    vec3 world_up;  // Up vector of world
    

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
        vec3 position = vec3(20.0, 0.0, 0.0);
        vec3 look_at = vec3();
        Camera(position, look_at);
    }
    Camera(vec3 position, vec3 look_at) {
        this->position = position;
        this->look_at = look_at;
        world_up = vec3(0.0,1.0,0.0);

        normal = (look_at - position).normalize();
        right = (normal.cross(world_up)).normalize();
        up = (right.cross(normal)).normalize();

        aspect_ratio = ASPECT_RATIO;
        fov_y = FIELD_OF_VIEW_Y;
        z_near = NEAR_CLIPPING_PLANE;
        z_far = FAR_CLIPPING_PLANE;

    }

    ~Camera() {
    }

    // TODO: add these functions to move the camera around
    // void setCamera(const vec3 &look_at, const vec3 &position, float aspect_ratio, float fov_y, float z_near, float z_far);
    // bool UpdateOrientation(float yaw, float pitch);
    // bool UpdateLookAt(vec3 &look_at);
    // bool UpdatePosition(vec3 &position);

    // Returns camera matrix (world to camera transformation)
    mat4 GetViewMatrix();

    // Returns perspective matrix (prospective transformation from camera frame)
    mat4 GetPerspectiveMatrix();
};
