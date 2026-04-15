#include "RasterUtilities.h"
#include <cmath>

static float getMin(float a, float b, float c) {
    float m = (a < b) ? a : b;
    return (m < c) ? m : c;
}

static float getMax(float a, float b, float c) {
    float m = (a > b) ? a : b;
    return (m > c) ? m : c;
}

void ClearBuffers(Raster& r, float z) {
    int width = r.Width();
    int height = r.Height();

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            r.GotoPoint(x, y);
            r.WritePixel();
            r.WriteZ(z);
        }
    }
}

void FillTriangle(Raster& r, const Hcoord& P, const Hcoord& Q, const Hcoord& R) {
    float px = P.x / P.w, py = P.y / P.w, pz = P.z / P.w;
    float qx = Q.x / Q.w, qy = Q.y / Q.w, qz = Q.z / Q.w;
    float rx = R.x / R.w, ry = R.y / R.w, rz = R.z / R.w;

    int minX = static_cast<int>(std::floor(getMin(px, qx, rx)));
    int maxX = static_cast<int>(std::ceil(getMax(px, qx, rx)));
    int minY = static_cast<int>(std::floor(getMin(py, qy, ry)));
    int maxY = static_cast<int>(std::ceil(getMax(py, qy, ry)));

    if (minX < 0) minX = 0;
    if (maxX > r.Width() - 1) maxX = r.Width() - 1;
    if (minY < 0) minY = 0;
    if (maxY > r.Height() - 1) maxY = r.Height() - 1;

    if (minX > maxX || minY > maxY) return;

    float A0 = py - qy, B0 = qx - px, C0 = px * qy - qx * py;
    if (A0 * rx + B0 * ry + C0 < 0) { A0 = -A0; B0 = -B0; C0 = -C0; }

    float A1 = qy - ry, B1 = rx - qx, C1 = qx * ry - rx * qy;
    if (A1 * px + B1 * py + C1 < 0) { A1 = -A1; B1 = -B1; C1 = -C1; }

    float A2 = ry - py, B2 = px - rx, C2 = rx * py - px * ry;
    if (A2 * qx + B2 * qy + C2 < 0) { A2 = -A2; B2 = -B2; C2 = -C2; }

    float v1x = qx - px, v1y = qy - py, v1z = qz - pz;
    float v2x = rx - px, v2y = ry - py, v2z = rz - pz;

    float Nx = v1y * v2z - v1z * v2y;
    float Ny = v1z * v2x - v1x * v2z;
    float Nz = v1x * v2y - v1y * v2x;

    if (Nz == 0.0f) return;

    float Dx = -Nx / Nz;
    float Dy = -Ny / Nz;
    float D0 = pz + (Nx * px + Ny * py) / Nz;

    for (int y = minY; y <= maxY; ++y) {
        for (int x = minX; x <= maxX; ++x) {
            float E0 = A0 * x + B0 * y + C0;
            float E1 = A1 * x + B1 * y + C1;
            float E2 = A2 * x + B2 * y + C2;

            if (E0 >= 0 && E1 >= 0 && E2 >= 0) {
                float Z = Dx * x + Dy * y + D0;

                r.GotoPoint(x, y);
                if (Z < r.GetZ()) {
                    r.WriteZ(Z);
                    r.WritePixel();
                }
            }
        }
    }
}