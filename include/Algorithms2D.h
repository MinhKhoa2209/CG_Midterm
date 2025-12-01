#ifndef ALGORITHMS2D_H
#define ALGORITHMS2D_H

#include "Math3D.h"
#include <vector>
using namespace std;

// Định nghĩa mã vùng cho Cohen-Sutherland 
const int INSIDE = 0; // 0000
const int LEFT = 1;   // 0001
const int RIGHT = 2;  // 0010
const int BOTTOM = 4; // 0100
const int TOP = 8;    // 1000

class Algorithms2D {
public:
    //  Thuật toán Bresenham vẽ đoạn thẳng
    // Trả về danh sách các điểm (pixel) để vẽ lên Minimap
    static vector<Vec3> bresenhamLine(int x1, int y1, int x2, int y2) {
        vector<Vec3> points;
        int dx = abs(x2 - x1);
        int dy = abs(y2 - y1);
        int sx = (x1 < x2) ? 1 : -1;
        int sy = (y1 < y2) ? 1 : -1;
        int err = dx - dy;

        while (true) {
            // Chuẩn hóa tọa độ pixel về không gian NDC (-1 đến 1) hoặc giữ raw pixel tuỳ shader
            // Ở đây ta lưu tọa độ pixel thô, shader Minimap sẽ dùng Ortho matrix để map
            points.push_back(Vec3((float)x1, (float)y1, 0.0f));

            if (x1 == x2 && y1 == y2) break;
            int e2 = 2 * err;
            if (e2 > -dy) { err -= dy; x1 += sx; }
            if (e2 < dx) { err += dx; y1 += sy; }
        }
        return points;
    }

    // Hàm phụ trợ tính OutCode cho Cohen-Sutherland 
    static int computeOutCode(double x, double y, double xmin, double xmax, double ymin, double ymax) {
        int code = INSIDE;
        if (x < xmin) code |= LEFT;
        else if (x > xmax) code |= RIGHT;
        if (y < ymin) code |= BOTTOM;
        else if (y > ymax) code |= TOP;
        return code;
    }

    //  Thuật toán Xén hình Cohen-Sutherland
    // Cắt đường đi của camera nếu nó ra khỏi khung Minimap
    static bool cohenSutherlandClip(double& x0, double& y0, double& x1, double& y1, 
                                    double xmin, double xmax, double ymin, double ymax) {
        int outcode0 = computeOutCode(x0, y0, xmin, xmax, ymin, ymax);
        int outcode1 = computeOutCode(x1, y1, xmin, xmax, ymin, ymax);
        bool accept = false;

        while (true) {
            if (!(outcode0 | outcode1)) { // Cả hai đều trong vùng
                accept = true;
                break;
            } else if (outcode0 & outcode1) { // Cả hai cùng nằm ngoài 1 phía
                break;
            } else {
                double x, y;
                int outcodeOut = outcode0 ? outcode0 : outcode1;

                // Tìm giao điểm 
                if (outcodeOut & TOP) {
                    x = x0 + (x1 - x0) * (ymax - y0) / (y1 - y0);
                    y = ymax;
                } else if (outcodeOut & BOTTOM) {
                    x = x0 + (x1 - x0) * (ymin - y0) / (y1 - y0);
                    y = ymin;
                } else if (outcodeOut & RIGHT) {
                    y = y0 + (y1 - y0) * (xmax - x0) / (x1 - x0);
                    x = xmax;
                } else if (outcodeOut & LEFT) {
                    y = y0 + (y1 - y0) * (xmin - x0) / (x1 - x0);
                    x = xmin;
                }

                if (outcodeOut == outcode0) {
                    x0 = x; y0 = y;
                    outcode0 = computeOutCode(x0, y0, xmin, xmax, ymin, ymax);
                } else {
                    x1 = x; y1 = y;
                    outcode1 = computeOutCode(x1, y1, xmin, xmax, ymin, ymax);
                }
            }
        }
        return accept;
    }
};

#endif