#pragma once
#include "mat4.h"
#include "vec3.h"
#include "camera.h"
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

    void DrawEdges() {
        glPushMatrix();
        // 1.calculate your model matrix by rotation, translation, scale matrices
        mat4 model_matrix = translate_matrix * rotate_matrix * scale_matrix;
        // switch order of translate and rotate to get movements relative to current orientation:
        //mat4 model_matrix = translate_matrix * rotate_matrix * scale_matrix;

        // 2.get the current viewing matrix in the system via glGetFloatv(GL_MODELVIEW_MATRIX, &array)
        mat4 view_matrix(1);
        glGetFloatv(GL_MODELVIEW_MATRIX, view_matrix.mat);
        // 3.calculate your final model-view matrix
        mat4 model_view_matrix = view_matrix * model_matrix;
        // 4.apply your model-view matrix
        glLoadMatrixf(model_view_matrix.mat);

        glBegin(GL_LINES);
        for (unsigned int i = 0; i < faces.size(); i++) {
            for (unsigned int k = 0; k < faces[i].indices.size(); k++) {
                if (i % 2 == 0)
                    glColor4f(0.2f, 0.2f, 0.5f, 1.0f);
                else
                    glColor4f(0.4f, 0.5f, 0.5f, 1.0f);
                int p0 = faces[i].indices[k];
                int p1 = faces[i].indices[(k + 1) % faces[i].indices.size()];
                glVertex3fv(verts[p0].ptr());
                glVertex3fv(verts[p1].ptr());
                
            }
        }
        glEnd();
        glPopMatrix();
    }

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
