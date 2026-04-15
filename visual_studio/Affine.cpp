/*!
\file      Affine.cpp
\par       CS250 Affine and Mesh Programming Assignment
\author    Junyoung Ki
\date      2026-03-25
\par       Course: CS250
\copyright Copyright (C) 2026 DigiPen Institute of Technology
*/


#include "Affine.h"
#include <cmath>

// ---------------- Constructors ----------------

Hcoord::Hcoord(float X, float Y, float Z, float W)
    : x(X), y(Y), z(Z), w(W) {}


Point::Point(float X, float Y, float Z)
    : Hcoord(X, Y, Z, 1.0f) {}


Vector::Vector(float X, float Y, float Z)
    : Hcoord(X, Y, Z, 0.0f) {}


Affine::Affine(void) {
    row[0] = Hcoord(1, 0, 0, 0);
    row[1] = Hcoord(0, 1, 0, 0);
    row[2] = Hcoord(0, 0, 1, 0);
    row[3] = Hcoord(0, 0, 0, 1);
}


Affine::Affine(const Vector& Lx, const Vector& Ly, const Vector& Lz, const Point& D) {
    row[0] = Hcoord(Lx.x, Ly.x, Lz.x, D.x);
    row[1] = Hcoord(Lx.y, Ly.y, Lz.y, D.y);
    row[2] = Hcoord(Lx.z, Ly.z, Lz.z, D.z);
    row[3] = Hcoord(0, 0, 0, 1);
}

// ---------------- Hcoord operations ----------------
Hcoord operator+(const Hcoord& u, const Hcoord& v) {
    return Hcoord(u.x + v.x, u.y + v.y, u.z + v.z, u.w + v.w);
}

Hcoord operator-(const Hcoord& u, const Hcoord& v) {
    return Hcoord(u.x - v.x, u.y - v.y, u.z - v.z, u.w - v.w);
}

Hcoord operator-(const Hcoord& v) {
    return Hcoord(-v.x, -v.y, -v.z, -v.w);
}

Hcoord operator*(float r, const Hcoord& v) {
    return Hcoord(r * v.x, r * v.y, r * v.z, r * v.w);
}

// ---------------- Matrix operations ----------------
Hcoord operator*(const Matrix& A, const Hcoord& v) {
    Hcoord result;
    for (int i = 0; i < 4; i++) {
        result[i] = A[i][0] * v[0] + A[i][1] * v[1] + A[i][2] * v[2] + A[i][3] * v[3];
    }
    return result;
}

Matrix operator*(const Matrix& A, const Matrix& B) {
    Matrix result;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result[i][j] = 0;
            for (int k = 0; k < 4; k++) {
                result[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    return result;
}

// ---------------- Vector math ----------------
float dot(const Vector& u, const Vector& v) {
    return u.x * v.x + u.y * v.y + u.z * v.z;
}

float abs(const Vector& v) {
    return std::sqrt(dot(v, v));
}

Vector normalize(const Vector& v) {
    float len = abs(v);
    return Vector(v.x / len, v.y / len, v.z / len);
}

Vector cross(const Vector& u, const Vector& v) {
    return Vector(
        u.y * v.z - u.z * v.y,
        u.z * v.x - u.x * v.z,
        u.x * v.y - u.y * v.x
    );
}

// ---------------- Affine transforms ----------------
Affine translate(const Vector& v) {
    Affine T;
    T[0][3] = v.x;
    T[1][3] = v.y;
    T[2][3] = v.z;
    return T;
}

Affine scale(float r) {
    return scale(r, r, r);
}

Affine scale(float rx, float ry, float rz) {
    Affine S;
    S[0][0] = rx;
    S[1][1] = ry;
    S[2][2] = rz;
    return S;
}

// Rodrigues rotation
Affine rotate(float t, const Vector& v) {
    Vector u = normalize(v);
    float c = cos(t);
    float s = sin(t);

    float x = u.x, y = u.y, z = u.z;

    Affine R;

    R[0][0] = c + x * x * (1 - c);
    R[0][1] = x * y * (1 - c) - z * s;
    R[0][2] = x * z * (1 - c) + y * s;

    R[1][0] = y * x * (1 - c) + z * s;
    R[1][1] = c + y * y * (1 - c);
    R[1][2] = y * z * (1 - c) - x * s;

    R[2][0] = z * x * (1 - c) - y * s;
    R[2][1] = z * y * (1 - c) + x * s;
    R[2][2] = c + z * z * (1 - c);

    return R;
}

// inverse of affine
Affine inverse(const Affine& A) {
    // extract rotation/scale part
    Matrix R;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            R[i][j] = A[i][j];

    // transpose (inverse for rotation part)
    Matrix Rt;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            Rt[i][j] = R[j][i];

    Vector t(A[0][3], A[1][3], A[2][3]);

    // -R^T * t
    Vector newT = Vector(
        -(Rt[0][0] * t.x + Rt[0][1] * t.y + Rt[0][2] * t.z),
        -(Rt[1][0] * t.x + Rt[1][1] * t.y + Rt[1][2] * t.z),
        -(Rt[2][0] * t.x + Rt[2][1] * t.y + Rt[2][2] * t.z)
    );

    Affine inv;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            inv[i][j] = Rt[i][j];

    inv[0][3] = newT.x;
    inv[1][3] = newT.y;
    inv[2][3] = newT.z;

    return inv;
}