#ifndef TERRAIN_H
#define TERRAIN_H

#include <vector>
#include "Math3D.h"

class Terrain {
public:
    int width, height;
    std::vector<float> vertices; // Lưu x, y, z, nx, ny, nz (6 float/vertex)
    std::vector<unsigned int> indices;

    Terrain(int w, int h) : width(w), height(h) {
        generateTerrain();
    }

    // [CG.5 - Slide 25] Tạo lưới đa giác (Polygon Mesh)
    // [CG.pdf - Mục 2] Input là heightMap (ở đây giả lập ngẫu nhiên)
    void generateTerrain() {
        // 1. Tạo đỉnh và độ cao - [CG.pdf - Mục 2] HeightMap
        std::vector<Vec3> tempVertices;
        for (int z = 0; z < height; ++z) {
            for (int x = 0; x < width; ++x) {
                // [CG.pdf] Cải thiện heightMap với nhiều tần số để tạo địa hình tự nhiên hơn
                float fx = (float)x / (float)width;
                float fz = (float)z / (float)height;
                
                // Kết hợp nhiều tần số (multi-octave) để tạo địa hình phức tạp
                float y = 0.0f;
                y += sin(fx * 3.14159f * 2.0f) * cos(fz * 3.14159f * 2.0f) * 3.0f; // Tần số thấp
                y += sin(fx * 3.14159f * 4.0f) * cos(fz * 3.14159f * 4.0f) * 1.5f; // Tần số trung bình
                y += sin(fx * 3.14159f * 8.0f) * cos(fz * 3.14159f * 8.0f) * 0.5f; // Tần số cao (chi tiết)
                y += (sin(x * 0.15f) + cos(z * 0.15f)) * 1.0f; // Thêm biến đổi ngẫu nhiên
                
                tempVertices.push_back(Vec3((float)x, y, (float)z));
            }
        }

        // 2. Tính pháp tuyến (Normals) cho Tô bóng Gouraud [CG.6 - Slide 29]
        // Mảng chứa tổng pháp tuyến cho mỗi đỉnh
        std::vector<Vec3> tempNormals(tempVertices.size(), Vec3(0, 0, 0));

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