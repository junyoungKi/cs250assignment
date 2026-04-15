/*!
\file      FrustumMesh.cpp
\par       CS250 Camera Frustum Programming Assignment
\author    Junyoung Ki
\date      2026-03-26
\par       Course: CS250
\copyright Copyright (C) 2026 DigiPen Institute of Technology
*/

#include <cmath>
#include "FrustumMesh.h"

const Mesh::Edge FrustumMesh::edges[16] = {
    {1,2}, {2,3}, {3,4}, {4,1}, // Near plane edges
    {5,6}, {6,7}, {7,8}, {8,5}, // Far plane edges
    {1,5}, {2,6}, {3,7}, {4,8}, // Side edges
    {0,1}, {0,2}, {0,3}, {0,4}  // COP to Near plane edges
};

const Mesh::Face FrustumMesh::faces[12] = {
    {1,2,3}, {1,3,4}, // Near face
    {5,8,7}, {5,7,6}, // Far face
    {1,5,6}, {1,6,2}, // Top face
    {4,8,5}, {4,5,1}, // Right face
    {3,7,8}, {3,8,4}, // Bottom face
    {2,6,7}, {2,7,3}  // Left face
};

FrustumMesh::FrustumMesh(float fov, float aspect, float near, float far) {
    vertices[0] = Point(0, 0, 0);

    float tanHalfFov = std::tan(fov / 2.0f);
    float w_n = 2.0f * near * tanHalfFov;
    float h_n = w_n / aspect;
    float w_f = 2.0f * far * tanHalfFov;
    float h_f = w_f / aspect;

    vertices[1] = Point(w_n / 2, h_n / 2, -near);
    vertices[2] = Point(-w_n / 2, h_n / 2, -near);
    vertices[3] = Point(-w_n / 2, -h_n / 2, -near);
    vertices[4] = Point(w_n / 2, -h_n / 2, -near);

    vertices[5] = Point(w_f / 2, h_f / 2, -far);
    vertices[6] = Point(-w_f / 2, h_f / 2, -far);
    vertices[7] = Point(-w_f / 2, -h_f / 2, -far);
    vertices[8] = Point(w_f / 2, -h_f / 2, -far);

    dimensions = Vector(w_f, h_f, far);
    center = Point(0, 0, -(near + far) / 2.0f);
}

int FrustumMesh::VertexCount(void) { return 9; }

Point FrustumMesh::GetVertex(int i) { return vertices[i]; }

Vector FrustumMesh::Dimensions(void) { return dimensions; }

Point FrustumMesh::Center(void) { return center; }

int FrustumMesh::FaceCount(void) { return 12; }

Mesh::Face FrustumMesh::GetFace(int i) { return faces[i]; }

int FrustumMesh::EdgeCount(void) { return 16; }

Mesh::Edge FrustumMesh::GetEdge(int i) { return edges[i]; }