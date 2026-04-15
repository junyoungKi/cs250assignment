/*!
\file      SimpleRender.cpp
\par       CS250 Camera Frustum Programming Assignment
\author    Junyoung Ki
\date      2026-03-26
\par       Course: CS250
\copyright Copyright (C) 2026 DigiPen Institute of Technology
*/

#include <cmath>
#include "SimpleRender.h"

SimpleRender::SimpleRender(Render& r) : render(r) {
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            PersProj[i][j] = 0.0f;

    PersProj[0][0] = 11.0f;
    PersProj[1][1] = 11.0f;
    PersProj[2][2] = 1.0f;  PersProj[2][3] = -5.0f;
    PersProj[3][2] = -2.0f; PersProj[3][3] = 10.0f;
}

SimpleRender::~SimpleRender(void) {}

void SimpleRender::DisplayEdges(Mesh& m, const Affine& A, const Vector& color) {
    int vCount = m.VertexCount();
    if (vCount <= 0) return;

    proj_vertices.resize(vCount);

    for (int i = 0; i < vCount; ++i) {
        Point worldP = A * m.GetVertex(i);
        Hcoord h = PersProj * worldP;

        if (std::abs(h.w) > 0.0001f) {
            proj_vertices[i] = Point(h.x / h.w, h.y / h.w, h.z / h.w);
        }
        else {
            proj_vertices[i] = Point(0, 0, 0);
        }
    }

    render.SetColor(color);
    for (int i = 0; i < m.EdgeCount(); ++i) {
        Mesh::Edge e = m.GetEdge(i);
        if (e.index1 < vCount && e.index2 < vCount) {
            render.DrawLine(proj_vertices[e.index1], proj_vertices[e.index2]);
        }
    }
}

void SimpleRender::DisplayFaces(Mesh& m, const Affine& A, const Vector& color) {
    int vCount = m.VertexCount();
    if (vCount <= 0) return;

    proj_vertices.resize(vCount);
    world_vertices.resize(vCount);

    for (int i = 0; i < vCount; ++i) {
        world_vertices[i] = A * m.GetVertex(i);
        Hcoord h = PersProj * world_vertices[i];

        if (std::abs(h.w) > 0.0001f) {
            proj_vertices[i] = Point(h.x / h.w, h.y / h.w, h.z / h.w);
        }
        else {
            proj_vertices[i] = Point(0, 0, 0);
        }
    }

    Point COP(0, 0, 5);
    for (int i = 0; i < m.FaceCount(); ++i) {
        Mesh::Face f = m.GetFace(i);
        if (f.index1 >= vCount || f.index2 >= vCount || f.index3 >= vCount) continue;

        Vector edge1 = world_vertices[f.index2] - world_vertices[f.index1];
        Vector edge2 = world_vertices[f.index3] - world_vertices[f.index1];

        Vector N(edge1.y * edge2.z - edge1.z * edge2.y,
            edge1.z * edge2.x - edge1.x * edge2.z,
            edge1.x * edge2.y - edge1.y * edge2.x);

        Vector viewDir = world_vertices[f.index1] - COP;
        float dotView = viewDir.x * N.x + viewDir.y * N.y + viewDir.z * N.z;

        if (dotView < 0) {
            float magN = std::sqrt(N.x * N.x + N.y * N.y + N.z * N.z);
            float mu = (magN > 0.0001f) ? (N.z / magN) : 0.0f;
            if (mu < 0) mu = 0;

            render.SetColor(mu * color);
            render.FillTriangle(proj_vertices[f.index1], proj_vertices[f.index2], proj_vertices[f.index3]);
        }
    }
}