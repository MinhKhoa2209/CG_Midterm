#ifndef CAMERA_H
#define CAMERA_H

#include "Math3D.h"

class Camera {
public:
    Vec3 position;
    Vec3 front;
    Vec3 up;
    float yaw, pitch;

    Camera(Vec3 startPos) : position(startPos), front(0.0f, 0.0f, -1.0f), up(0.0f, 1.0f, 0.0f), yaw(-90.0f), pitch(0.0f) {}

    Mat4 getViewMatrix() {
        return Mat4::lookAt(position, position + front, up);
    }

    // Xử lý input bàn phím (Tịnh tiến) [CG.pdf - Mục 3.B]
    void processKeyboard(int direction, float deltaTime) {
        float velocity = 5.0f * deltaTime;
        Vec3 right = front.cross(up).normalize();
        
        if (direction == 0) position = position + front * velocity; // W
        if (direction == 1) position = position - front * velocity; // S
        if (direction == 2) position = position - right * velocity; // A
        if (direction == 3) position = position + right * velocity; // D
        // Giữ camera trên mặt đất một chút nếu muốn (đơn giản hóa)
    }

    // Xử lý chuột (Quay) [CG.pdf - Mục 3.B]
    void processMouseMovement(float xoffset, float yoffset) {
        xoffset *= 0.1f;
        yoffset *= 0.1f;

        yaw += xoffset;
        pitch += yoffset;

        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;

        updateCameraVectors();
    }

private:
    void updateCameraVectors() {
        Vec3 frontNew;
        frontNew.x = cos(yaw * PI / 180.0f) * cos(pitch * PI / 180.0f);
        frontNew.y = sin(pitch * PI / 180.0f);
        frontNew.z = sin(yaw * PI / 180.0f) * cos(pitch * PI / 180.0f);
        front = frontNew.normalize();
    }
};

#endif