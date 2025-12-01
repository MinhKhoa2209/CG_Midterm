#ifndef TERRAIN_H
#define TERRAIN_H

#include <vector>
using namespace std;

#include "Math3D.h"

class Terrain {
public:
    int width, height;
    vector<float> vertices; // Lưu x, y, z, nx, ny, nz (6 float/vertex)
    vector<unsigned int> indices;

    Terrain(int w, int h) : width(w), height(h) {
        generateTerrain();
    }

    //  Tạo lưới đa giác (Polygon Mesh) - Đồi núi tròn, đỉnh mượt, đặt giữa biển
    void generateTerrain() {
        // 1. Tạo đỉnh và độ cao - HeightMap
        vector<Vec3> tempVertices;
        
        // Tâm của đồi núi (giữa terrain)
        float centerX = (float)width / 2.0f;
        float centerZ = (float)height / 2.0f;
        float maxRadius = sqrt(centerX * centerX + centerZ * centerZ);
        
        for (int z = 0; z < height; ++z) {
            for (int x = 0; x < width; ++x) {
                float fx = (float)x;
                float fz = (float)z;
                
                // Tính khoảng cách từ tâm (tạo hình đồi tròn)
                float dx = fx - centerX;
                float dz = fz - centerZ;
                float dist = sqrt(dx * dx + dz * dz);
                float normalizedDist = dist / maxRadius; // 0 ở tâm, 1 ở biên
                
                // Tạo đồi núi với đỉnh bo tròn mượt (không nhọn)
                float y = 0.0f;
                float hillHeight = 16.0f;
                float hillRadius = maxRadius * 0.65f; // Bán kính đồi
                
                if (dist < hillRadius) {
                    // Dùng smoothstep để tạo đỉnh tròn mượt, không nhọn
                    float t = dist / hillRadius; // 0 ở tâm, 1 ở rìa đồi
                    // Smoothstep tạo đường cong mượt: 0 -> 1 mượt mà, không có góc nhọn
                    float smoothFactor = t * t * (3.0f - 2.0f * t); // Smoothstep function
                    y += hillHeight * (1.0f - smoothFactor); // Đỉnh cao ở tâm, thấp dần ra ngoài
                }
                
                // Thêm các đỉnh núi phụ với đỉnh tròn
                float noise1 = sin(fx * 0.25f) * cos(fz * 0.25f) * 1.8f;
                float noise2 = sin(fx * 0.5f) * cos(fz * 0.5f) * 0.9f;
                float noise3 = sin(fx * 1.0f) * cos(fz * 1.0f) * 0.4f;
                
                // Chỉ thêm noise ở vùng đồi (không thêm ở biên)
                if (normalizedDist < 0.75f) {
                    float noiseFactor = 1.0f - (normalizedDist / 0.75f);
                    // Smoothstep cho noise để đỉnh phụ cũng tròn
                    noiseFactor = noiseFactor * noiseFactor * (3.0f - 2.0f * noiseFactor);
                    y += (noise1 + noise2 + noise3) * noiseFactor;
                }
                
                // Đảm bảo không có vùng âm (nổi trên mặt nước)
                y = max(y, 0.8f);
                
                tempVertices.push_back(Vec3(fx, y, fz));
            }
        }

        // 2. Tính pháp tuyến (Normals) cho Tô bóng Gouraud [CG.6 - Slide 29]
        // Mảng chứa tổng pháp tuyến cho mỗi đỉnh
        vector<Vec3> tempNormals(tempVertices.size(), Vec3(0, 0, 0));

        // Duyệt qua từng ô lưới (mỗi ô là 2 tam giác)
        for (int z = 0; z < height - 1; ++z) {
            for (int x = 0; x < width - 1; ++x) {
                // 4 đỉnh của ô
                int i0 = z * width + x;
                int i1 = z * width + (x + 1);
                int i2 = (z + 1) * width + x;
                int i3 = (z + 1) * width + (x + 1);

                // Tam giác 1: i0, i2, i1
                Vec3 v1 = tempVertices[i2] - tempVertices[i0];
                Vec3 v2 = tempVertices[i1] - tempVertices[i0];
                Vec3 normal1 = v1.cross(v2).normalize(); // [CG.6 - Slide 8] Tích có hướng

                // Cộng dồn pháp tuyến vào các đỉnh tham gia (Gouraud Average)
                tempNormals[i0] = tempNormals[i0] + normal1;
                tempNormals[i2] = tempNormals[i2] + normal1;
                tempNormals[i1] = tempNormals[i1] + normal1;

                // Tam giác 2: i1, i2, i3
                Vec3 v3 = tempVertices[i2] - tempVertices[i1];
                Vec3 v4 = tempVertices[i3] - tempVertices[i1];
                Vec3 normal2 = v3.cross(v4).normalize();

                tempNormals[i1] = tempNormals[i1] + normal2;
                tempNormals[i2] = tempNormals[i2] + normal2;
                tempNormals[i3] = tempNormals[i3] + normal2;

                // Lưu indices cho EBO
                indices.push_back(i0); indices.push_back(i2); indices.push_back(i1);
                indices.push_back(i1); indices.push_back(i2); indices.push_back(i3);
            }
        }

        // 3. Đóng gói dữ liệu (Vị trí + Pháp tuyến đã chuẩn hóa)
        for (size_t i = 0; i < tempVertices.size(); ++i) {
            vertices.push_back(tempVertices[i].x);
            vertices.push_back(tempVertices[i].y);
            vertices.push_back(tempVertices[i].z);
            
            Vec3 n = tempNormals[i].normalize();
            vertices.push_back(n.x);
            vertices.push_back(n.y);
            vertices.push_back(n.z);
        }
    }
};

#endif