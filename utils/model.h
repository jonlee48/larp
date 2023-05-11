#pragma once
#include "mat4.h"
#include "vec3.h"
#include "camera.h"
#include "constants.h"
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
    std::vector< vec3 > verts_normals;
    std::vector< vec3 > face_normals;
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

    void DrawFaces(Camera &camera, SDL_Renderer *renderer, double z_buffer[SCREEN_WIDTH][SCREEN_HEIGHT][4]);

    //=============================================
    // scale the model into the range of [ -0.9, 0.9 ]
    void ResizeModel(void);

    bool CalcBound(vec3& min, vec3& max);

    //=============================================
    // Transform Model
    //=============================================

    void Scale(double scale);

    void Translate(vec3 offset);

    void Rotate(double x, double y, double z);
};
