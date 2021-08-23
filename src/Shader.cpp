#include "Shader.h"
#include "glad/glad.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include "glm/glm.hpp"

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath) {
    m_vertexPath = vertexPath;
    m_fragmentPath = fragmentPath;
    std::string vertexCode;
    std::string fragmentCode;
    load(vertexCode, fragmentCode);
    this->m_vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    this->m_fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
    this->m_geoShaderId = 0;
    this->ID = glCreateProgram();
    compile(vertexCode.c_str(), fragmentCode.c_str());
}

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geoPath) {
    m_vertexPath = vertexPath;
    m_fragmentPath = fragmentPath;
    m_geoPath = geoPath;
    std::string vertexCode;
    std::string fragmentCode;
    std::string geoCode;
    load(vertexCode, fragmentCode, geoCode);
    this->m_vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    this->m_fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
    this->m_geoShaderId = glCreateShader(GL_GEOMETRY_SHADER);
    this->ID = glCreateProgram();
    compile(vertexCode.c_str(), fragmentCode.c_str(), geoCode.c_str());
}

void Shader::load(std::string& vertex, std::string& fragement)
{
    std::ifstream shaderFile;
    shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // open files
        shaderFile.open(m_vertexPath);
        std::stringstream stream;
        stream << shaderFile.rdbuf();
        shaderFile.close();
        vertex = stream.str();
        stream.str(std::string());

        shaderFile.open(m_fragmentPath);
        stream << shaderFile.rdbuf();
        shaderFile.close();
        fragement = stream.str();
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
    }
}
void Shader::load(std::string& vertex, std::string& fragement, std::string& geo)
{
    std::ifstream shaderFile;
    shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // open files
        shaderFile.open(m_vertexPath);
        std::stringstream stream;
        stream << shaderFile.rdbuf();
        shaderFile.close();
        vertex = stream.str();
        stream.str(std::string());

        shaderFile.open(m_fragmentPath);
        stream << shaderFile.rdbuf();
        shaderFile.close();
        fragement = stream.str();
        stream.str(std::string());

        shaderFile.open(m_geoPath);
        stream << shaderFile.rdbuf();
        shaderFile.close();
        geo = stream.str();
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
    }
}

void Shader::compile(const char *vCode, const char *fCode) {
    glShaderSource(this->m_vertexShaderId, 1, &vCode, nullptr);
    glCompileShader(this->m_vertexShaderId);

    glShaderSource(this->m_fragmentShaderId, 1, &fCode, nullptr);
    glCompileShader(this->m_fragmentShaderId);

    glAttachShader(this->ID, this->m_vertexShaderId);
    glAttachShader(this->ID, this->m_fragmentShaderId);
    glLinkProgram(this->ID);
    int success;
    char infoLog[1024];
    glGetShaderiv(this->m_vertexShaderId, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(this->m_vertexShaderId, 1024, NULL, infoLog);
        std::cout << "ERROR::SHADER_COMPILATION_ERROR::VERTEX " << infoLog << std::endl;
    }
    glGetShaderiv(this->m_fragmentShaderId, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(this->m_fragmentShaderId, 1024, NULL, infoLog);
        std::cout << "ERROR::SHADER_COMPILATION_ERROR::FRAGEMENT " << infoLog << std::endl;
    }
    glGetProgramiv(this->ID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(this->ID, 1024, NULL, infoLog);
        std::cout << "ERROR::PROGRAM_LINKING_ERROR " << infoLog << std::endl;
    }
    glDeleteShader(this->m_vertexShaderId);
    glDeleteShader(this->m_fragmentShaderId);
}

void Shader::compile(const char *vCode, const char *fCode, const char* gCode) {
    glShaderSource(this->m_vertexShaderId, 1, &vCode, nullptr);
    glCompileShader(this->m_vertexShaderId);

    glShaderSource(this->m_fragmentShaderId, 1, &fCode, nullptr);
    glCompileShader(this->m_fragmentShaderId);
	
    glShaderSource(this->m_geoShaderId, 1, &gCode, nullptr);
    glCompileShader(this->m_geoShaderId);

    glAttachShader(this->ID, this->m_vertexShaderId);
    glAttachShader(this->ID, this->m_fragmentShaderId);
    glAttachShader(this->ID, this->m_geoShaderId);
    glLinkProgram(this->ID);
    int success;
    char infoLog[1024];
    glGetShaderiv(this->m_vertexShaderId, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(this->m_vertexShaderId, 1024, NULL, infoLog);
        std::cout << "ERROR::SHADER_COMPILATION_ERROR::VERTEX " << infoLog << std::endl;
    }
    glGetShaderiv(this->m_fragmentShaderId, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(this->m_fragmentShaderId, 1024, NULL, infoLog);
        std::cout << "ERROR::SHADER_COMPILATION_ERROR::FRAGEMENT " << infoLog << std::endl;
    }
    glGetShaderiv(this->m_geoShaderId, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(this->m_geoShaderId, 1024, NULL, infoLog);
        std::cout << "ERROR::SHADER_COMPILATION_ERROR::GEOMETRY " << infoLog << std::endl;
    }
    glGetProgramiv(this->ID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(this->ID, 1024, NULL, infoLog);
        std::cout << "ERROR::PROGRAM_LINKING_ERROR " << infoLog << std::endl;
    }
    glDeleteShader(this->m_vertexShaderId);
    glDeleteShader(this->m_fragmentShaderId);
    glDeleteShader(this->m_geoShaderId);
}

void Shader::Reload() {
    std::string vertexCode;
    std::string fragmentCode;
	if(m_geoShaderId == 0)
	{
        load(vertexCode, fragmentCode);
        compile(vertexCode.c_str(), fragmentCode.c_str());
	}
    else
    {
        std::string geoCode;
        load(vertexCode, fragmentCode, geoCode);
        compile(vertexCode.c_str(), fragmentCode.c_str(), geoCode.c_str());
    }
    std::cout << "INFO::SHADER::RELOADED" << std::endl;
}

void Shader::Use()
{
    glUseProgram(ID);
}

void Shader::SetMVP(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection){
    set("model", model);
    set("view", view);
    set("projection", projection);
}

void Shader::set(const std::string name, bool value) {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::set(const std::string name, int value) {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::set(const std::string name, float value) {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::set(const std::string name, const glm::vec2& value) {
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::set(const std::string name, float x, float y) {
    glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}

void Shader::set(const std::string name, const glm::vec3& value) {
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::set(const std::string name, float x, float y, float z) {
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}

void Shader::set(const std::string name, const glm::vec4& value) {
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::set(const std::string name, float x, float y, float z, float w) {
    glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}

void Shader::set(const std::string name, const glm::mat2& mat){
    glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::set(const std::string name, const glm::mat3& mat){
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::set(const std::string name, const glm::mat4& mat){
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}