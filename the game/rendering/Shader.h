#pragma once
#include <string>
#include <unordered_map>
#include <glm/ext/matrix_float4x4.hpp>

struct ShaderProgramSource {
	std::string VertexSource;
	std::string FragmentSource;
};

class Shader
{
public:
	Shader() = default;
	
	~Shader();
	void Init(const std::string& filepath);
	void Bind() const;
	void Unbind() const;

	//set Uniforms
	void SetUniform1f(const std::string& name, float value);
	void SetUniform4f(const std::string& name, float v0, float v1, float f2, float f3);
	void SetUniform1i(const std::string& name, int value);
	void SetUniformMat4f(const std::string& name, const glm::mat4& matrix);
	void SetUniform1iv(const std::string& name, const int* value, const int count);
private:
	std::string m_FilePath;
	unsigned int m_RendererID;
	std::unordered_map<std::string, int> m_UniformLocationCache;
	//caching for uniforms
	int GetUniformLocation(const std::string& name);
	unsigned int CompileShader(const std::string& source, unsigned int type);
	unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
	ShaderProgramSource ParseShader(const std::string& filepath);
};