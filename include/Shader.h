#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "Math3D.h"

class Shader {
public:
    unsigned int ID;

    Shader(const char* vertexPath, const char* fragmentPath) {
        ID = 0;
        // 1. Retrieve code from file
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try {
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            if (!vShaderFile.is_open()) {
                std::cout << "ERROR::SHADER::FILE_NOT_FOUND: " << vertexPath << std::endl;
                return;
            }
            if (!fShaderFile.is_open()) {
                std::cout << "ERROR::SHADER::FILE_NOT_FOUND: " << fragmentPath << std::endl;
                return;
            }
            std::stringstream vShaderStream, fShaderStream;
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            vShaderFile.close();
            fShaderFile.close();
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
            
            if (vertexCode.empty()) {
                std::cout << "ERROR::SHADER::VERTEX_SHADER_IS_EMPTY: " << vertexPath << std::endl;
                return;
            }
            if (fragmentCode.empty()) {
                std::cout << "ERROR::SHADER::FRAGMENT_SHADER_IS_EMPTY: " << fragmentPath << std::endl;
                return;
            }
        } catch (std::ifstream::failure& e) {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
            std::cout << "  Vertex path: " << vertexPath << std::endl;
            std::cout << "  Fragment path: " << fragmentPath << std::endl;
            std::cout << "  Error: " << e.what() << std::endl;
            return;
        }
        const char* vShaderCode = vertexCode.c_str();
        const char * fShaderCode = fragmentCode.c_str();
        
        // 2. Compile
        unsigned int vertex, fragment;
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        if (!checkCompileErrors(vertex, "VERTEX")) {
            glDeleteShader(vertex);
            return;
        }
        
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        if (!checkCompileErrors(fragment, "FRAGMENT")) {
            glDeleteShader(vertex);
            glDeleteShader(fragment);
            return;
        }
        
        // Shader Program
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        if (!checkCompileErrors(ID, "PROGRAM")) {
            glDeleteShader(vertex);
            glDeleteShader(fragment);
            glDeleteProgram(ID);
            ID = 0;
            return;
        }
        
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }
    
    void use() { glUseProgram(ID); }
    
    void setMat4(const std::string &name, const Mat4 &mat) const {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, mat.value_ptr());
    }
    
    void setVec3(const std::string &name, const Vec3 &value) const {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z);
    }
    
    void setInt(const std::string &name, int value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }

private:
    bool checkCompileErrors(unsigned int shader, std::string type) {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM") {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << std::endl;
                return false;
            }
        } else {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << std::endl;
                return false;
            }
        }
        return true;
    }
};
#endif