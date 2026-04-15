/*!
\file      CubeMesh.cpp
\par       CS250 Affine and Mesh Programming Assignment
\author    Junyoung Ki
\date      2026-03-25
\par       Course: CS250
\copyright Copyright (C) 2026 DigiPen Institute of Technology
*/


#include "CubeMesh.h"

const Point CubeMesh::vertices[8] = {
  Point(-1,-1,-1), // 0
  Point(1,-1,-1), // 1
  Point(1, 1,-1), // 2
  Point(-1, 1,-1), // 3
  Point(-1,-1, 1), // 4
  Point(1,-1, 1), // 5
  Point(1, 1, 1), // 6
  Point(-1, 1, 1)  // 7
};

// CCW
const Mesh::Face CubeMesh::faces[12] = {
  {0,3,2}, {0,2,1}, // Back  (Z = -1)
  {4,5,6}, {4,6,7}, // Front (Z = 1)
  {0,1,5}, {0,5,4}, // Bottom (Y = -1)
  {3,7,6}, {3,6,2}, // Top    (Y = 1)
  {1,2,6}, {1,6,5}, // Right  (X = 1)
  {0,4,7}, {0,7,3}  // Left   (X = -1)
};

const Mesh::Edge CubeMesh::edges[12] = {
  {0,1}, {1,2}, {2,3}, {3,0}, {4,5}, {5,6},
  {6,7}, {7,4}, {0,4}, {1,5}, {2,6}, {3,7}
};

int CubeMesh::VertexCount(void) { return 8; }
Point CubeMesh::GetVertex(int i) { return vertices[i]; }
Vector CubeMesh::Dimensions(void) { return Vector(2, 2, 2); }
Point CubeMesh::Center(void) { return Point(0, 0, 0); }
int CubeMesh::FaceCount(void) { return 12; }
Mesh::Face CubeMesh::GetFace(int i) { return faces[i]; }
int CubeMesh::EdgeCount(void) { return 12; }
Mesh::Edge CubeMesh::GetEdge(int i) { return edges[i]; }