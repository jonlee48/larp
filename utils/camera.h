#pragma once
#include "vec3.h"
#include "mat4.h"

//================================
// Camera
//================================

// Constants
#define ASPECT_RATIO 800.0/600.0;
#define FIELD_OF_VIEW_Y 90;
#define NEAR_CLIPPING_PLANE 0.1;//1.0;//0.1;
#define FAR_CLIPPING_PLANE 1000.0;//100.0; //10.0;

class Camera {
public:
    
    vec3 position;  // Position of camera (center of projection)
    vec3 normal;    // Viewing direction normal to view plane (Z-axis)
    vec3 right;     // Positive right vector (X-axis)
    vec3 up;        // Camera up vector (Y-axis)
    vec3 world_up;  // Up vector of world
    

    vec3 look_at;   // Reference point to orbit around
    // Euler angles (radians)
    //double yaw;
    //double pitch;

    // Viewing Frustrum
    double aspect_ratio; // Display width/height
    double fov_y;        // Field of View for Y axis in degrees
    double z_near;       // Near clipping plane
    double z_far;        // Far clipping plane

public:
    Camera() {
        vec3 position = vec3(20.0f, 0.0f, 0.0f);
        vec3 look_at = vec3();
        Camera(position, look_at);
    }
    Camera(vec3 position, vec3 look_at) {
        this->position = position;
        this->look_at = look_at;
        world_up = vec3(0.0f,1.0f,0.0f);

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
    // void setCamera(const vec3 &look_at, const vec3 &position, double aspect_ratio, double fov_y, double z_near, double z_far);
    // bool UpdateOrientation(double yaw, double pitch);
    // bool UpdateLookAt(vec3 &look_at);
    // bool UpdatePosition(vec3 &position);

    // Returns camera matrix (world to camera transformation)
    mat4 GetViewMatrix();

    // Returns perspective matrix (prospective transformation from camera frame)
    mat4 GetPerspectiveMatrix();
};
