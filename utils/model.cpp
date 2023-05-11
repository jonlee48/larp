#include "model.h"
#include "vec4.h"
#include "vec3.h"
#include "utils.h"
#include "constants.h"
#include "edgetable.h"
#include <assert.h>

//=============================================
// Load Model
//=============================================

void Model::Free(void) 
{
    verts.clear();
    faces.clear();
    face_normals.clear();
    face_colors.clear();
    verts_normals.clear();
}

bool Model::LoadModel(const char* path) 
{
    if (!path) {
        printf("Error loading model.\n");
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
    face_colors.resize(numFaces);
    verts_normals.resize(numVerts);


    // read vertices
    for (unsigned int i = 0; i < numVerts; i++) {
        fscanf(fp, "%lf%lf%lf", &verts[i].x, &verts[i].y, &verts[i].z);
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
    
        // vec3 v0 = vec3(v40.x, v40.y, v40.z).normalize();
        // vec3 v1 = vec3(v41.x, v41.y, v41.z).normalize();
        // vec3 v2 = vec3(v42.x, v42.y, v42.z).normalize();
        // vec3 normal = ((v1-v0).cross(v2-v1)).normalize();
    // calculate face normals
    for (size_t i = 0; i < numFaces; i++) {
        // get the first 3 verts of a face
        vec3 v0 = verts[faces[i].indices[0]];
        vec3 v1 = verts[faces[i].indices[1]];
        vec3 v2 = verts[faces[i].indices[2]];
        // vec3 edge1 = v0 - v1;
        // vec3 edge2 = v0 - v0;
        // vec3 normal = edge2.cross(edge1);
        vec3 edge1 = v0 - v1;
        vec3 edge2 = v2 - v1;
        vec3 normal = edge2.cross(edge1);
        // vec3 normal = ((v2-v1).cross(v0-v1)).normalize();
        
        face_normals[i] = normal.normalize();
        // Set face to random color
        face_colors[i] = vec3(rand() % 256, rand() % 256, rand() % 256);
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
    
    // For each face in model
    for (unsigned int i = 0; i < faces.size(); i++) {
        // Backface culling 
        vec4 v40 = model_view_matrix * vec4(verts[faces[i].indices[0]], 1.0f);
        vec4 v41 = model_view_matrix * vec4(verts[faces[i].indices[1]], 1.0f);
        vec4 v42 = model_view_matrix * vec4(verts[faces[i].indices[2]], 1.0f);
        vec3 v0 = vec3(v40.x, v40.y, v40.z).normalize();
        vec3 v1 = vec3(v41.x, v41.y, v41.z).normalize();
        vec3 v2 = vec3(v42.x, v42.y, v42.z).normalize();
        vec3 normal = ((v2-v1).cross(v0-v1)).normalize();

        // Don't draw any faces that point away from image plane (positive z)
        if (BACK_FACE_CULLING && normal.z > 0.0)
            continue;

        /*
        // Backface culling (alternative method)
        vec4 v3 = (model_matrix) * vec4(face_normals[i], 1.0f);
        vec3 normalp = vec3(v3.x, v3.y, v3.z).normalize();
        vec4 v4 = (model_matrix) * vec4(verts[faces[i].indices[1]], 1.0f);
        vec3 lineofsight = (vec3(v4.x, v4.y, v4.z) - camera.position).normalize();
        double dot = normalp.dot(lineofsight);

        // Visible if dot of normal and line of sight is positive
        if (BACK_FACE_CULLING && dot < 0.0)
            continue;
        */

        // For each edge in face 
        for (unsigned int k = 0; k < faces[i].indices.size(); k++) {

            // Get perspective transform of edge
            int p0 = faces[i].indices[k];
            int p1 = faces[i].indices[(k + 1) % faces[i].indices.size()];

            vec4 h0 = model_view_matrix * vec4(verts[p0], 1.0f);
            vec4 h1 = model_view_matrix * vec4(verts[p1], 1.0f);
            vec3 v0 = vec3(h0.x, h0.y, h0.z).normalize();
            vec3 v1 = vec3(h1.x, h1.y, h1.z).normalize();

            // Scale normalized coordinates [-1, 1] to device coordinates [SCREEN_WIDTH, SCREEN_HEIGHT]
            double half_width = SCREEN_WIDTH / 2.0;
            double half_height = SCREEN_HEIGHT / 2.0;

            double zoom = 1.0;

            double x0 = zoom * half_width * v0.x + half_width;
            double x1 = zoom * half_width * v1.x + half_width;
            double y0 = zoom * half_height * v0.y + half_height;
            double y1 = zoom * half_height * v1.y + half_height;

            // Round to closest int
            int ix0 = (int)round(x0);
            int ix1 = (int)round(x1);
            int iy0 = (int)round(y0);
            int iy1 = (int)round(y1);

            SDL_RenderDrawLine(renderer, ix0, iy0, ix1, iy1);
        }
    }
}

void Model::DrawFaces(Camera &camera, SDL_Renderer *renderer, double z_buffer[SCREEN_WIDTH][SCREEN_HEIGHT][4]) {
    double minimum_z = 100000.0;
    double maximum_z = 0.0;
    // Apply transformation matrices to get from
    // Model -> World -> Screen 

    // Calculate transformation matrix
    mat4 model_matrix = translate_matrix * rotate_matrix * scale_matrix;
    mat4 view_matrix = camera.GetViewMatrix();
    mat4 perspective_matrix = camera.GetPerspectiveMatrix();
    mat4 model_view_matrix = perspective_matrix * view_matrix * model_matrix;
    
    // For each face in model
    for (unsigned int i = 0; i < faces.size(); i++) {
        // Backface culling 
        vec4 v40 = model_view_matrix * vec4(verts[faces[i].indices[0]], 1.0f);
        vec4 v41 = model_view_matrix * vec4(verts[faces[i].indices[1]], 1.0f);
        vec4 v42 = model_view_matrix * vec4(verts[faces[i].indices[2]], 1.0f);
        vec3 v0 = vec3(v40.x, v40.y, v40.z).normalize();
        vec3 v1 = vec3(v41.x, v41.y, v41.z).normalize();
        vec3 v2 = vec3(v42.x, v42.y, v42.z).normalize();
        vec3 normal = ((v2-v1).cross(v0-v1)).normalize();

        // Don't draw any faces that point away from image plane (positive z)
        if (BACK_FACE_CULLING && normal.z > 0.0)
            continue;

        // Randomize color
        SDL_SetRenderDrawColor(renderer, (Uint8)face_colors[i].x, (Uint8)face_colors[i].y, (Uint8)face_colors[i].z, 0xFF);

        EdgeTable et;
        // For each edge in face 
        for (unsigned int k = 0; k < faces[i].indices.size(); k++) {

            // Get perspective transform of edge
            int p0 = faces[i].indices[k];
            int p1 = faces[i].indices[(k + 1) % faces[i].indices.size()];
            vec4 h0 = model_view_matrix * vec4(verts[p0], 1.0f);
            vec4 h1 = model_view_matrix * vec4(verts[p1], 1.0f);
            vec3 v0 = vec3(h0.x/h0.w, h0.y/h0.w, h0.z/h0.w);//.normalize();
            vec3 v1 = vec3(h1.x/h1.w, h1.y/h1.w, h1.z/h1.w);//.normalize();
            // vec3 v1 = vec3(h1.x, h1.y, h1.z).normalize();

            // Scale normalized coordinates [-1, 1] to device coordinates [SCREEN_WIDTH, SCREEN_HEIGHT]
            double half_width = SCREEN_WIDTH / 2.0;
            double half_height = SCREEN_HEIGHT / 2.0;

            double zoom = 1.0;

            double x0 = zoom * half_width * v0.x + half_width;
            double x1 = zoom * half_width * v1.x + half_width;
            double y0 = zoom * half_height * v0.y + half_height;
            double y1 = zoom * half_height * v1.y + half_height;
            double z0 = v0.z;
            double z1 = v1.z;

            // Round points 0 and 1
            int iy0 = (int)round(y0);
            int iy1 = (int)round(y1);

            double inv_m = (x1 - x0)/(y1 - y0);

            // Add only non-horizontal edges to ET
            if (comparedoubles(iy0, iy1, FLOAT_TOL) != 0) {

                /* Assume convex polygon - don't shorten edges */

                // Add to edge table
                int y_max;      // higher y value
                int y_min;      // lower y value
                double x_min;    // x value at lower edge
                double z_min;    // z value at lower edge
                double del_z;    // rate of change from z_min
                if (iy0 < iy1) {
                    // p0 is lower than p1
                    y_max = iy1;
                    y_min = iy0;
                    x_min = x0;
                    z_min = z0;
                    del_z = (z1-z0)/(y1-y0);
                }
                else {
                    // p1 is lower than p0
                    y_max = iy0;
                    y_min = iy1;
                    x_min = x1;
                    z_min = z1;
                    del_z = (z0-z1)/(y0-y1);
                }

                Edge* e = new Edge(y_max, x_min, inv_m, z_min, del_z);
                et.InsertEdge(y_min,e);
            }
        }
        // printf("ET - FACE %d:\n", i);
        // et.PrintEdgeTable();
        // printf("\n");

        // Create active edge table 
        ActiveEdgeTable aet;
            
        // Start at the first scanline containing an edge
        // Stop when ET and AET are empty
        for (int y = et.scanlines.begin()->first; (!et.IsEmpty() || !aet.IsEmpty()) && y < SCREEN_HEIGHT; y++) {
            // Move edges from ET to AET
            Edge* e;
            while((e = et.RemoveEdge(y)) != nullptr) {
                // printf("Moving edge (y_max=%d x_min=%f 1/m=%f) to AET\n",e->y_max, e->x_min, e->inv_m);
                // AET is keyed by x_int
                int x_int = (int)round(e->x_min);
                aet.InsertEdge(x_int, e);
            }

            // Draw lines between pairs of edges in AET
            // aet.PrintActiveEdgeTable();
            assert((*aet.aet).size() % 2 == 0);
            std::multimap<int,Edge*>::iterator it;
            for (it = (*aet.aet).begin(); it != (*aet.aet).end(); it++) {
                // printf("AET @ scanline %d: \n", scanline);
                int ix0 = it->first;
                Edge *e0 = it->second;
                it++;
                int ix1 = it->first;
                Edge *e1 = it->second;

                // printf("(y_max=%d x_min=%f 1/m=%f) -> (y_max=%d x_min=%f 1/m=%f)\n",cur->y_max, cur->x_min, cur->inv_m, next->y_max, next->x_min, next->inv_m);
                assert(ix0 >= 0 && ix0 < SCREEN_WIDTH);
                assert(ix1 >= 0 && ix1 < SCREEN_WIDTH);

                // Fill in points between and including edges
                double z0 = e0->z_min;
                double z1 = e1->z_min;
                double hor_del_z = (z1 - z0)/(ix1 - ix0);
                double z = z0;

                for (int x = ix0; x <= ix1; x++) {
                    // Only draw point if point is in front of current z value
                    // Or if nothing is there (z buf is 0)
                    double d = z;//(z-0.5)*(z-0.5);
                    if (d < minimum_z)
                        minimum_z = z;
                    if (d > maximum_z)
                        maximum_z = z;
                    // if (d < z_buffer[x][y][3]) {
                    if (comparedoubles(d,z_buffer[x][y][3], FLOAT_TOL) == -1) {
                        z_buffer[x][y][3] = d;
                        // Uint8 c = (Uint8)round(255.0-d*255.0); 
                        // printf("c: %d\n", c);
                        // SDL_SetRenderDrawColor(renderer, c, c, c, 0xFF);
                        SDL_RenderDrawPoint(renderer, x, y);
                    }
                    z += hor_del_z;
                }
            }

            // Update edges
            aet.UpdateEdges(y);
        }
    }
    // printf("min z: %f max z: %f\n", minimum_z, maximum_z);
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

    double r = size.x;
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

void Model::Scale(double scale) 
{
    scale_matrix[0] = scale;
    scale_matrix[5] = scale;
    scale_matrix[10] = scale;
}

void Model::Translate(vec3 offset) 
{
    translate_matrix[3] = offset.x;
    translate_matrix[7] = offset.y;
    translate_matrix[11] = offset.z;
}

void Model::Rotate(double x, double y, double z) 
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
