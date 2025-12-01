#version 330 core
in vec3 FragPos;
in vec3 Normal;

uniform vec3 viewPos;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform float time;

out vec4 FragColor;

void main() {
    // Màu nước biển
    vec3 waterColor = vec3(0.1, 0.32, 0.6);    // Xanh dương đậm
    vec3 shallowColor = vec3(0.45, 0.70, 0.95); // Xanh nhạt (rìa, gần bờ)
    
    // Tính khoảng cách tới tâm water plane
    float waterCenterX = 0.0;
    float waterCenterZ = 0.0;
    float maxR = 26.0 + 1.0; // bán kính lớn nhất của water plane
    float d = length(vec2(FragPos.x - waterCenterX, FragPos.z - waterCenterZ));
    float edge = smoothstep(0.75*maxR, maxR, d); // 0 ở tâm, 1 tại rìa
    
    // Blend màu nước theo vị trí với tâm
    vec3 baseColor = mix(waterColor, shallowColor, edge);
    
    // Thêm hiệu ứng sóng mềm
    float wave = sin((FragPos.x + time * 6.0) * 0.09 + cos(FragPos.z - time * 3.4) * 0.13) * 0.08;
    wave += sin(FragPos.x * 0.11 + FragPos.z * 0.15 + time * 0.7) * 0.09;
    wave += 0.04 * sin((FragPos.x + FragPos.z) * 0.17 - time);
    
    // Sáng dần ở rìa nước
    baseColor += wave * (0.7 - 0.6*edge);

    // Lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64.0);
    vec3 ambient = baseColor * 0.3;
    vec3 diffuse = baseColor * diff * 0.69;
    vec3 specular = lightColor * spec * 0.41;
    vec3 result = ambient + diffuse + specular;

    // Alpha tự nhiên: rìa mờ, giữa đậm
    float myAlpha = mix(0.45, 0.80, 1.0-edge);
    myAlpha *= 0.94 + 0.04*sin(time+FragPos.x*0.12 + FragPos.z*0.16);
    FragColor = vec4(result, myAlpha);
}

