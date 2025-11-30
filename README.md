# 3D Terrain

## 1. Giới thiệu
- Ứng dụng mô phỏng địa hình 3D 
- Thể hiện lưới đa giác (polygon mesh) với các kỹ thuật tô bóng tiêu chuẩn (Gouraud/Lambert, Phong, Flat), hỗ trợ chuyển đổi trực tiếp giữa các chế độ hiển thị.
- Cung cấp minimap (UI 2D) thể hiện đường đi của camera, khả năng khám phá/quản lý địa hình trực quan.

## 2. Cấu trúc thư mục
```
├── assets/            # Chứa các file shader (GLSL)
├── build/             # Tạo tự động (output binary, không có source code chính)
├── include/           # Header file chia module: Math3D, Terrain, Camera, Algorithm...
│   ├── GLFW/          # GLFW header
│   └── glad/          # GLAD header
├── lib/               # Thư viện ngoài (GLFW)
├── src/               # Code nguồn C++ chính (main.cpp, glad.c)
├── CMakeLists.txt     # Build project
└── README.md          # File mô tả dự án
```

## 3. Hướng dẫn Build và Chạy
- **Yêu cầu:**
    - Windows, CMake >= 3.10
    - Compiler: MinGW hoặc MSYS2 + MinGW-w64
    - Thư viện: GLFW3 (lib/ và include/ sẵn)
    - GLAD: `src/glad.c` và `include/glad/glad.h`
- **Build:**
```bash
cmake -S . -B build -G "MinGW Makefiles"
cd build
mingw32-make
```
- **Chạy:**
```bash
./3DTerrain.exe
```

## 4. Điều khiển (Controls)
- **Camera:**
    - W/A/S/D: Tiến/Lùi/Trái/Phải
    - Mouse: Quay góc nhìn
    - ESC: Thoát
- **Nguồn sáng (Point Light):**
    - I/K: Di chuyển sáng theo trục Z (trước/sau)
    - J/L: Di chuyển sang trái/phải theo trục X
    - U/O: Lên/xuống theo trục Y
- **Chuyển đổi shading:**
    - P: Lambert/Gouraud (mặc định) ↔ Phong
- **Chuyển đổi chế độ hiển thị:**
    - F: Wireframe ⇄ Flat Shading ⇄ Smooth Shading

## 5. Tính năng nổi bật
- **Địa hình mô hình lưới đa giác 50x50:** tạo bởi heightmap multi-octave.
- **Phong Shading và Flat Shading (full GLSL):** chuyển đổi trực tiếp, quan sát ảnh hưởng tới bề mặt mesh rõ ràng.
- **Back-face Culling, Z-Buffer:** tối ưu hiệu năng, loại bỏ mặt sau/bị khuất.
- **Điều khiển Point Light động:** có thể “di chuyển mặt trời” quan sát hiệu ứng chiều sáng và bóng.
- **Minimap 2D trực tiếp:**
    - Vẽ đường đi qua Bresenham/Cohen-Sutherland.
    - Marker camera (vòng tròn xanh), mũi tên chỉ hướng camera, khung minimap rõ ràng.
- **Hiển thị Wireframe:** thấy cấu trúc mesh và từng tam giác tạo nên địa hình.

## 6. Kỹ thuật đồ họa đã áp dụng
- **Polygon Mesh Model**: Địa hình cấu trúc từ lưới tam giác (vertex/indices).
- **Rendering Pipeline:**
    - *Phong Shading*: Ánh sáng từng pixel (GLSL, specular highlight).
    - *Gouraud/Lambert*: Ánh sáng từng đỉnh, nội suy màu qua tam giác.
    - *Flat Shading*: Mỗi mặt một màu (GLSL `flat`).
- **Ánh sáng - Lighting:**
    - Thành phần *Ambient*, *Diffuse* (Lambert), *Specular* (Phong).
    - Điều khiển nguồn sáng động (Point Light), thử nghiệm hiệu ứng chiếu sáng động.
- **Các phép biến đổi ma trận:**
    - Ma trận Model/View/Projection custom thuần C++ (Mat4), phép chiếu phối cảnh (Perspective), trực giao (Ortho).
- **Camera/View:**
    - Camera điều khiển theo góc nhìn (LookAt) bằng chuột, phím, tạo cảm giác tương tác thực.
- **Culling & Z-Buffer:**
    - *Back-face Culling*: Loại các mặt sau camera.
    - *Z-Buffer*: Đảm bảo object gần che object xa.
- **Wireframe Mode:** Xem trực tiếp lưới mesh với `glPolygonMode` và line width tùy chỉnh.
- **Minimap & 2D Algorithms:**
    - *Bresenham*: Vẽ đường đi camera trên minimap.
    - *Cohen-Sutherland*: Clip đường đi ra khung minimap.
    - Vẽ marker, mũi tên hướng nhìn camera, khung minimap dùng primitive OpenGL.
- **Procedural Terrain:**
    - Heightmap địa hình tạo động, phối nhiều hàm sóng, chi tiết tự nhiên.
- **UI Overlay:**
    - Vẽ 2D UI clear trên nền 3D thực, phân lớp không gian renderer.

