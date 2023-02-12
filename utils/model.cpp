#include "model.h"
#include "vec4.h"
#include "vec3.h"

//=============================================
// Load Model
//=============================================

void Model::Free(void) 
{
    verts.clear();
    faces.clear();
    face_normals.clear();
    verts_normals.clear();
}

bool Model::LoadModel(const char* path) 
{
    if (!path) {
        return false;
    }

    Free();

    // open file
    FILE* fp = fopen(path, "r");
    if (!fp) {
        return false;
    }

    unsigned int numVerts = 0;
    unsigned int numFaces = 0;
    // num of vertices and indices
    fscanf(fp, "data%d%d", &numVerts, &numFaces);

    // alloc vertex and index buffer
    verts.resize(numVerts);
    faces.resize(numFaces);
    face_normals.resize(numFaces);
    verts_normals.resize(numVerts);


    // read vertices
    for (unsigned int i = 0; i < numVerts; i++) {
        fscanf(fp, "%f%f%f", &verts[i].x, &verts[i].y, &verts[i].z);
    }


    // read indices
    for (unsigned int i = 0; i < numFaces; i++) {
        int numSides = 0;
        fscanf(fp, "%i", &numSides);
        faces[i].indices.resize(numSides);

        for (unsigned int k = 0; k < faces[i].indices.size(); k++) {
            fscanf(fp, "%i", &faces[i].indices[k]);
            faces[i].indices[k] -= 1;
        }
    }
    
    // calculate face normals
    for (size_t i = 0; i < numFaces; i++) {
        // get the first 3 verts of a face
        vec3 v0 = verts[faces[i].indices[0]];
        vec3 v1 = verts[faces[i].indices[1]];
        vec3 v2 = verts[faces[i].indices[2]];
        vec3 edge1 = v0 - v1;
        vec3 edge2 = v0 - v2;
        vec3 normal = edge2.cross(edge1);
        face_normals[i] = normal.normalize();
    }

    // calculate verts normals
    for (size_t i = 0; i < numVerts; i++) {
        // get all faces containing this vertex
        std::vector<int> faces_index;
        for (size_t j = 0; j < numFaces; j++) {
            // loop through all indices of the face to check if the vertex is in it
            for (size_t k = 0; k < faces[j].indices.size(); k++) {
                if ((unsigned int)faces[j].indices[k] == i) {
                    faces_index.push_back(j);
                }
            }
        }
        // then average their normals
        vec3 normal_sum(0, 0, 0);
        for (size_t i = 0; i < faces_index.size(); i++) {
            normal_sum += face_normals[faces_index[i]];
        }
        verts_normals[i] = (normal_sum / faces_index.size()).normalize();
    }


    // close file
    fclose(fp);

    ResizeModel();

    return true;
}

//=============================================
// Render Model
//=============================================

void Model::DrawEdges(Camera &camera, SDL_Renderer *renderer) {
    // Apply transformation matrices to get from
    // Model -> World -> Screen 

    // Calculate transformation matrix
    mat4 model_matrix = translate_matrix * rotate_matrix * scale_matrix;
    mat4 view_matrix = camera.GetViewMatrix();
    mat4 perspective_matrix = camera.GetPerspectiveMatrix();
    mat4 model_view_matrix = perspective_matrix * view_matrix * model_matrix;

    for (unsigned int i = 0; i < faces.size(); i++) {
        for (unsigned int k = 0; k < faces[i].indices.size(); k++) {
            int p0 = faces[i].indices[k];
            int p1 = faces[i].indices[(k + 1) % faces[i].indices.size()];

            vec4 h0 = model_view_matrix * vec4(verts[p0], 1.0f);
            vec4 h1 = model_view_matrix * vec4(verts[p1], 1.0f);

            float x1 = 200.0*h0.x+400;
            float x2 = 200.0*h1.x+400;
            float y1 = 200.0*h0.y+300;
            float y2 = 200.0*h1.y+300;
            SDL_RenderDrawLine(renderer,x1,y1,x2,y2);
        }
    }

}

//=============================================
// Resize Model
//=============================================
// scale the model into the range of [ -0.9, 0.9 ]
void Model::ResizeModel(void) 
{
    // bound
    vec3 min, max;
    if (!CalcBound(min, max)) {
        return;
    }

    // max side
    vec3 size = max - min;

    float r = size.x;
    if (size.y > r) {
        r = size.y;
    }
    if (size.z > r) {
        r = size.z;
    }

    if (r < 1e-6f) {
        r = 0;
    }
    else {
        r = 1.0f / r;
    }

    // scale
    for (unsigned int i = 0; i < verts.size(); i++) {
        // [0, 1]
        verts[i] = (verts[i] - min) * r;

        // [-1, 1]
        verts[i] = verts[i] * 2.0f - vec3(1.0f, 1.0f, 1.0f);

        // [-0.9, 0.9]
        verts[i] *= 0.9;
    }
}

bool Model::CalcBound(vec3& min, vec3& max) 
{
    if (verts.size() <= 0) {
        return false;
    }

    min = verts[0];
    max = verts[0];

    for (unsigned int i = 1; i < verts.size(); i++) {
        vec3 v = verts[i];

        if (v.x < min.x) {
            min.x = v.x;
        }
        else if (v.x > max.x) {
            max.x = v.x;
        }

        if (v.y < min.y) {
            min.y = v.y;
        }
        else if (v.y > max.y) {
            max.y = v.y;
        }

        if (v.z < min.z) {
            min.z = v.z;
        }
        else if (v.z > max.z) {
            max.z = v.z;
        }
    }

    return true;
}


//=============================================
// Transform Model
//=============================================

void Model::Scale(float scale) 
{
    scale_matrix[0] = scale;
    scale_matrix[5] = scale;
    scale_matrix[10] = scale;
}

void Model::Translate(vec3 offset) 
{
    translate_matrix[12] = offset.x;
    translate_matrix[13] = offset.y;
    translate_matrix[14] = offset.z;
}

void Model::Rotate(float x, float y, float z) 
{
    // apply R = Rz(Ry(Rx))
    rotate_matrix[0] = cos(z) * cos(y);
    rotate_matrix[1] = sin(z) * cos(y);
    rotate_matrix[2] = -sin(y);

    rotate_matrix[4] = cos(z) * sin(y) * sin(x) - sin(z) * cos(x);
    rotate_matrix[5] = sin(z) * sin(y) * sin(x) + cos(z) * cos(x);
    rotate_matrix[6] = cos(y) * sin(x);

    rotate_matrix[8] = cos(z) * sin(y) * cos(x) + sin(z) * sin(x);
    rotate_matrix[9] = sin(z) * sin(y) * cos(x) - cos(z) * sin(x);
    rotate_matrix[10] = cos(y) * cos(x);
}
