#pragma once

//================================
// Screen Settings
//================================

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define ASPECT_RATIO SCREEN_WIDTH/SCREEN_HEIGHT
#define WINDOW_NAME "Rendering Pipeline"
// #define FRAMES_PER_SECOND 60
// #define DEBUG 1
#define ANIMATE 1
#define ROTATION_SPEED 0.05

//================================
// Render Style
//================================
// #define RENDER_TYPE WIREFRAME
// #define RENDER_TYPE FACES 
// #define RENDER_TYPE DEPTH 
// #define RENDER_TYPE NORMAL
// #define RENDER_TYPE FLAT
// #define RENDER_TYPE GOURAUD
#define RENDER_TYPE PHONG
// #define RENDER_TYPE ENVIRONMENT
// #define RENDER_TYPE TEXTURE

//================================
// Material Style
//================================
// #define MATERIAL_TYPE METAL
// #define MATERIAL_TYPE PLASTIC
// #define MATERIAL_TYPE MATTE
// #define MATERIAL_TYPE MARBLE
#define MATERIAL_TYPE CARTOON

//================================
// Render Settings
//================================
#define BACK_FACE_CULLING true 
#define FIELD_OF_VIEW_Y 90;
#define NEAR_CLIPPING_PLANE 1.0;
#define FAR_CLIPPING_PLANE 100.0;
#define FLOAT_TOL 1e-6

//================================
// Model 0
//================================
// #define MODEL_0 "assets/dfiles/shuttle.d"
// #define MODEL_0 "assets/dfiles/pole.d"
// #define MODEL_0 "assets/dfiles/jcolumn.d"
// #define MODEL_0 "assets/dfiles/jbase.d"
// #define MODEL_0 "assets/dfiles/floor.d"
// #define MODEL_0 "assets/dfiles/cow.d"         // coooow
// #define MODEL_0 "assets/dfiles/atc.d"         // huuuge model
// #define MODEL_0 "assets/dfiles/better-ball.d" // very good test
// #define MODEL_0 "assets/dfiles/house.d"
#define MODEL_0 "assets/dfiles/nteapot6.d"    // looks good
// #define MODEL_0 "assets/dfiles/cylnd.d"       // good sample
// #define MODEL_0 "assets/dfiles/cube.d"
// #define MODEL_0 "assets/dfiles/camaro.d" 
// #define MODEL_0 "assets/dfiles/bunny.d"       // also large model
// #define TEXTURE_0 "assets/forest.png"         // environment map
// #define TEXTURE_0 "assets/street.png"         // environment map
#define TEXTURE_0 "assets/marble.png"         // texture (looks the best)
// #define TEXTURE_0 "assets/sand.png"           // texture (good on sphere)
// #define TEXTURE_0 "assets/brick.png"          // texture (good on sphere)

//================================
// Model 1 (comment out to remove)
//================================
// #define MODEL_1 "assets/dfiles/shuttle.d"
// #define MODEL_1 "assets/dfiles/pole.d"
// #define MODEL_1 "assets/dfiles/jcolumn.d"
// #define MODEL_1 "assets/dfiles/jbase.d"
// #define MODEL_1 "assets/dfiles/floor.d"
// #define MODEL_1 "assets/dfiles/cow.d"         // coooow
// #define MODEL_1 "assets/dfiles/atc.d"         // huuuge model
// #define MODEL_1 "assets/dfiles/better-ball.d" // very good test
// #define MODEL_1 "assets/dfiles/house.d"
// #define MODEL_1 "assets/dfiles/nteapot6.d"    // looks good
// #define MODEL_1 "assets/dfiles/cylnd.d"       // good sample
// #define MODEL_1 "assets/dfiles/cube.d"
// #define MODEL_1 "assets/dfiles/camaro.d" 
// #define MODEL_1 "assets/dfiles/bunny.d"       // also large model
// #define TEXTURE_1 "assets/forest.png"         // environment map
// #define TEXTURE_1 "assets/street.png"         // environment map
// #define TEXTURE_1 "assets/marble.png"         // texture (best)
// #define TEXTURE_1 "assets/sand.png"           // texture (good on sphere)
#define TEXTURE_1 "assets/brick.png"          // texture (good on sphere)


enum RenderType {
    WIREFRAME,
    FACES,
    DEPTH,
    NORMAL,
    FLAT,
    GOURAUD,
    PHONG,
    TEXTURE,
    ENVIRONMENT,
};

enum MaterialType {
    METAL,
    PLASTIC,
    MATTE,
    MARBLE,
    CARTOON
};