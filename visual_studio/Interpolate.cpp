/*
 * File:        Interpolate.cpp
 * Description: Implementation of perspective-correct texture mapping
 * and polygon clipping for textured triangles using Barycentric Coordinates.
 */

#include "Interpolate.h"
#include <algorithm>
#include <cmath>

float TexturedCoord::color_scale = 1.0f;
Texture* TexturedCoord::texture = nullptr;

// ============================================================================
// 1. Scan Converting & Perspective-Correct Interpolation (Barycentric)
// ============================================================================
void FillTriangle(Raster& r, const TexturedCoord& P, const TexturedCoord& Q, const TexturedCoord& R) {

    struct Vertex { float x, y, z, invW, uw, vw; };

    // ★ 핵심 해결책: NDC 변환 후 Viewport(모니터 크기)로 뻥튀기하는 과정 추가!
    auto ToScreen = [&](const TexturedCoord& v) -> Vertex {
        // 1. 원근 나눗셈 (NDC: -1.0 ~ 1.0 사이로 변환)
        float ndcX = v.x / v.w;
        float ndcY = v.y / v.w;
        float ndcZ = v.z / v.w;

        // 2. 뷰포트 변환 (화면 픽셀: 0 ~ Width, 0 ~ Height 로 늘리기)
        // 화면 Y축이 위로 갈수록 커지는지, 아래로 갈수록 커지는지에 따라 Y 반전
        float screenX = (ndcX + 1.0f) * 0.5f * (float)r.Width();
        float screenY = (1.0f - ndcY) * 0.5f * (float)r.Height(); // 대부분의 윈도우 그래픽스는 Y가 반전됨

        return { screenX, screenY, ndcZ, 1.0f / v.w, v.u / v.w, v.v / v.w };
        };

    Vertex V0 = ToScreen(P);
    Vertex V1 = ToScreen(Q);
    Vertex V2 = ToScreen(R);

    // 삼각형 넓이 계산 (0이면 그리지 않음)
    float denom = (V1.x - V0.x) * (V2.y - V0.y) - (V1.y - V0.y) * (V2.x - V0.x);
    if (std::abs(denom) < 1e-5f) return;

    // 모니터 밖으로 나가지 않도록 바운딩 박스 설정
    int minX = std::max(0, static_cast<int>(std::floor(std::min({ V0.x, V1.x, V2.x }))));
    int maxX = std::min((int)r.Width() - 1, static_cast<int>(std::ceil(std::max({ V0.x, V1.x, V2.x }))));
    int minY = std::max(0, static_cast<int>(std::floor(std::min({ V0.y, V1.y, V2.y }))));
    int maxY = std::min((int)r.Height() - 1, static_cast<int>(std::ceil(std::max({ V0.y, V1.y, V2.y }))));

    if (minX > maxX || minY > maxY) return;

    for (int y = minY; y <= maxY; ++y) {
        float py = (float)y + 0.5f;
        for (int x = minX; x <= maxX; ++x) {
            float px = (float)x + 0.5f;

            float E0 = (V2.x - V1.x) * (py - V1.y) - (V2.y - V1.y) * (px - V1.x);
            float E1 = (V0.x - V2.x) * (py - V2.y) - (V0.y - V2.y) * (px - V2.x);
            float E2 = (V1.x - V0.x) * (py - V0.y) - (V1.y - V0.y) * (px - V0.x);

            bool inside = (denom > 0) ? (E0 >= 0 && E1 >= 0 && E2 >= 0)
                : (E0 <= 0 && E1 <= 0 && E2 <= 0);

            if (inside) {
                r.GotoPoint(x, y);

                float alpha = E0 / denom;
                float beta = E1 / denom;
                float gamma = E2 / denom;

                // 과제 요구사항: Z값 선형 보간
                float Z = alpha * V0.z + beta * V1.z + gamma * V2.z;

                // [체크포인트 1] 혹시 배경색에 가려진다면 여기 부등호를 (Z > r.GetZ())로 바꿔보세요.
                if (Z < r.GetZ()) {

                    // 과제 요구사항: u, v 원근 보정 보간
                    float invW = alpha * V0.invW + beta * V1.invW + gamma * V2.invW;
                    float w = 1.0f / invW;
                    float u = (alpha * V0.uw + beta * V1.uw + gamma * V2.uw) * w;
                    float v = (alpha * V0.vw + beta * V1.vw + gamma * V2.vw) * w;

                    Vector texColor = TexturedCoord::texture->uvToRGB(u, v);
                    float sr = texColor.x * TexturedCoord::color_scale;
                    float sg = texColor.y * TexturedCoord::color_scale;
                    float sb = texColor.z * TexturedCoord::color_scale;

                    sr = std::max(0.0f, std::min(255.0f, sr));
                    sg = std::max(0.0f, std::min(255.0f, sg));
                    sb = std::max(0.0f, std::min(255.0f, sb));

                    // [체크포인트 2] 텍스처 문제인지 확인하는 디버그 용도:
                    // 만약 이 코드를 돌렸는데 아무것도 안 뜬다면, 아래 주석을 풀고 실행해보세요!
                    // (텍스처를 무시하고 100% 새빨간 삼각형으로 화면에 그려버리는 강제 코드입니다.)
                    // sr = 255.0f; sg = 0.0f; sb = 0.0f;

                    r.SetColor(static_cast<Raster::byte>(sr),
                        static_cast<Raster::byte>(sg),
                        static_cast<Raster::byte>(sb));
                    r.WriteZ(Z);
                    r.WritePixel();
                }
            }
        }
    }
}

// ============================================================================
// 2. Polygon Clipping (Sutherland-Hodgman Algorithm)
// ============================================================================
bool TextureClip::operator()(std::vector<TexturedCoord>& vertices) {
    if (half_spaces.empty()) return !vertices.empty();

    for (size_t h_idx = 0; h_idx < half_spaces.size(); ++h_idx) {
        const HalfSpace& h = half_spaces[h_idx];
        if (vertices.empty()) return false;

        temp_vertices.clear();
        size_t n = vertices.size();

        for (size_t i = 0; i < n; ++i) {
            const TexturedCoord& A = vertices[i];
            const TexturedCoord& B = vertices[(i + 1) % n];

            float dA = h.x * A.x + h.y * A.y + h.z * A.z + h.w * A.w;
            float dB = h.x * B.x + h.y * B.y + h.z * B.z + h.w * B.w;

            if (dA >= 0.0f) {
                temp_vertices.push_back(A);
                if (dB < 0.0f) {
                    float t = dA / (dA - dB);
                    TexturedCoord I;
                    I.x = A.x + t * (B.x - A.x); I.y = A.y + t * (B.y - A.y);
                    I.z = A.z + t * (B.z - A.z); I.w = A.w + t * (B.w - A.w);
                    I.u = A.u + t * (B.u - A.u); I.v = A.v + t * (B.v - A.v);
                    temp_vertices.push_back(I);
                }
            }
            else {
                if (dB >= 0.0f) {
                    float t = dA / (dA - dB);
                    TexturedCoord I;
                    I.x = A.x + t * (B.x - A.x); I.y = A.y + t * (B.y - A.y);
                    I.z = A.z + t * (B.z - A.z); I.w = A.w + t * (B.w - A.w);
                    I.u = A.u + t * (B.u - A.u); I.v = A.v + t * (B.v - A.v);
                    temp_vertices.push_back(I);
                }
            }
        }
        vertices = temp_vertices;
    }
    return !vertices.empty();
}