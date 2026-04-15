/******************************************************************************
 * File: CameraRender2D.cpp
 * Author: Junyoung Ki
 * Course: CS200
 * Description: Renders a mesh using camera transformations.
 *****************************************************************************/
#include "CameraRender2D.h"
#include "Projection.h"
#include <cmath>
#include <algorithm>

 /**
  * @brief Constructor for CameraRender2D.
  * @param r Reference to the Render object used for drawing primitives.
  */
CameraRender2D::CameraRender2D(Render& r) : render(r) {
    SetCamera(Camera());
}

/**
 * @brief Destructor for CameraRender2D.
 */
CameraRender2D::~CameraRender2D(void) {
}

/**
 * @brief Updates the internal View and Projection matrices based on the provided camera.
 * @param cam The camera object defining the current viewpoint and perspective.
 */
void CameraRender2D::SetCamera(const Camera& cam) {
    world2camera = WorldToCamera(cam);
    camera2ndc = CameraToNDC(cam);
}

/**
 * @brief Renders the wireframe (edges) of a mesh after applying transformations.
 * @details Transforms vertices to NDC space, performs perspective division,
 * and draws edges if both vertices are within the visible Z-range.
 * @param m The mesh to be rendered.
 * @param A The model's world transformation matrix.
 * @param color The color to be used for the wireframe.
 */
void CameraRender2D::DisplayEdges(Mesh& m, const Affine& A, const Vector& color) {
    int vCount = m.VertexCount();
    cam_vertices.assign(vCount, Point());
    ndc_vertices.assign(vCount, Point());

    // Transform all vertices from Model -> World -> Camera -> NDC
    for (int i = 0; i < vCount; ++i) {
        cam_vertices[i] = world2camera * (A * m.GetVertex(i));
        Hcoord h = camera2ndc * cam_vertices[i];
        // Perspective Division: Convert homogeneous coordinates to Cartesian NDC
        ndc_vertices[i] = Point(h.x / h.w, h.y / h.w, h.z / h.w);
    }

    render.SetColor(color);
    // Render edges that are in front of the camera (Z < 0 in RHS camera space)
    for (int i = 0; i < m.EdgeCount(); ++i) {
        Mesh::Edge e = m.GetEdge(i);
        if (cam_vertices[e.index1].z < 0 && cam_vertices[e.index2].z < 0) {
            render.DrawLine(ndc_vertices[e.index1], ndc_vertices[e.index2]);
        }
    }
}

/**
 * @brief Renders the shaded faces of a mesh with back-face culling.
 * @details Transforms faces to NDC, calculates face normals for culling and
 * shading, and fills triangles based on light intensity.
 * @param m The mesh to be rendered.
 * @param A The model's world transformation matrix.
 * @param color The base color of the mesh faces.
 */
void CameraRender2D::DisplayFaces(Mesh& m, const Affine& A, const Vector& color) {
    int vCount = m.VertexCount();
    cam_vertices.assign(vCount, Point());
    ndc_vertices.assign(vCount, Point());

    // Transform all vertices from Model -> World -> Camera -> NDC
    for (int i = 0; i < vCount; ++i) {
        cam_vertices[i] = world2camera * (A * m.GetVertex(i));
        Hcoord h = camera2ndc * cam_vertices[i];
        ndc_vertices[i] = Point(h.x / h.w, h.y / h.w, h.z / h.w);
    }

    for (int i = 0; i < m.FaceCount(); ++i) {
        Mesh::Face f = m.GetFace(i);
        Point p0 = cam_vertices[f.index1];
        Point p1 = cam_vertices[f.index2];
        Point p2 = cam_vertices[f.index3];

        // Z-Clipping: Skip faces if any vertex is behind or at the camera plane
        if (p0.z >= 0 || p1.z >= 0 || p2.z >= 0) continue;

        // Calculate face normal in camera space
        Vector normal = cross(p1 - p0, p2 - p0);

        // Back-face Culling: Only render faces where the normal points towards the camera
        if (dot(normal, Vector(p0.x, p0.y, p0.z)) < 0) {
            normal = normalize(normal);
            // Diffuse Shading: Intensity based on the angle between normal and view direction (Z-axis)
            float intensity = std::max(0.0f, dot(normal, Vector(0, 0, 1)));
            Vector shadedColor(color.x * intensity, color.y * intensity, color.z * intensity);

            render.SetColor(shadedColor);
            render.FillTriangle(ndc_vertices[f.index1], ndc_vertices[f.index2], ndc_vertices[f.index3]);
        }
    }
}