/*!
\file      Camera.cpp
\par       CS250 Camera Frustum Programming Assignment
\author    Junyoung Ki
\date      2026-03-26
\par       Course: CS250
\copyright Copyright (C) 2026 DigiPen Institute of Technology
*/

#include <cmath>
#include "Affine.h"
#include "Camera.h"

Vector SafeNormalize(const Vector& v) {
    float magSq = v.x * v.x + v.y * v.y + v.z * v.z;
    if (magSq < 1e-10f) return Vector(0, 0, 0);
    float invMag = 1.0f / std::sqrt(magSq);
    return Vector(v.x * invMag, v.y * invMag, v.z * invMag);
}

Vector SafeCross(const Vector& a, const Vector& b) {
    return Vector(a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x);
}

Camera::Camera(void) : eye(0, 0, 0), right(1, 0, 0), up(0, 1, 0), back(0, 0, 1),
near(0.1f), far(1000.0f), distance(0.1f) {
    width = 2.0f * distance * std::tan(45.0f * 3.14159265f / 360.0f);
    height = width;
}

Camera::Camera(const Point& E, const Vector& look, const Vector& rel,
    float fov, float aspect, float n, float f)
    : eye(E), near(n), far(f), distance(n) {
    back = SafeNormalize(Vector(-look.x, -look.y, -look.z));
    right = SafeNormalize(SafeCross(rel, back));
    up = SafeCross(back, right);

    width = 2.0f * distance * std::tan(fov / 2.0f);
    height = width / aspect;
}


Camera& Camera::Yaw(float angle) {
    Affine R = rotate(angle, up);
    right = R * right;
    back = R * back;
    return *this;
}

Camera& Camera::Pitch(float angle) {
    Affine R = rotate(angle, right);
    up = R * up;
    back = R * back;
    return *this;
}

Camera& Camera::Roll(float angle) {
    Affine R = rotate(angle, back);
    right = R * right;
    up = R * up;
    return *this;
}

// Getter
Point Camera::Eye(void) const { return eye; }
Vector Camera::Right(void) const { return right; }
Vector Camera::Up(void) const { return up; }
Vector Camera::Back(void) const { return back; }
float Camera::NearDistance(void) const { return near; }
float Camera::FarDistance(void) const { return far; }
Vector Camera::ViewportGeometry(void) const { return Vector(width, height, distance); }
Camera& Camera::Zoom(float factor) { width *= factor; height *= factor; return *this; }
Camera& Camera::Forward(float dist) { eye = eye - dist * back; return *this; }