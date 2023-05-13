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
    model_face_normals.clear();
    face_colors.clear();
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
    model_face_normals.resize(numFaces);
    face_colors.resize(numFaces);

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
        model_face_normals[i] = normal.normalize();

        // Set face to random color
        face_colors[i] = vec3(rand() % 256, rand() % 256, rand() % 256);
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
        vec4 _normal = model_matrix * vec4(model_face_normals[i], 1.0);
        vec3 normal = vec3(_normal.x, _normal.y, _normal.z).normalize();
        vec4 _view = model_matrix * vec4(verts[faces[i].indices[1]], 1.0);
        vec3 view = (vec3(_view.x, _view.y, _view.z) - camera.position);
        float dot = normal.dot(view);

        // Visible if dot of normal and line of sight is positive
        if (BACK_FACE_CULLING && comparefloats(dot,0.0,FLOAT_TOL) <= 0)
            continue;

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

void Model::DrawFaces(Camera &camera, SDL_Renderer *renderer, float buffer[SCREEN_WIDTH][SCREEN_HEIGHT], bool render_depth) {
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
        vec4 _normal = model_matrix * vec4(model_face_normals[i], 1.0);
        vec3 normal = vec3(_normal.x, _normal.y, _normal.z).normalize();
        vec4 _view = model_matrix * vec4(verts[faces[i].indices[1]], 1.0);
        vec3 view = (vec3(_view.x, _view.y, _view.z) - camera.position);
        float dot = normal.dot(view);

        // Visible if dot of normal and line of sight is positive
        if (BACK_FACE_CULLING && comparefloats(dot,0.0,FLOAT_TOL) <= 0)
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
                    if (comparefloats(z, buffer[x][y], FLOAT_TOL) == -1) {
                        buffer[x][y] = z;

                        // Draw depth map
                        if (render_depth) {
                            Uint8 c = (Uint8)round(255 * ((z - 0.95) / 0.05)); 
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

void Model::DrawFlat(Camera &camera, Light &light, Material &material, SDL_Renderer *renderer, float buffer[SCREEN_WIDTH][SCREEN_HEIGHT]) {
    // Apply transformation matrices to get from
    // Model -> World -> Screen 

    // Calculate transformation matrix
    mat4 model_matrix = translate_matrix * rotate_matrix * scale_matrix;
    mat4 view_matrix = camera.GetViewMatrix();
    mat4 perspective_matrix = camera.GetPerspectiveMatrix();
    mat4 model_view_matrix = view_matrix * model_matrix;
    mat4 perspective_transform = perspective_matrix * model_view_matrix;

    // Calculate viewing and lighting direction (assume both are infinitely far away)
    vec4 _center = model_matrix * vec4(0.0, 0.0, 0.0, 1.0);
    vec3 center = vec3(_center.x, _center.y, _center.z);
    vec3 view_direction = (camera.position - center).normalize();
    vec3 light_direction = light.LightDirection(center);

    // For each face in model
    for (unsigned int i = 0; i < faces.size(); i++) {
        // Backface culling 
        vec4 _normal = model_matrix * vec4(model_face_normals[i], 1.0);
        vec3 normal = vec3(_normal.x, _normal.y, _normal.z).normalize();
        vec4 _view = model_matrix * vec4(verts[faces[i].indices[1]], 1.0);
        vec3 view = (vec3(_view.x, _view.y, _view.z) - camera.position);
        float dot = normal.dot(view);

        // Visible if dot of normal and line of sight is positive
        if (BACK_FACE_CULLING && comparefloats(dot,0.0,FLOAT_TOL) <= 0)
            continue;

        // Calculate surface normal
        vec4 _v0 = model_matrix * vec4(verts[faces[i].indices[0]], 1.0);
        vec4 _v1 = model_matrix * vec4(verts[faces[i].indices[1]], 1.0);
        vec4 _v2 = model_matrix * vec4(verts[faces[i].indices[2]], 1.0);
        vec3 v0 = vec3(_v0.x, _v0.y, _v0.z);
        vec3 v1 = vec3(_v1.x, _v1.y, _v1.z);
        vec3 v2 = vec3(_v2.x, _v2.y, _v2.z);
        // Note: switching cross product A, B because of some weirdness with LH coordinate system
        // vec3 surface_normal = ((v2-v1).cross(v0-v1)).normalize();
        vec3 surface_normal = ((v0-v1).cross(v2-v1)).normalize();

        // Calculate intensity
        vec3 intensity = material.PhongIllumination(material.color, view_direction, surface_normal, light_direction, light);

        Uint8 r = (Uint8)floor(abs(intensity.x) * 255.0);
        Uint8 g = (Uint8)floor(abs(intensity.y) * 255.0);
        Uint8 b = (Uint8)floor(abs(intensity.z) * 255.0);

        // Draw RGB scaled by intensity
        SDL_SetRenderDrawColor(renderer, r, g, b, 0xFF);

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
                    if (comparefloats(z, buffer[x][y], FLOAT_TOL) == -1) {
                        buffer[x][y] = z;
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

void Model::DrawGouraud(Camera &camera, Light &light, Material &material, SDL_Renderer *renderer, float buffer[SCREEN_WIDTH][SCREEN_HEIGHT]) {
    // Apply transformation matrices to get from
    // Model -> World -> Screen 

    // Calculate transformation matrix
    mat4 model_matrix = translate_matrix * rotate_matrix * scale_matrix;
    mat4 view_matrix = camera.GetViewMatrix();
    mat4 perspective_matrix = camera.GetPerspectiveMatrix();
    mat4 model_view_matrix = view_matrix * model_matrix;
    mat4 perspective_transform = perspective_matrix * model_view_matrix;

    // Calculate viewing and lighting direction (assume both are infinitely far away)
    vec4 _center = model_matrix * vec4(0.0, 0.0, 0.0, 1.0);
    vec3 center = vec3(_center.x, _center.y, _center.z);
    vec3 view_direction = (camera.position - center).normalize();
    vec3 light_direction = light.LightDirection(center);

    // Calculate face and vertex normals
    std::vector< vec3 > face_normals;
    std::vector< vec3 > vert_normals;
    std::vector< vec3 > vert_intensities;
    face_normals.resize(faces.size());
    vert_normals.resize(verts.size());
    vert_intensities.resize(verts.size());
    for (size_t i = 0; i < faces.size(); i++) {
        // Calculate surface normal
        vec4 _v0 = model_matrix * vec4(verts[faces[i].indices[0]], 1.0);
        vec4 _v1 = model_matrix * vec4(verts[faces[i].indices[1]], 1.0);
        vec4 _v2 = model_matrix * vec4(verts[faces[i].indices[2]], 1.0);
        vec3 v0 = vec3(_v0.x, _v0.y, _v0.z);
        vec3 v1 = vec3(_v1.x, _v1.y, _v1.z);
        vec3 v2 = vec3(_v2.x, _v2.y, _v2.z);
        // Note: switching cross product A, B because of some weirdness with LH coordinate system
        // vec3 surface_normal = ((v2-v1).cross(v0-v1)).normalize();
        face_normals[i] = ((v0-v1).cross(v2-v1)).normalize();
    }

    // Calculate verts normals
    for (size_t i = 0; i < verts.size(); i++) {
        // Get all faces containing this vertex
        std::vector<int> faces_index;
        for (size_t j = 0; j < faces.size(); j++) {
            // Loop through all indices of the face to check if the vertex is in it
            for (size_t k = 0; k < faces[j].indices.size(); k++) {
                if ((unsigned int)faces[j].indices[k] == i) {
                    faces_index.push_back(j);
                }
            }
        }
        // Then average their normals
        vec3 normal_sum(0, 0, 0);
        for (size_t i = 0; i < faces_index.size(); i++) {
            normal_sum += face_normals[faces_index[i]];
        }
        vert_normals[i] = (normal_sum / faces_index.size()).normalize();

        // Calculate intensity
        vert_intensities[i] = material.PhongIllumination(material.color, view_direction, vert_normals[i], light_direction, light); 
    }

    // For each face in model
    for (unsigned int i = 0; i < faces.size(); i++) {
        // Backface culling 
        vec4 _normal = model_matrix * vec4(model_face_normals[i], 1.0);
        vec3 normal = vec3(_normal.x, _normal.y, _normal.z).normalize();
        vec4 _view = model_matrix * vec4(verts[faces[i].indices[1]], 1.0);
        vec3 view = (vec3(_view.x, _view.y, _view.z) - camera.position);
        float dot = normal.dot(view);

        // Visible if dot of normal and line of sight is positive
        if (BACK_FACE_CULLING && comparefloats(dot,0.0,FLOAT_TOL) <= 0)
            continue;

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

                // Assume convex polygon - don't shorten edges

                // Get lighting intensity at ends of edge
                vec3 vert_intensity0 = vert_intensities[_p0];
                vec3 vert_intensity1 = vert_intensities[_p1];


                // Add to edge table
                int y_max;      // higher y value
                int y_min;      // lower y value
                float x_min;    // x value at lower edge
                float z_min;    // z value at lower edge
                float del_z;    // rate of change from z_min
                vec3 vec_min;
                vec3 del_vec;
                if (iy0 < iy1) {
                    // p0 is lower than p1
                    y_max = iy1;
                    y_min = iy0;
                    x_min = x0;
                    z_min = z0;
                    del_z = (z1-z0)/(y1-y0);
                    vec_min = vert_intensity0;
                    del_vec = (1.0/(y1-y0))*(vert_intensity1 - vert_intensity0);
                }
                else {
                    // p1 is lower than p0
                    y_max = iy0;
                    y_min = iy1;
                    x_min = x1;
                    z_min = z1;
                    del_z = (z0-z1)/(y0-y1);
                    vec_min = vert_intensity1;
                    del_vec = (1.0/(y0-y1))*(vert_intensity0 - vert_intensity1);
                }

                Edge* e = new Edge(y_max, x_min, inv_m, z_min, del_z, vec_min, del_vec);
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

                // Interpolate vertex intensity horizontally
                vec3 start = e0->vec_min;
                vec3 end = e1->vec_min;
                vec3 hor_del_vec = (1.0/(ix1 - ix0))*(end - start);
                vec3 intensity = start;

                for (int x = ix0; x <= ix1; x++) {
                    // Only draw point if point is in front of current z value
                    if (comparefloats(z, buffer[x][y], FLOAT_TOL) == -1) {
                        buffer[x][y] = z;

                        // Draw RGB scaled by intensity
                        Uint8 r = (Uint8)floor(abs(intensity.x) * 255.0);
                        Uint8 g = (Uint8)floor(abs(intensity.y) * 255.0);
                        Uint8 b = (Uint8)floor(abs(intensity.z) * 255.0);

                        SDL_SetRenderDrawColor(renderer, r, g, b, 0xFF);
                        SDL_RenderDrawPoint(renderer, x, y);
                    }
                    z += hor_del_z;
                    intensity = intensity + hor_del_vec;
                }
            }

            // Update edges
            aet.UpdateEdges(y);
        }
    }
}

void Model::DrawPhong(Camera &camera, Light &light, Material &material, SDL_Renderer *renderer, float buffer[SCREEN_WIDTH][SCREEN_HEIGHT], bool render_normal) {
    // Apply transformation matrices to get from
    // Model -> World -> Screen 

    // Calculate transformation matrix
    mat4 model_matrix = translate_matrix * rotate_matrix * scale_matrix;
    mat4 view_matrix = camera.GetViewMatrix();
    mat4 perspective_matrix = camera.GetPerspectiveMatrix();
    mat4 model_view_matrix = view_matrix * model_matrix;
    mat4 perspective_transform = perspective_matrix * model_view_matrix;

    // Calculate viewing and lighting direction (assume both are infinitely far away)
    vec4 _center = model_matrix * vec4(0.0, 0.0, 0.0, 1.0);
    vec3 center = vec3(_center.x, _center.y, _center.z);
    vec3 view_direction = (camera.position - center).normalize();
    vec3 light_direction = light.LightDirection(center);

    // Calculate face and vertex normals
    std::vector< vec3 > face_normals;
    std::vector< vec3 > vert_normals;
    std::vector< vec3 > vert_intensities;
    face_normals.resize(faces.size());
    vert_normals.resize(verts.size());
    vert_intensities.resize(verts.size());
    for (size_t i = 0; i < faces.size(); i++) {
        // Calculate surface normal
        vec4 _v0 = model_matrix * vec4(verts[faces[i].indices[0]], 1.0);
        vec4 _v1 = model_matrix * vec4(verts[faces[i].indices[1]], 1.0);
        vec4 _v2 = model_matrix * vec4(verts[faces[i].indices[2]], 1.0);
        vec3 v0 = vec3(_v0.x, _v0.y, _v0.z);
        vec3 v1 = vec3(_v1.x, _v1.y, _v1.z);
        vec3 v2 = vec3(_v2.x, _v2.y, _v2.z);
        // Note: switching cross product A, B because of some weirdness with LH coordinate system
        // vec3 surface_normal = ((v2-v1).cross(v0-v1)).normalize();
        face_normals[i] = ((v0-v1).cross(v2-v1)).normalize();
    }

    // Calculate verts normals
    for (size_t i = 0; i < verts.size(); i++) {
        // Get all faces containing this vertex
        std::vector<int> faces_index;
        for (size_t j = 0; j < faces.size(); j++) {
            // Loop through all indices of the face to check if the vertex is in it
            for (size_t k = 0; k < faces[j].indices.size(); k++) {
                if ((unsigned int)faces[j].indices[k] == i) {
                    faces_index.push_back(j);
                }
            }
        }
        // Then average their normals
        vec3 normal_sum(0, 0, 0);
        for (size_t i = 0; i < faces_index.size(); i++) {
            normal_sum += face_normals[faces_index[i]];
        }
        vert_normals[i] = (normal_sum / faces_index.size()).normalize();
    }

    // For each face in model
    for (unsigned int i = 0; i < faces.size(); i++) {
        // Backface culling 
        vec4 _normal = model_matrix * vec4(model_face_normals[i], 1.0);
        vec3 normal = vec3(_normal.x, _normal.y, _normal.z).normalize();
        vec4 _view = model_matrix * vec4(verts[faces[i].indices[1]], 1.0);
        vec3 view = (vec3(_view.x, _view.y, _view.z) - camera.position);
        float dot = normal.dot(view);

        // Visible if dot of normal and line of sight is positive
        if (BACK_FACE_CULLING && comparefloats(dot,0.0,FLOAT_TOL) <= 0)
            continue;

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

                // Assume convex polygon - don't shorten edges

                // Get vertex normal at ends of edge
                vec3 vert_norm0 = vert_normals[_p0];
                vec3 vert_norm1 = vert_normals[_p1];

                // Add to edge table
                int y_max;      // higher y value
                int y_min;      // lower y value
                float x_min;    // x value at lower edge
                float z_min;    // z value at lower edge
                float del_z;    // rate of change from z_min
                vec3 vec_min;
                vec3 del_vec;
                if (iy0 < iy1) {
                    // p0 is lower than p1
                    y_max = iy1;
                    y_min = iy0;
                    x_min = x0;
                    z_min = z0;
                    del_z = (z1-z0)/(y1-y0);
                    vec_min = vert_norm0;
                    del_vec = (1.0/(y1-y0))*(vert_norm1 - vert_norm0);
                }
                else {
                    // p1 is lower than p0
                    y_max = iy0;
                    y_min = iy1;
                    x_min = x1;
                    z_min = z1;
                    del_z = (z0-z1)/(y0-y1);
                    vec_min = vert_norm1;
                    del_vec = (1.0/(y0-y1))*(vert_norm0 - vert_norm1);
                }

                Edge* e = new Edge(y_max, x_min, inv_m, z_min, del_z, vec_min, del_vec);
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

                // Interpolate vertex intensity horizontally
                vec3 start = e0->vec_min;
                vec3 end = e1->vec_min;
                vec3 hor_del_vec = (1.0/(ix1 - ix0))*(end - start);
                vec3 norm = start;

                for (int x = ix0; x <= ix1; x++) {
                    // Only draw point if point is in front of current z value
                    if (comparefloats(z, buffer[x][y], FLOAT_TOL) == -1) {
                        buffer[x][y] = z;

                        // Calculate intensity
                        norm = norm.normalize();
                        vec3 intensity = material.PhongIllumination(material.color, view_direction, norm, light_direction, light); 

                        Uint8 r, g, b;
                        if (!render_normal) {
                            // Draw RGB scaled by intensity
                            r = (Uint8)floor(abs(intensity.x) * 255.0);
                            g = (Uint8)floor(abs(intensity.y) * 255.0);
                            b = (Uint8)floor(abs(intensity.z) * 255.0);
                        }
                        else {
                            // Draw RGB based on surface normal
                            r = (Uint8)floor(abs(norm.x) * 255.0);
                            g = (Uint8)floor(abs(norm.y) * 255.0);
                            b = (Uint8)floor(abs(norm.z) * 255.0);
                        }

                        SDL_SetRenderDrawColor(renderer, r, g, b, 0xFF);
                        SDL_RenderDrawPoint(renderer, x, y);
                    }
                    z += hor_del_z;
                    norm = norm + hor_del_vec;
                }
            }

            // Update edges
            aet.UpdateEdges(y);
        }
    }
}

void Model::DrawEnvironment(Camera &camera, Light &light, Material &material, SDL_Renderer *renderer, float buffer[SCREEN_WIDTH][SCREEN_HEIGHT]) {
    // Apply transformation matrices to get from
    // Model -> World -> Screen 

    // Calculate transformation matrix
    mat4 model_matrix = translate_matrix * rotate_matrix * scale_matrix;
    mat4 view_matrix = camera.GetViewMatrix();
    mat4 perspective_matrix = camera.GetPerspectiveMatrix();
    mat4 model_view_matrix = view_matrix * model_matrix;
    mat4 perspective_transform = perspective_matrix * model_view_matrix;

    // Calculate viewing and lighting direction (assume both are infinitely far away)
    vec4 _center = model_matrix * vec4(0.0, 0.0, 0.0, 1.0);
    vec3 center = vec3(_center.x, _center.y, _center.z);
    vec3 view_direction = (camera.position - center).normalize();
    vec3 light_direction = light.LightDirection(center);

    // Calculate face and vertex normals
    std::vector< vec3 > face_normals;
    std::vector< vec3 > vert_normals;
    std::vector< vec3 > vert_intensities;
    face_normals.resize(faces.size());
    vert_normals.resize(verts.size());
    vert_intensities.resize(verts.size());
    for (size_t i = 0; i < faces.size(); i++) {
        // Calculate surface normal
        vec4 _v0 = model_matrix * vec4(verts[faces[i].indices[0]], 1.0);
        vec4 _v1 = model_matrix * vec4(verts[faces[i].indices[1]], 1.0);
        vec4 _v2 = model_matrix * vec4(verts[faces[i].indices[2]], 1.0);
        vec3 v0 = vec3(_v0.x, _v0.y, _v0.z);
        vec3 v1 = vec3(_v1.x, _v1.y, _v1.z);
        vec3 v2 = vec3(_v2.x, _v2.y, _v2.z);
        // Note: switching cross product A, B because of some weirdness with LH coordinate system
        // vec3 surface_normal = ((v2-v1).cross(v0-v1)).normalize();
        face_normals[i] = ((v0-v1).cross(v2-v1)).normalize();
    }

    // Calculate verts normals
    for (size_t i = 0; i < verts.size(); i++) {
        // Get all faces containing this vertex
        std::vector<int> faces_index;
        for (size_t j = 0; j < faces.size(); j++) {
            // Loop through all indices of the face to check if the vertex is in it
            for (size_t k = 0; k < faces[j].indices.size(); k++) {
                if ((unsigned int)faces[j].indices[k] == i) {
                    faces_index.push_back(j);
                }
            }
        }
        // Then average their normals
        vec3 normal_sum(0, 0, 0);
        for (size_t i = 0; i < faces_index.size(); i++) {
            normal_sum += face_normals[faces_index[i]];
        }
        vert_normals[i] = (normal_sum / faces_index.size()).normalize();
    }

    // For each face in model
    for (unsigned int i = 0; i < faces.size(); i++) {
        // Backface culling 
        vec4 _normal = model_matrix * vec4(model_face_normals[i], 1.0);
        vec3 normal = vec3(_normal.x, _normal.y, _normal.z).normalize();
        vec4 _view = model_matrix * vec4(verts[faces[i].indices[1]], 1.0);
        vec3 view = (vec3(_view.x, _view.y, _view.z) - camera.position);
        float dot = normal.dot(view);

        // Visible if dot of normal and line of sight is positive
        if (BACK_FACE_CULLING && comparefloats(dot,0.0,FLOAT_TOL) <= 0)
            continue;

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

                // Assume convex polygon - don't shorten edges

                // Get vertex normal at ends of edge
                vec3 vert_norm0 = vert_normals[_p0];
                vec3 vert_norm1 = vert_normals[_p1];

                // Add to edge table
                int y_max;      // higher y value
                int y_min;      // lower y value
                float x_min;    // x value at lower edge
                float z_min;    // z value at lower edge
                float del_z;    // rate of change from z_min
                vec3 vec_min;
                vec3 del_vec;
                if (iy0 < iy1) {
                    // p0 is lower than p1
                    y_max = iy1;
                    y_min = iy0;
                    x_min = x0;
                    z_min = z0;
                    del_z = (z1-z0)/(y1-y0);
                    vec_min = vert_norm0;
                    del_vec = (1.0/(y1-y0))*(vert_norm1 - vert_norm0);
                }
                else {
                    // p1 is lower than p0
                    y_max = iy0;
                    y_min = iy1;
                    x_min = x1;
                    z_min = z1;
                    del_z = (z0-z1)/(y0-y1);
                    vec_min = vert_norm1;
                    del_vec = (1.0/(y0-y1))*(vert_norm0 - vert_norm1);
                }

                Edge* e = new Edge(y_max, x_min, inv_m, z_min, del_z, vec_min, del_vec);
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

                // Interpolate vertex intensity horizontally
                vec3 start = e0->vec_min;
                vec3 end = e1->vec_min;
                vec3 hor_del_vec = (1.0/(ix1 - ix0))*(end - start);
                vec3 norm = start;

                for (int x = ix0; x <= ix1; x++) {
                    // Only draw point if point is in front of current z value
                    if (comparefloats(z, buffer[x][y], FLOAT_TOL) == -1) {
                        buffer[x][y] = z;

                        // Calculate intensity
                        norm = norm.normalize();

                        // Get corresponding color from texture map
                        vec3 texture = material.GetTexture(norm);

                        vec3 intensity = material.PhongIllumination(texture, view_direction, norm, light_direction, light); 

                        // Draw RGB based on intensity
                        Uint8 r = (Uint8)floor(abs(intensity.x) * 255.0);
                        Uint8 g = (Uint8)floor(abs(intensity.y) * 255.0);
                        Uint8 b = (Uint8)floor(abs(intensity.z) * 255.0);

                        SDL_SetRenderDrawColor(renderer, r, g, b, 0xFF);
                        SDL_RenderDrawPoint(renderer, x, y);
                    }
                    z += hor_del_z;
                    norm = norm + hor_del_vec;
                }
            }

            // Update edges
            aet.UpdateEdges(y);
        }
    }
}

void Model::DrawTexture(Camera &camera, Light &light, Material &material, SDL_Renderer *renderer, float buffer[SCREEN_WIDTH][SCREEN_HEIGHT]) {
    // Apply transformation matrices to get from
    // Model -> World -> Screen 

    // Calculate transformation matrix
    mat4 model_matrix = translate_matrix * rotate_matrix * scale_matrix;
    mat4 view_matrix = camera.GetViewMatrix();
    mat4 perspective_matrix = camera.GetPerspectiveMatrix();
    mat4 model_view_matrix = view_matrix * model_matrix;
    mat4 perspective_transform = perspective_matrix * model_view_matrix;

    // Calculate viewing and lighting direction (assume both are infinitely far away)
    vec4 _center = model_matrix * vec4(0.0, 0.0, 0.0, 1.0);
    vec3 center = vec3(_center.x, _center.y, _center.z);
    vec3 view_direction = (camera.position - center).normalize();
    vec3 light_direction = light.LightDirection(center);

    // Calculate face and vertex normals
    std::vector< vec3 > face_normals;
    std::vector< vec3 > vert_normals;
    std::vector< vec3 > vert_intensities;
    face_normals.resize(faces.size());
    vert_normals.resize(verts.size());
    vert_intensities.resize(verts.size());
    for (size_t i = 0; i < faces.size(); i++) {
        // Calculate surface normal
        vec4 _v0 = model_matrix * vec4(verts[faces[i].indices[0]], 1.0);
        vec4 _v1 = model_matrix * vec4(verts[faces[i].indices[1]], 1.0);
        vec4 _v2 = model_matrix * vec4(verts[faces[i].indices[2]], 1.0);
        vec3 v0 = vec3(_v0.x, _v0.y, _v0.z);
        vec3 v1 = vec3(_v1.x, _v1.y, _v1.z);
        vec3 v2 = vec3(_v2.x, _v2.y, _v2.z);
        // Note: switching cross product A, B because of some weirdness with LH coordinate system
        // vec3 surface_normal = ((v2-v1).cross(v0-v1)).normalize();
        face_normals[i] = ((v0-v1).cross(v2-v1)).normalize();
    }

    // Calculate verts normals
    for (size_t i = 0; i < verts.size(); i++) {
        // Get all faces containing this vertex
        std::vector<int> faces_index;
        for (size_t j = 0; j < faces.size(); j++) {
            // Loop through all indices of the face to check if the vertex is in it
            for (size_t k = 0; k < faces[j].indices.size(); k++) {
                if ((unsigned int)faces[j].indices[k] == i) {
                    faces_index.push_back(j);
                }
            }
        }
        // Then average their normals
        vec3 normal_sum(0, 0, 0);
        for (size_t i = 0; i < faces_index.size(); i++) {
            normal_sum += face_normals[faces_index[i]];
        }
        vert_normals[i] = (normal_sum / faces_index.size()).normalize();
    }

    // For each face in model
    for (unsigned int i = 0; i < faces.size(); i++) {
        // Backface culling 
        vec4 _normal = model_matrix * vec4(model_face_normals[i], 1.0);
        vec3 normal = vec3(_normal.x, _normal.y, _normal.z).normalize();
        vec4 _view = model_matrix * vec4(verts[faces[i].indices[1]], 1.0);
        vec3 view = (vec3(_view.x, _view.y, _view.z) - camera.position);
        float dot = normal.dot(view);

        // Visible if dot of normal and line of sight is positive
        if (BACK_FACE_CULLING && comparefloats(dot,0.0,FLOAT_TOL) <= 0)
            continue;

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

                // Assume convex polygon - don't shorten edges

                // Get vertex normal at ends of edge
                vec3 vert_norm0 = vert_normals[_p0];
                vec3 vert_norm1 = vert_normals[_p1];

                // Add to edge table
                int y_max;      // higher y value
                int y_min;      // lower y value
                float x_min;    // x value at lower edge
                float z_min;    // z value at lower edge
                float del_z;    // rate of change from z_min
                vec3 vec_min;
                vec3 del_vec;
                vec3 vert_min;
                vec3 del_vert;
                if (iy0 < iy1) {
                    // p0 is lower than p1
                    y_max = iy1;
                    y_min = iy0;
                    x_min = x0;
                    z_min = z0;
                    del_z = (z1-z0)/(y1-y0);
                    vec_min = vert_norm0;
                    del_vec = (1.0/(y1-y0))*(vert_norm1 - vert_norm0);
                    vert_min = verts[_p0];
                    del_vert = (1.0/(y1-y0))*(verts[_p1] - verts[_p0]);
                }
                else {
                    // p1 is lower than p0
                    y_max = iy0;
                    y_min = iy1;
                    x_min = x1;
                    z_min = z1;
                    del_z = (z0-z1)/(y0-y1);
                    vec_min = vert_norm1;
                    del_vec = (1.0/(y0-y1))*(vert_norm0 - vert_norm1);
                    vert_min = verts[_p1];
                    del_vert = (1.0/(y0-y1))*(verts[_p0] - verts[_p1]);
                }

                Edge* e = new Edge(y_max, x_min, inv_m, z_min, del_z, vec_min, del_vec, vert_min, del_vert);
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

                // Interpolate vertex intensity horizontally
                vec3 start = e0->vec_min;
                vec3 end = e1->vec_min;
                vec3 hor_del_vec = (1.0/(ix1 - ix0))*(end - start);
                vec3 norm = start;

                // Interpolate vertex position horizontally
                vec3 v_start = e0->vert_min;
                vec3 v_end = e1->vert_min;
                vec3 hor_del_vert = (1.0/(ix1 - ix0))*(v_end - v_start);
                vec3 vert = v_start;

                for (int x = ix0; x <= ix1; x++) {
                    // Only draw point if point is in front of current z value
                    if (comparefloats(z, buffer[x][y], FLOAT_TOL) == -1) {
                        buffer[x][y] = z;

                        // Calculate intensity
                        norm = norm.normalize();

                        // Get corresponding color from texture map
                        vert = vert.normalize();
                        vec3 texture = material.GetTexture(vert);

                        vec3 intensity = material.PhongIllumination(texture, view_direction, norm, light_direction, light); 

                        // Draw RGB based on intensity
                        Uint8 r = (Uint8)floor(abs(intensity.x) * 255.0);
                        Uint8 g = (Uint8)floor(abs(intensity.y) * 255.0);
                        Uint8 b = (Uint8)floor(abs(intensity.z) * 255.0);

                        SDL_SetRenderDrawColor(renderer, r, g, b, 0xFF);
                        SDL_RenderDrawPoint(renderer, x, y);
                    }
                    z += hor_del_z;
                    norm = norm + hor_del_vec;
                    vert = vert + hor_del_vert;
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
