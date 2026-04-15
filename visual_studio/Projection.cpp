/******************************************************************************
 * File: Projection.cpp
 * Author: Junyoung Ki
 * Course: CS200
 * Description: Implementation of camera transformation matrices.
 *****************************************************************************/
#include "Projection.h"

 /**
  * @brief Constructs a transformation matrix from Camera Space to World Space.
  * @param cam The camera object containing orientation and position data.
  * @return An Affine matrix representing the camera's basis vectors and eye position in world space.
  */
Affine CameraToWorld(const Camera& cam) {
    return Affine(cam.Right(), cam.Up(), cam.Back(), cam.Eye());
}

/**
 * @brief Constructs a transformation matrix from World Space to Camera Space (View Matrix).
 * @param cam The camera object.
 * @return The inverse of the Camera-to-World matrix, used to transform world coordinates into the camera's local frame.
 */
Affine WorldToCamera(const Camera& cam) {
    return inverse(CameraToWorld(cam));
}

/**
 * @brief Constructs the Perspective Projection matrix to transform Camera Space to NDC.
 * @param cam The camera object containing viewport and frustum properties.
 * @return A 4x4 Matrix that projects 3D camera coordinates into a 4D homogeneous clip space for NDC conversion.
 */
Matrix CameraToNDC(const Camera& cam) {
    Vector viewport = cam.ViewportGeometry();
    float W = viewport.x;
    float H = viewport.y;
    float D = viewport.z;
    float N = cam.NearDistance();
    float F = cam.FarDistance();

    Matrix m;
    m[0][0] = (2.0f * D) / W;
    m[1][1] = (2.0f * D) / H;
    m[2][2] = (F + N) / (N - F);
    m[2][3] = (2.0f * F * N) / (N - F);
    m[3][2] = -1.0f;
    m[3][3] = 0.0f;
    return m;
}