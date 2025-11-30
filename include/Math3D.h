#ifndef MATH3D_H
#define MATH3D_H

#include <cmath>
#include <iostream>

const float PI = 3.14159265359f;

//  Vector cơ bản
struct Vec3 {
    float x, y, z;
    Vec3() : x(0), y(0), z(0) {}
    Vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

    Vec3 operator+(const Vec3& v) const { return Vec3(x + v.x, y + v.y, z + v.z); }
    Vec3 operator-(const Vec3& v) const { return Vec3(x - v.x, y - v.y, z - v.z); }
    Vec3 operator*(float s) const { return Vec3(x * s, y * s, z * s); }
    
    // Tích vô hướng (Dot product) -  dùng cho chiếu sáng
    float dot(const Vec3& v) const { return x * v.x + y * v.y + z * v.z; }

    // Tích có hướng (Cross product) - dùng tính pháp tuyến
    Vec3 cross(const Vec3& v) const {
        return Vec3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
    }

    // Chuẩn hóa vector
    Vec3 normalize() {
        float len = sqrt(x * x + y * y + z * z);
        if (len > 0) return Vec3(x / len, y / len, z / len);
        return *this;
    }
};

struct Vec4 { float x, y, z, w; };

//  Ma trận 4x4 cho biến đổi Affine
struct Mat4 {
    float m[4][4];

    Mat4() {
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++) m[i][j] = (i == j) ? 1.0f : 0.0f;
    }

    // Phép nhân ma trận 
    Mat4 operator*(const Mat4& other) const {
        Mat4 res;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                res.m[i][j] = 0;
                for (int k = 0; k < 4; k++)
                    res.m[i][j] += m[i][k] * other.m[k][j];
            }
        }
        return res;
    }

    //Ma trận Tịnh tiến
    static Mat4 translate(const Vec3& v) {
        Mat4 res;
        res.m[3][0] = v.x; res.m[3][1] = v.y; res.m[3][2] = v.z;
        return res;
    }

    //  Ma trận Tỉ lệ
    static Mat4 scale(const Vec3& v) {
        Mat4 res;
        res.m[0][0] = v.x; res.m[1][1] = v.y; res.m[2][2] = v.z;
        return res;
    }

    //  Ma trận Quay quanh trục Y
    static Mat4 rotateY(float angle) {
        Mat4 res;
        float c = cos(angle);
        float s = sin(angle);
        res.m[0][0] = c;  res.m[0][2] = -s;
        res.m[2][0] = s;  res.m[2][2] = c;
        return res;
    }

    //  Phép chiếu phối cảnh (Perspective Projection)
    static Mat4 perspective(float fov, float aspect, float near, float far) {
        Mat4 res;
        float tanHalfFov = tan(fov / 2.0f);
        res.m[0][0] = 0; res.m[1][1] = 0; res.m[2][2] = 0; res.m[3][3] = 0; // Reset identity
        
        res.m[0][0] = 1.0f / (aspect * tanHalfFov);
        res.m[1][1] = 1.0f / tanHalfFov;
        res.m[2][2] = -(far + near) / (far - near);
        res.m[2][3] = -1.0f;
        res.m[3][2] = -(2.0f * far * near) / (far - near);
        return res;
    }

    //  Phép chiếu trực giao (Orthographic) cho Minimap
    static Mat4 ortho(float left, float right, float bottom, float top, float near, float far) {
        Mat4 res;
        res.m[0][0] = 2.0f / (right - left);
        res.m[1][1] = 2.0f / (top - bottom);
        res.m[2][2] = -2.0f / (far - near);
        res.m[3][0] = -(right + left) / (right - left);
        res.m[3][1] = -(top + bottom) / (top - bottom);
        res.m[3][2] = -(far + near) / (far - near);
        return res;
    }

    //  Quan sát đối tượng 3D (LookAt)
    static Mat4 lookAt(Vec3 eye, Vec3 center, Vec3 up) {
        Vec3 f = (center - eye).normalize(); // Forward
        Vec3 u = up.normalize();
        Vec3 s = f.cross(u).normalize();     // Side
        u = s.cross(f);                      // Re-calculate Up

        Mat4 res;
        res.m[0][0] = s.x; res.m[1][0] = s.y; res.m[2][0] = s.z;
        res.m[0][1] = u.x; res.m[1][1] = u.y; res.m[2][1] = u.z;
        res.m[0][2] = -f.x; res.m[1][2] = -f.y; res.m[2][2] = -f.z;
        res.m[3][0] = -s.dot(eye);
        res.m[3][1] = -u.dot(eye);
        res.m[3][2] = f.dot(eye);
        return res;
    }
    
    // Chuyển đổi sang mảng float để gửi xuống Shader
    const float* value_ptr() const { return &m[0][0]; }
};

#endif