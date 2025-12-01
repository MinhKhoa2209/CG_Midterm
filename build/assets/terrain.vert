#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform int shadingModel; // 0 = Lambert, 1 = Phong
uniform int displayMode; // 0 = Wireframe, 1 = Flat, 2 = Smooth

out vec3 FragPos;
flat out vec3 FlatNormal; // flat qualifier cho Flat Shading - không nội suy
out vec3 SmoothNormal;    // smooth normal cho Smooth Shading - có nội suy
out vec3 LightDir;
out vec3 ViewDir;
out vec3 LightingColor; // Gouraud shading (Lambert only)

void main() {
    // Tính vị trí đỉnh trong thế giới thực
    FragPos = vec3(model * vec4(aPos, 1.0));
    gl_Position = projection * view * vec4(FragPos, 1.0);

    // Transform normal to world space
    FlatNormal = normalize(aNormal);  // Cho Flat Shading
    SmoothNormal = normalize(aNormal); // Cho Smooth Shading
    LightDir = normalize(lightPos - FragPos);
    ViewDir = normalize(viewPos - FragPos);

    //  Lambert Illumination (Diffuse) - cho Gouraud
    float diff = max(dot(SmoothNormal, LightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Ambient (Giả lập ánh sáng môi trường)
    float ambientStrength = 0.15;
    vec3 ambient = ambientStrength * lightColor;

    // Màu vật thể (đất núi màu xanh lá đậm)
    vec3 objectColor = vec3(0.2, 0.5, 0.2); 

    // Gouraud: chỉ tính Lambert tại vertex
    LightingColor = (ambient + diffuse) * objectColor;
}