#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;

#include "Math3D.h"

class Shader {
public:
    unsigned int ID;

    Shader(const char* vertexPath, const char* fragmentPath) {
        ID = 0;
        // 1. Retrieve code from file
        string vertexCode;
        string fragmentCode;
        ifstream vShaderFile;
        ifstream fShaderFile;
        
        vShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
        fShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
        try {
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            if (!vShaderFile.is_open()) {
                cout << "ERROR::SHADER::FILE_NOT_FOUND: " << vertexPath << endl;
                return;
            }
            if (!fShaderFile.is_open()) {
                cout << "ERROR::SHADER::FILE_NOT_FOUND: " << fragmentPath << endl;
                return;
            }
            stringstream vShaderStream, fShaderStream;
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            vShaderFile.close();
            fShaderFile.close();
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
            
            if (vertexCode.empty()) {
                cout << "ERROR::SHADER::VERTEX_SHADER_IS_EMPTY: " << vertexPath << endl;
                return;
            }
            if (fragmentCode.empty()) {
                cout << "ERROR::SHADER::FRAGMENT_SHADER_IS_EMPTY: " << fragmentPath << endl;
                return;
            }
        } catch (ifstream::failure& e) {
            cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << endl;
            cout << "  Vertex path: " << vertexPath << endl;
            cout << "  Fragment path: " << fragmentPath << endl;
            cout << "  Error: " << e.what() << endl;
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
    
    void setMat4(const string &name, const Mat4 &mat) const {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, mat.value_ptr());
    }
    
    void setVec3(const string &name, const Vec3 &value) const {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z);
    }
    
    void setInt(const string &name, int value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
    
    void setFloat(const string &name, float value) const {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }

private:
    bool checkCompileErrors(unsigned int shader, string type) {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM") {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << endl;
                return false;
            }
        } else {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << endl;
                return false;
            }
        }
        return true;
    }
};
#endif