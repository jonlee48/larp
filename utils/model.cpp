#include "model.h"
#include "vec4.h"
#include "vec3.h"
#include "utils.h"
#include "constants.h"
#include "illumination.h"
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
        vec3 edge2 = v2 - v1;
        vec3 normal = edge2.cross(edge1);
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
    mat4 perspective_transform = perspective_matrix * view_matrix * model_matrix;

    // For each face in model
    for (unsigned int i = 0; i < faces.size(); i++) {
        // Backface culling 
        vec4 _v0 = perspective_transform * vec4(verts[faces[i].indices[0]], 1.0);
        vec4 _v1 = perspective_transform * vec4(verts[faces[i].indices[1]], 1.0);
        vec4 _v2 = perspective_transform * vec4(verts[faces[i].indices[2]], 1.0);
        vec3 v0 = vec3(_v0.x, _v0.y, _v0.z).normalize();
        vec3 v1 = vec3(_v1.x, _v1.y, _v1.z).normalize();
        vec3 v2 = vec3(_v2.x, _v2.y, _v2.z).normalize();
        vec3 normal = ((v2-v1).cross(v0-v1)).normalize();

        // Don't draw any faces that point away from image plane (positive z)
        if (BACK_FACE_CULLING && normal.z > 0.0)
            continue;
        /*
        // Backface culling (alternative method)
        vec4 v3 = (model_matrix) * vec4(face_normals[i], 1.0);
        vec3 normalp = vec3(v3.x, v3.y, v3.z).normalize();
        vec4 v4 = (model_matrix) * vec4(verts[faces[i].indices[1]], 1.0);
        vec3 lineofsight = (vec3(v4.x, v4.y, v4.z) - camera.position).normalize();
        float dot = normalp.dot(lineofsight);

        // Visible if dot of normal and line of sight is positive
        if (BACK_FACE_CULLING && dot < 0.0)
            continue;
        */

        // For each edge in face 
        for (unsigned int k = 0; k < faces[i].indices.size(); k++) {

            // Get perspective transform of edge
            int p0 = faces[i].indices[k];
            int p1 = faces[i].indices[(k + 1) % faces[i].indices.size()];

            vec4 h0 = perspective_transform * vec4(verts[p0], 1.0);
            vec4 h1 = perspective_transform * vec4(verts[p1], 1.0);
            vec3 v0 = vec3(h0.x/h0.w, h0.y/h0.w, h0.z/h0.w);
            vec3 v1 = vec3(h1.x/h1.w, h1.y/h1.w, h1.z/h1.w);

            // Scale normalized coordinates [-1, 1] to device coordinates [SCREEN_WIDTH, SCREEN_HEIGHT]
            float half_width = SCREEN_WIDTH / 2.0;
            float half_height = SCREEN_HEIGHT / 2.0;

            float x0 = half_width * v0.x + half_width;
            float x1 = half_width * v1.x + half_width;
            float y0 = half_height * v0.y + half_height;
            float y1 = half_height * v1.y + half_height;

            // Round to closest int
            int ix0 = (int)round(x0);
            int ix1 = (int)round(x1);
            int iy0 = (int)round(y0);
            int iy1 = (int)round(y1);

            SDL_SetRenderDrawColor(renderer, (Uint8)face_colors[i].x, (Uint8)face_colors[i].y, (Uint8)face_colors[i].z, 0xFF);
            SDL_RenderDrawLine(renderer, ix0, iy0, ix1, iy1);
        }
    }
}

void Model::DrawFaces(Camera &camera, SDL_Renderer *renderer, float buffer[SCREEN_WIDTH][SCREEN_HEIGHT][4], bool render_depth) {
    // Apply transformation matrices to get from
    // Model -> World -> Screen 

    // Calculate transformation matrix
    mat4 model_matrix = translate_matrix * rotate_matrix * scale_matrix;
    mat4 view_matrix = camera.GetViewMatrix();
    mat4 perspective_matrix = camera.GetPerspectiveMatrix();
    mat4 perspective_transform = perspective_matrix * view_matrix * model_matrix;
    
    // For each face in model
    for (unsigned int i = 0; i < faces.size(); i++) {
        // Backface culling 
        vec4 _v0 = perspective_transform * vec4(verts[faces[i].indices[0]], 1.0);
        vec4 _v1 = perspective_transform * vec4(verts[faces[i].indices[1]], 1.0);
        vec4 _v2 = perspective_transform * vec4(verts[faces[i].indices[2]], 1.0);
        vec3 v0 = vec3(_v0.x, _v0.y, _v0.z).normalize();
        vec3 v1 = vec3(_v1.x, _v1.y, _v1.z).normalize();
        vec3 v2 = vec3(_v2.x, _v2.y, _v2.z).normalize();
        vec3 normal = ((v2-v1).cross(v0-v1)).normalize();

        // Don't draw any faces that point away from image plane (positive z)
        if (BACK_FACE_CULLING && normal.z > 0.0)
            continue;

        // Use constant random color
        SDL_SetRenderDrawColor(renderer, (Uint8)face_colors[i].x, (Uint8)face_colors[i].y, (Uint8)face_colors[i].z, 0xFF);

        EdgeTable et;
        // For each edge in face 
        for (unsigned int k = 0; k < faces[i].indices.size(); k++) {

            // Get perspective transform of edge
            int p0 = faces[i].indices[k];
            int p1 = faces[i].indices[(k + 1) % faces[i].indices.size()];
            vec4 h0 = perspective_transform * vec4(verts[p0], 1.0);
            vec4 h1 = perspective_transform * vec4(verts[p1], 1.0);
            vec3 v0 = vec3(h0.x/h0.w, h0.y/h0.w, h0.z/h0.w);
            vec3 v1 = vec3(h1.x/h1.w, h1.y/h1.w, h1.z/h1.w);

            // Scale normalized coordinates [-1, 1] to device coordinates [SCREEN_WIDTH, SCREEN_HEIGHT]
            float half_width = SCREEN_WIDTH / 2.0;
            float half_height = SCREEN_HEIGHT / 2.0;

            float x0 = half_width * v0.x + half_width;
            float x1 = half_width * v1.x + half_width;
            float y0 = half_height * v0.y + half_height;
            float y1 = half_height * v1.y + half_height;
            float z0 = v0.z;
            float z1 = v1.z;

            // Round points 0 and 1
            int iy0 = (int)round(y0);
            int iy1 = (int)round(y1);

            float inv_m = (x1 - x0)/(y1 - y0);

            // Add only non-horizontal edges to ET
            if (comparefloats(iy0, iy1, FLOAT_TOL) != 0) {

                // Assume convex polygon - don't shorten edges

                // Add to edge table
                int y_max;      // higher y value
                int y_min;      // lower y value
                float x_min;    // x value at lower edge
                float z_min;    // z value at lower edge
                float del_z;    // rate of change from z_min
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

        // Create active edge table 
        ActiveEdgeTable aet;
            
        // Start at the first scanline containing an edge
        // Stop when ET and AET are empty
        for (int y = et.scanlines.begin()->first; (!et.IsEmpty() || !aet.IsEmpty()) && y < SCREEN_HEIGHT; y++) {
            // Move edges from ET to AET
            Edge* e;
            while((e = et.RemoveEdge(y)) != nullptr) {
                // AET is keyed by x_int
                int x_int = (int)round(e->x_min);
                aet.InsertEdge(x_int, e);
            }

            // Draw lines between pairs of edges in AET
            assert((*aet.aet).size() % 2 == 0);
            std::multimap<int,Edge*>::iterator it;
            for (it = (*aet.aet).begin(); it != (*aet.aet).end(); it++) {
                int ix0 = it->first;
                Edge *e0 = it->second;
                it++;
                int ix1 = it->first;
                Edge *e1 = it->second;

                assert(ix0 >= 0 && ix0 < SCREEN_WIDTH);
                assert(ix1 >= 0 && ix1 < SCREEN_WIDTH);

                // Fill in points between and including edges
                float z0 = e0->z_min;
                float z1 = e1->z_min;
                float hor_del_z = (z1 - z0)/(ix1 - ix0);
                float z = z0;

                for (int x = ix0; x <= ix1; x++) {
                    // Only draw point if point is in front of current z value
                    if (comparefloats(z, buffer[x][y][3], FLOAT_TOL) == -1) {
                        buffer[x][y][3] = z;

                        // Draw depth map
                        if (render_depth) {
                            Uint8 c = (Uint8)round(255 * ((z - 0.8) / 0.2)); 
                            SDL_SetRenderDrawColor(renderer, c, c, c, 0xFF);
                        }

                        SDL_RenderDrawPoint(renderer, x, y);
                    }
                    z += hor_del_z;
                }
            }

            // Update edges
            aet.UpdateEdges(y);
        }
    }
}

void Model::DrawFlat(Camera &camera, Light &light, Material &material, SDL_Renderer *renderer, float buffer[SCREEN_WIDTH][SCREEN_HEIGHT][4]) {
    // Apply transformation matrices to get from
    // Model -> World -> Screen 

    // Calculate transformation matrix
    mat4 model_matrix = translate_matrix * rotate_matrix * scale_matrix;
    mat4 view_matrix = camera.GetViewMatrix();
    mat4 perspective_matrix = camera.GetPerspectiveMatrix();
    mat4 model_view_matrix = view_matrix * model_matrix;
    mat4 perspective_transform = perspective_matrix * model_view_matrix;
    
    // For each face in model
    for (unsigned int i = 0; i < faces.size(); i++) {
        // Backface culling 
        vec4 _v0 = perspective_transform * vec4(verts[faces[i].indices[0]], 1.0);
        vec4 _v1 = perspective_transform * vec4(verts[faces[i].indices[1]], 1.0);
        vec4 _v2 = perspective_transform * vec4(verts[faces[i].indices[2]], 1.0);
        vec3 v0 = vec3(_v0.x, _v0.y, _v0.z).normalize();
        vec3 v1 = vec3(_v1.x, _v1.y, _v1.z).normalize();
        vec3 v2 = vec3(_v2.x, _v2.y, _v2.z).normalize();
        vec3 normal = ((v2-v1).cross(v0-v1)).normalize();

        // Don't draw any faces that point away from image plane (positive z)
        if (BACK_FACE_CULLING && normal.z > 0.0)
            continue;

        // Calculate surface normal
        _v0 = model_view_matrix * vec4(verts[faces[i].indices[0]], 1.0);
        _v1 = model_view_matrix * vec4(verts[faces[i].indices[1]], 1.0);
        _v2 = model_view_matrix * vec4(verts[faces[i].indices[2]], 1.0);
        v0 = vec3(_v0.x, _v0.y, _v0.z).normalize();
        v1 = vec3(_v1.x, _v1.y, _v1.z).normalize();
        v2 = vec3(_v2.x, _v2.y, _v2.z).normalize();
        vec3 surface_normal = ((v2-v1).cross(v0-v1)).normalize();

        // vec4 _vsn = model_view_matrix * vec4(face_normals[i], 1.0);
        // vec3 vsn = vec3(_vsn.x, _vsn.y, _vsn.z).normalize();

        // Calculate lighting direction (assume infinitely far away)
        vec4 _vc = model_view_matrix * vec4(0.0, 0.0, 0.0, 1.0);
        vec3 vc = vec3(_vc.x, _vc.y, _vc.z).normalize();
        vec3 light_direction = light.LightDirection(vc);

        // Calculate viewing direction (assume infinitely far away)
        vec3 view_direction = -vc;

        // Calculate intensity
        vec3 intensity = material.PhongIllumination(view_direction, surface_normal, light_direction, light);
        // Uint8 r = (Uint8)round((surface_normal.x/2.0 + 0.5) * 255);
        // Uint8 g = (Uint8)round((surface_normal.y/2.0 + 0.5) * 255);
        // Uint8 b = (Uint8)round((surface_normal.z/2.0 + 0.5) * 255);
        Uint8 r = (Uint8)round((intensity.x/2.0 + 0.5) * 255);
        Uint8 g = (Uint8)round((intensity.y/2.0 + 0.5) * 255);
        Uint8 b = (Uint8)round((intensity.z/2.0 + 0.5) * 255);

        SDL_SetRenderDrawColor(renderer, r, g, b, 0xFF);

        printf("surface_normal: (x: %f\ty: %f\tz: %f)\n", surface_normal.x, surface_normal.y, surface_normal.z);
        // printf("intensity: (x: %f\ty: %f\tz: %f)\n", intensity.x, intensity.y, intensity.z);
        printf("rgb: (r: %d\tg: %d\tb: %d)\n", r, g, b);
        if (i == 65) {
            printf("%d\n",i);
            printf("view_center: (x: %f\ty: %f\tz: %f)\n", vc.x, vc.y, vc.z);
            printf("v0: (x: %f\ty: %f\tz: %f)\n", v0.x, v0.y, v0.z);
            printf("v1: (x: %f\ty: %f\tz: %f)\n", v1.x, v1.y, v1.z);
            printf("v2: (x: %f\ty: %f\tz: %f)\n", v2.x, v2.y, v2.z);
            // printf("alt_normal: (x: %f\ty: %f\tz: %f)\n", vsn.x, vsn.y, vsn.z);
            printf("surface_normal: (x: %f\ty: %f\tz: %f)\n", surface_normal.x, surface_normal.y, surface_normal.z);
            printf("light_direction: (x: %f\ty: %f\tz: %f)\n", light_direction.x, light_direction.y, light_direction.z);
            printf("view_direction: (x: %f\ty: %f\tz: %f)\n", view_direction.x, view_direction.y, view_direction.z);
            printf("intensity: (x: %f\ty: %f\tz: %f)\n", intensity.x, intensity.y, intensity.z);
            printf("intensity: (r: %d\tg: %d\tb: %d)\n", r, g, b);
            printf("\n\n");
        }
        else {
            // SDL_SetRenderDrawColor(renderer, 0xAA, 0xAA, 0xAA, 0xFF);
        }

        // Use constant random color
        // SDL_SetRenderDrawColor(renderer, (Uint8)face_colors[i].x, (Uint8)face_colors[i].y, (Uint8)face_colors[i].z, 0xFF);

        EdgeTable et;
        // For each edge in face 
        for (unsigned int k = 0; k < faces[i].indices.size(); k++) {

            // Get perspective transform of edge
            int _p0 = faces[i].indices[k];
            int _p1 = faces[i].indices[(k + 1) % faces[i].indices.size()];
            vec4 _h0 = perspective_transform * vec4(verts[_p0], 1.0);
            vec4 _h1 = perspective_transform * vec4(verts[_p1], 1.0);
            vec3 e0 = vec3(_h0.x/_h0.w, _h0.y/_h0.w, _h0.z/_h0.w);
            vec3 e1 = vec3(_h1.x/_h1.w, _h1.y/_h1.w, _h1.z/_h1.w);

            // Scale normalized coordinates [-1, 1] to device coordinates [SCREEN_WIDTH, SCREEN_HEIGHT]
            float half_width = SCREEN_WIDTH / 2.0;
            float half_height = SCREEN_HEIGHT / 2.0;

            float x0 = half_width * e0.x + half_width;
            float x1 = half_width * e1.x + half_width;
            float y0 = half_height * e0.y + half_height;
            float y1 = half_height * e1.y + half_height;
            float z0 = e0.z;
            float z1 = e1.z;

            // Round points 0 and 1
            int iy0 = (int)round(y0);
            int iy1 = (int)round(y1);

            float inv_m = (x1 - x0)/(y1 - y0);

            // Add only non-horizontal edges to ET
            if (comparefloats(iy0, iy1, FLOAT_TOL) != 0) {

                /* Assume convex polygon - don't shorten edges */

                // Add to edge table
                int y_max;      // higher y value
                int y_min;      // lower y value
                float x_min;    // x value at lower edge
                float z_min;    // z value at lower edge
                float del_z;    // rate of change from z_min
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

        // Create active edge table 
        ActiveEdgeTable aet;
            
        // Start at the first scanline containing an edge
        // Stop when ET and AET are empty
        for (int y = et.scanlines.begin()->first; (!et.IsEmpty() || !aet.IsEmpty()) && y < SCREEN_HEIGHT; y++) {
            // Move edges from ET to AET
            Edge* e;
            while((e = et.RemoveEdge(y)) != nullptr) {
                // AET is keyed by x_int
                int x_int = (int)round(e->x_min);
                aet.InsertEdge(x_int, e);
            }

            // Draw lines between pairs of edges in AET
            assert((*aet.aet).size() % 2 == 0);
            std::multimap<int,Edge*>::iterator it;
            for (it = (*aet.aet).begin(); it != (*aet.aet).end(); it++) {
                int ix0 = it->first;
                Edge *e0 = it->second;
                it++;
                int ix1 = it->first;
                Edge *e1 = it->second;

                assert(ix0 >= 0 && ix0 < SCREEN_WIDTH);
                assert(ix1 >= 0 && ix1 < SCREEN_WIDTH);

                // Fill in points between and including edges
                float z0 = e0->z_min;
                float z1 = e1->z_min;
                float hor_del_z = (z1 - z0)/(ix1 - ix0);
                float z = z0;

                for (int x = ix0; x <= ix1; x++) {
                    // Only draw point if point is in front of current z value
                    if (comparefloats(z, buffer[x][y][3], FLOAT_TOL) == -1) {
                        buffer[x][y][3] = z;
                        SDL_RenderDrawPoint(renderer, x, y);
                    }
                    z += hor_del_z;
                }
            }

            // Update edges
            aet.UpdateEdges(y);
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
        r = 1.0 / r;
    }

    // scale
    for (unsigned int i = 0; i < verts.size(); i++) {
        // [0, 1]
        verts[i] = (verts[i] - min) * r;

        // [-1, 1]
        verts[i] = verts[i] * 2.0 - vec3(1.0, 1.0, 1.0);

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
    translate_matrix[3] = offset.x;
    translate_matrix[7] = offset.y;
    translate_matrix[11] = offset.z;
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
