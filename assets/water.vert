#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;

out vec3 FragPos;
out vec3 Normal;

void main() {
    // Tạo sóng nước mềm mại
    vec3 pos = aPos;
    pos.y += sin(pos.x * 0.1 + time * 2.0) * 0.08 + cos(pos.z * 0.1 + time * 1.5) * 0.08;
    pos.y += sin(pos.x * 0.15 + pos.z * 0.15 + time * 1.2) * 0.05;
    
    FragPos = vec3(model * vec4(pos, 1.0));
    Normal = vec3(0.0, 1.0, 0.0); // Normal hướng lên
    gl_Position = projection * view * vec4(FragPos, 1.0);
}

