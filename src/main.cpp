#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
#include <cmath>

using namespace std;

#include "Math3D.h"
#include "Camera.h"
#include "Terrain.h"
#include "Shader.h"
#include "Algorithms2D.h"

// Cài đặt màn hình
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// Camera - Đặt ở vị trí tốt để nhìn đồi núi giữa biển
Camera camera(Vec3(15.0f, 8.0f, 35.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Minimap settings
const int MINIMAP_SIZE = 200;
vector<Vec3> pathTrace; // Lưu đường đi cho Bresenham

// Lighting settings
Vec3 lightPos(0.0f, 20.0f, 0.0f); // Point Light position (di chuyển được)
int shadingModel = 0; // 0 = Lambert/Gouraud, 1 = Phong

// Display mode settings
enum DisplayMode {
    DISPLAY_WIREFRAME = 0,  // Khung dây (Wireframe)
    DISPLAY_FLAT = 1,        // Tô bóng hằng (Flat Shading)
    DISPLAY_SMOOTH = 2       // Tô bóng Gouraud/Phong (Smooth)
};
DisplayMode displayMode = DISPLAY_SMOOTH; // Mặc định là Smooth

// Callback xử lý chuột
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) { lastX = xpos; lastY = ypos; firstMouse = false; }
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // Đảo ngược y
    lastX = xpos; lastY = ypos;
    camera.processMouseMovement(xoffset, yoffset);
}

// Xử lý bàn phím
void processInput(GLFWwindow* window) {
    static bool escKeyPressed = false;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS && !escKeyPressed) {
        cout << "ESC: Thoat ung dung" << endl;
        escKeyPressed = true;
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_RELEASE) {
        escKeyPressed = false;
    }
    
    // Camera movement
    static bool wKeyPressed = false, sKeyPressed = false, aKeyPressed = false, dKeyPressed = false;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        if (!wKeyPressed) {
            cout << "W: Di chuyen camera tien" << endl;
            wKeyPressed = true;
        }
        camera.processKeyboard(0, deltaTime);
    } else {
        wKeyPressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        if (!sKeyPressed) {
            cout << "S: Di chuyen camera lui" << endl;
            sKeyPressed = true;
        }
        camera.processKeyboard(1, deltaTime);
    } else {
        sKeyPressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        if (!aKeyPressed) {
            cout << "A: Di chuyen camera trai" << endl;
            aKeyPressed = true;
        }
        camera.processKeyboard(2, deltaTime);
    } else {
        aKeyPressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        if (!dKeyPressed) {
            cout << "D: Di chuyen camera phai" << endl;
            dKeyPressed = true;
        }
        camera.processKeyboard(3, deltaTime);
    } else {
        dKeyPressed = false;
    }
    
    // Điều khiển Point Light (I/J/K/L/U/O)
    float lightSpeed = 10.0f * deltaTime;
    static bool iKeyPressed = false, jKeyPressed = false, kKeyPressed = false;
    static bool lKeyPressed = false, uKeyPressed = false, oKeyPressed = false;
    
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
        if (!iKeyPressed) {
            cout << "I: Di chuyen light tien (Z-)" << endl;
            iKeyPressed = true;
        }
        lightPos.z -= lightSpeed;
    } else {
        iKeyPressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
        if (!kKeyPressed) {
            cout << "K: Di chuyen light lui (Z+)" << endl;
            kKeyPressed = true;
        }
        lightPos.z += lightSpeed;
    } else {
        kKeyPressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
        if (!jKeyPressed) {
            cout << "J: Di chuyen light trai (X-)" << endl;
            jKeyPressed = true;
        }
        lightPos.x -= lightSpeed;
    } else {
        jKeyPressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
        if (!lKeyPressed) {
            cout << "L: Di chuyen light phai (X+)" << endl;
            lKeyPressed = true;
        }
        lightPos.x += lightSpeed;
    } else {
        lKeyPressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
        if (!uKeyPressed) {
            cout << "U: Di chuyen light len (Y+)" << endl;
            uKeyPressed = true;
        }
        lightPos.y += lightSpeed;
    } else {
        uKeyPressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
        if (!oKeyPressed) {
            cout << "O: Di chuyen light xuong (Y-)" << endl;
            oKeyPressed = true;
        }
        lightPos.y -= lightSpeed;
    } else {
        oKeyPressed = false;
    }
    
    // Toggle shading model (P key)
    static bool pKeyPressed = false;
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && !pKeyPressed) {
        shadingModel = 1 - shadingModel; // Toggle between 0 and 1
        pKeyPressed = true;
        cout << "Shading Model: " << (shadingModel == 0 ? "Lambert/Gouraud" : "Phong") << endl;
    }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE) {
        pKeyPressed = false;
    }
    
    // Toggle display mode (F key) - Wireframe/Flat/Smooth
    static bool fKeyPressed = false;
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && !fKeyPressed) {
        displayMode = (DisplayMode)((displayMode + 1) % 3);
        fKeyPressed = true;
        const char* modeNames[] = {"Wireframe ", "Flat Shading", "Smooth Shading "};
        cout << "Display Mode: " << modeNames[displayMode] << endl;
    }
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE) {
        fKeyPressed = false;
    }
}

int main() {

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "3D Terrain ", NULL, NULL);
    if (window == NULL) { cout << "Failed to create GLFW window" << endl; glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cout << "Failed to initialize GLAD" << endl; return -1;
    }

    // [CG.6 - Slide 17] Bật Z-Buffer (Depth Test)
    glEnable(GL_DEPTH_TEST);
    // [CG.6 - Slide 11] Bật Back-face Culling (Khử mặt sau)
    glEnable(GL_CULL_FACE);

    // 2. Tạo Shaders
    Shader terrainShader("assets/terrain.vert", "assets/terrain.frag");
    Shader waterShader("assets/water.vert", "assets/water.frag");
    Shader uiShader("assets/ui.vert", "assets/ui.frag");

    // 3. Tạo Địa hình (Modeling) 
    Terrain terrain(50, 50); // Lưới 50x50
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, terrain.vertices.size() * sizeof(float), &terrain.vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, terrain.indices.size() * sizeof(unsigned int), &terrain.indices[0], GL_STATIC_DRAW);

    // Vị trí (Location 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Pháp tuyến (Location 1) [CG.6 - Slide 28]
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Setup cho Water Plane - giới hạn sát terrain
    float waterSize = 26.0f; // Nửa cạnh (52x52, sát với terrain 50x50)
    float waterVertices[] = {
        -waterSize - 1.0f, 0.0f, -waterSize - 1.0f,
         waterSize + 1.0f, 0.0f, -waterSize - 1.0f,
         waterSize + 1.0f, 0.0f,  waterSize + 1.0f,
        -waterSize - 1.0f, 0.0f,  waterSize + 1.0f
    };
    unsigned int waterIndices[] = {
        0, 1, 2,
        2, 3, 0
    };
    unsigned int waterVAO, waterVBO, waterEBO;
    glGenVertexArrays(1, &waterVAO);
    glGenBuffers(1, &waterVBO);
    glGenBuffers(1, &waterEBO);
    glBindVertexArray(waterVAO);
    glBindBuffer(GL_ARRAY_BUFFER, waterVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(waterVertices), waterVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, waterEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(waterIndices), waterIndices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Setup cho Minimap (UI)
    unsigned int uiVAO, uiVBO;
    glGenVertexArrays(1, &uiVAO);
    glGenBuffers(1, &uiVBO);

    // Vòng lặp chính
    Vec3 lastPos = camera.position;
    
    while (!glfwWindowShouldClose(window)) {
        // Tính delta time
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        // --- A. RENDER 3D SCENE ---
        // Xóa màn hình với màu trời xanh
        glClearColor(0.5f, 0.7f, 0.9f, 1.0f); // Màu trời xanh
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Bật blending cho nước trong suốt
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Tính ma trận chung
        Mat4 view = camera.getViewMatrix();
        Mat4 projection = Mat4::perspective(45.0f * PI / 180.0f, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 200.0f);
        
        // --- VẼ NƯỚC TRƯỚC (để terrain vẽ đè lên) ---
        waterShader.use();
        Mat4 waterModel = Mat4::translate(Vec3(-25.0f, 0.0f, -25.0f)); // Đặt nước ở y=0
        waterShader.setMat4("model", waterModel);
        waterShader.setMat4("view", view);
        waterShader.setMat4("projection", projection);
        waterShader.setVec3("viewPos", camera.position);
        waterShader.setVec3("lightPos", lightPos);
        waterShader.setVec3("lightColor", Vec3(1.0f, 1.0f, 1.0f));
        waterShader.setFloat("time", (float)glfwGetTime());
        glBindVertexArray(waterVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        // --- VẼ TERRAIN ---
        terrainShader.use();

        //  Tạo các ma trận biến đổi (Model, View, Projection)
        Mat4 model; // Identity
        // Đặt terrain ở giữa, nổi trên mặt nước
        model = Mat4::translate(Vec3(-25.0f, 0.0f, -25.0f));

        terrainShader.setMat4("model", model);
        terrainShader.setMat4("view", view);
        terrainShader.setMat4("projection", projection);

        // Lighting Properties - Point Light di chuyển được
        terrainShader.setVec3("lightPos", lightPos);
        terrainShader.setVec3("viewPos", camera.position);
        terrainShader.setVec3("lightColor", Vec3(1.0f, 1.0f, 1.0f));
        terrainShader.setInt("shadingModel", shadingModel); // 0 = Lambert/Gouraud, 1 = Phong
        terrainShader.setInt("displayMode", displayMode); // 0 = Wireframe, 1 = Flat, 2 = Smooth

        glBindVertexArray(VAO);
        
        //  Chế độ hiển thị: Wireframe/Flat/Smooth
        if (displayMode == DISPLAY_WIREFRAME) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glLineWidth(4.0f);
        } else {
            // Fill mode cho Flat và Smooth
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        
        // Vẽ lưới tam giác [ - OpenGL Primitives]
        glDrawElements(GL_TRIANGLES, terrain.indices.size(), GL_UNSIGNED_INT, 0);
        
        // Reset về fill mode sau khi vẽ (để không ảnh hưởng đến minimap)
        if (displayMode == DISPLAY_WIREFRAME) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        glDisable(GL_BLEND); // Tắt blending sau khi vẽ nước

        // --- E. MINIMAP & UI (2D) ---
        //  Phép chiếu trực giao cho Minimap
        // Cập nhật đường đi: Áp dụng Bresenham mỗi khi di chuyển đáng kể
        if (abs(camera.position.x - lastPos.x) > 0.5 || abs(camera.position.z - lastPos.z) > 0.5) {
            // Map tọa độ 3D (x, z) sang 2D minimap (0-200)
            // Địa hình rộng 50x50 map vào 200x200 pixel, offset để đặt ở góc dưới bên phải
            int x1 = (int)((lastPos.x + 25) * 4);
            int y1 = (int)((lastPos.z + 25) * 4);
            int x2 = (int)((camera.position.x + 25) * 4);
            int y2 = (int)((camera.position.z + 25) * 4);
            
            //  Xén hình Cohen-Sutherland trước khi vẽ
            double dx1=x1, dy1=y1, dx2=x2, dy2=y2;
            bool visible = Algorithms2D::cohenSutherlandClip(dx1, dy1, dx2, dy2, 0, MINIMAP_SIZE, 0, MINIMAP_SIZE);
            
            if (visible) {
                // Bresenham để lấy các điểm ảnh
                vector<Vec3> newPoints = Algorithms2D::bresenhamLine((int)dx1, (int)dy1, (int)dx2, (int)dy2);
                pathTrace.insert(pathTrace.end(), newPoints.begin(), newPoints.end());
            }
            lastPos = camera.position;
        }

        // Vẽ Minimap & HUD - Tắt Depth Test để vẽ UI đè lên trên
        glDisable(GL_DEPTH_TEST); 
        uiShader.use();
        Mat4 ortho = Mat4::ortho(0.0f, (float)SCR_WIDTH, 0.0f, (float)SCR_HEIGHT, -1.0f, 1.0f);
        uiShader.setMat4("projection", ortho);
        
        glBindVertexArray(uiVAO);
        glBindBuffer(GL_ARRAY_BUFFER, uiVBO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        
        // Vị trí minimap: góc dưới bên phải
        float minimapX = SCR_WIDTH - MINIMAP_SIZE - 20.0f;
        float minimapY = 20.0f;
        
        // 1. Vẽ khung minimap (4 đường thẳng)
        vector<Vec3> frame;
        frame.push_back(Vec3(minimapX, minimapY, 0.0f));
        frame.push_back(Vec3(minimapX + MINIMAP_SIZE, minimapY, 0.0f));
        frame.push_back(Vec3(minimapX + MINIMAP_SIZE, minimapY, 0.0f));
        frame.push_back(Vec3(minimapX + MINIMAP_SIZE, minimapY + MINIMAP_SIZE, 0.0f));
        frame.push_back(Vec3(minimapX + MINIMAP_SIZE, minimapY + MINIMAP_SIZE, 0.0f));
        frame.push_back(Vec3(minimapX, minimapY + MINIMAP_SIZE, 0.0f));
        frame.push_back(Vec3(minimapX, minimapY + MINIMAP_SIZE, 0.0f));
        frame.push_back(Vec3(minimapX, minimapY, 0.0f));
        glBufferData(GL_ARRAY_BUFFER, frame.size() * sizeof(Vec3), &frame[0], GL_STATIC_DRAW);
        uiShader.setVec3("color", Vec3(1.0f, 1.0f, 1.0f)); // Màu trắng cho khung
        glLineWidth(2.0f);
        glDrawArrays(GL_LINES, 0, frame.size());
        
        // 2. Vẽ đường đi (path trace) - offset vào trong khung
        if (!pathTrace.empty()) {
            vector<Vec3> offsetPath;
            for (const auto& pt : pathTrace) {
                // Map từ (0-200) sang vị trí minimap
                float x = minimapX + pt.x;
                float y = minimapY + pt.y;
                offsetPath.push_back(Vec3(x, y, 0.0f));
            }
            glBufferData(GL_ARRAY_BUFFER, offsetPath.size() * sizeof(Vec3), &offsetPath[0], GL_DYNAMIC_DRAW);
            uiShader.setVec3("color", Vec3(1.0f, 0.0f, 0.0f)); // Màu đỏ cho đường đi
            glPointSize(2.0f);
            glDrawArrays(GL_POINTS, 0, offsetPath.size());
        }
        
        // 3. Vẽ marker cho vị trí camera hiện tại
        float camX = minimapX + (camera.position.x + 25.0f) * 4.0f;
        float camY = minimapY + (camera.position.z + 25.0f) * 4.0f;
        vector<Vec3> cameraMarker;
        // Vẽ hình tròn nhỏ (8 điểm)
        for (int i = 0; i < 8; ++i) {
            float angle = i * 2.0f * PI / 8.0f;
            float x = camX + cos(angle) * 3.0f;
            float y = camY + sin(angle) * 3.0f;
            cameraMarker.push_back(Vec3(x, y, 0.0f));
        }
        glBufferData(GL_ARRAY_BUFFER, cameraMarker.size() * sizeof(Vec3), &cameraMarker[0], GL_STATIC_DRAW);
        uiShader.setVec3("color", Vec3(0.0f, 1.0f, 0.0f)); // Màu xanh lá cho camera
        glPointSize(4.0f);
        glDrawArrays(GL_POINTS, 0, cameraMarker.size());
        
        // Vẽ hướng camera (mũi tên)
        Vec3 front2D = Vec3(camera.front.x, 0.0f, camera.front.z).normalize();
        vector<Vec3> directionArrow;
        directionArrow.push_back(Vec3(camX, camY, 0.0f));
        directionArrow.push_back(Vec3(camX + front2D.x * 8.0f, camY + front2D.z * 8.0f, 0.0f));
        glBufferData(GL_ARRAY_BUFFER, directionArrow.size() * sizeof(Vec3), &directionArrow[0], GL_STATIC_DRAW);
        uiShader.setVec3("color", Vec3(0.0f, 1.0f, 1.0f)); // Màu cyan cho hướng
        glLineWidth(2.0f);
        glDrawArrays(GL_LINES, 0, directionArrow.size());

        // Reset state
        glEnable(GL_DEPTH_TEST);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &waterVAO);
    glDeleteBuffers(1, &waterVBO);
    glDeleteBuffers(1, &waterEBO);
    glDeleteVertexArrays(1, &uiVAO);
    glDeleteBuffers(1, &uiVBO);
    glfwTerminate();
    return 0;
}