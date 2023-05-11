#pragma once

// Screen Settings
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define ASPECT_RATIO SCREEN_WIDTH/SCREEN_HEIGHT
#define WINDOW_NAME "Perspective Vector Display System"
#define FRAMES_PER_SECOND 60
#define ANIMATE 1
#define ROTATION_SPEED 0.05

// Render Settings
// #define RENDER_TYPE WIREFRAME
// #define RENDER_TYPE FACES 
// #define RENDER_TYPE DEPTH 
#define RENDER_TYPE FLAT
// #define RENDER_TYPE GOURAUD
// #define RENDER_TYPE WIREFRAME
#define BACK_FACE_CULLING true 
#define FIELD_OF_VIEW_Y 90;
#define NEAR_CLIPPING_PLANE 1.0;
#define FAR_CLIPPING_PLANE 100.0;
#define FLOAT_TOL 1e-6

// Model
// #define MODEL_PATH "assets/dfiles/shuttle.d"
// #define MODEL_PATH "assets/dfiles/soccer.d"
// #define MODEL_PATH "assets/dfiles/redcar.d"
// #define MODEL_PATH "assets/dfiles/pole.d"
// #define MODEL_PATH "assets/dfiles/jcolumn.d"
// #define MODEL_PATH "assets/dfiles/jbase.d"
// #define MODEL_PATH "assets/dfiles/floor.d"
#define MODEL_PATH "assets/dfiles/cow.d"         // coooow
// #define MODEL_PATH "assets/dfiles/atc.d"         // huuuge model
// #define MODEL_PATH "assets/dfiles/better-ball.d" // very good test
// #define MODEL_PATH "assets/dfiles/house.d"
// #define MODEL_PATH "assets/dfiles/nteapot6.d"    // looks good
// #define MODEL_PATH "assets/dfiles/cylnd.d"       // good sample
// #define MODEL_PATH "assets/dfiles/cube.d"
// #define MODEL_PATH "assets/dfiles/camaro.d" 

enum RenderType {
    WIREFRAME,
    FACES,
    DEPTH,
    FLAT,
    GOURAUD,
    PHONG
};
