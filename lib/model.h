#pragma once
#include "mat4.h"
#include "vec3.h"
#include "camera.h"
#include "constants.h"
#include "illumination.h"
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <cmath>

//================================
// ModelFace
//================================
class ModelFace {
public:
    std::vector< int > indices;

public:
    ModelFace() {
    }

    ~ModelFace() {
    }
};

//================================
// Model
//================================
class Model {
public:
    std::vector< vec3 > verts;
    std::vector< vec3 > model_face_normals;
    std::vector< vec3 > face_colors;
    std::vector< ModelFace > faces;
    mat4 model_matrix;
    mat4 scale_matrix;
    mat4 translate_matrix;
    mat4 rotate_matrix;

public:
    Model() : model_matrix(1), scale_matrix(1), translate_matrix(1), rotate_matrix(1) {
    }

    ~Model() {
    }

    //=============================================
    // Load Model
    //=============================================
    void Free(void);

    bool LoadModel(const char* path);

    //=============================================
    // Render Model
    //=============================================
    void DrawEdges(Camera &camera, SDL_Renderer *renderer);

    void DrawFaces(Camera &camera, SDL_Renderer *renderer, float buffer[SCREEN_WIDTH][SCREEN_HEIGHT], bool render_depth);

    void DrawFlat(Camera &camera, Light &light, Material &material, SDL_Renderer *renderer, float buffer[SCREEN_WIDTH][SCREEN_HEIGHT]);

    void DrawGouraud(Camera &camera, Light &light, Material &material, SDL_Renderer *renderer, float buffer[SCREEN_WIDTH][SCREEN_HEIGHT]);

    void DrawPhong(Camera &camera, Light &light, Material &material, SDL_Renderer *renderer, float buffer[SCREEN_WIDTH][SCREEN_HEIGHT], bool render_normal);

    void DrawEnvironment(Camera &camera, Light &light, Material &material, SDL_Renderer *renderer, float buffer[SCREEN_WIDTH][SCREEN_HEIGHT]);

    void DrawTexture(Camera &camera, Light &light, Material &material, SDL_Renderer *renderer, float buffer[SCREEN_WIDTH][SCREEN_HEIGHT]);

    //=============================================
    // scale the model into the range of [ -0.9, 0.9 ]
    void ResizeModel(void);

    bool CalcBound(vec3& min, vec3& max);

    //=============================================
    // Transform Model
    //=============================================

    void Scale(float scale);

    void Translate(vec3 offset);

    void Rotate(float x, float y, float z);
};
