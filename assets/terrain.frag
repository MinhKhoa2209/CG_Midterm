#version 330 core
in vec3 FragPos;
flat in vec3 FlatNormal;  // flat qualifier - không nội suy (cho Flat Shading)
in vec3 SmoothNormal;     // smooth - có nội suy (cho Smooth Shading)
in vec3 LightDir;
in vec3 ViewDir;
in vec3 LightingColor;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform int shadingModel; // 0 = Lambert (Gouraud), 1 = Phong
uniform int displayMode; // 0 = Wireframe, 1 = Flat, 2 = Smooth

out vec4 FragColor;

void main() {
    vec3 objectColor = vec3(0.2, 0.5, 0.2);
    
    if (displayMode == 1) {
        // [CG.6] Flat Shading - Tô bóng hằng (không nội suy)
        // Sử dụng FlatNormal (flat qualifier) - tất cả fragment trong tam giác dùng cùng normal
        vec3 norm = normalize(FlatNormal);
        vec3 lightDir = normalize(lightPos - FragPos);
        
        // Ambient
        float ambientStrength = 0.15;
        vec3 ambient = ambientStrength * lightColor;
        
        // Diffuse (Lambert) - mỗi tam giác có một màu đồng nhất
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * lightColor;
        
        // Flat shading chỉ dùng ambient + diffuse, không có specular
        vec3 result = (ambient + diffuse) * objectColor;
        FragColor = vec4(result, 1.0);
        return;
    }
    
    if (shadingModel == 0) {
        // [CG.6] Gouraud Shading - màu được nội suy từ đỉnh (Lambert)
        FragColor = vec4(LightingColor, 1.0);
    } else {
        // [CG.6] Phong Shading Model - tính toán tại fragment
        vec3 norm = normalize(SmoothNormal); // Sử dụng smooth normal cho Smooth Shading
        vec3 lightDir = normalize(lightPos - FragPos);
        vec3 viewDir = normalize(viewPos - FragPos);
        
        // Ambient
        float ambientStrength = 0.15;
        vec3 ambient = ambientStrength * lightColor;
        
        // Diffuse (Lambert)
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * lightColor;
        
        // Specular (Phong) - [CG.6] Độ bóng
        float specularStrength = 0.5;
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0); // Shininess = 32
        vec3 specular = specularStrength * spec * lightColor;
        
        // Tổng hợp ánh sáng Phong
        vec3 result = (ambient + diffuse + specular) * objectColor;
        FragColor = vec4(result, 1.0);
    }
}