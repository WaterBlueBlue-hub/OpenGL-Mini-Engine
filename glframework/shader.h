#pragma once

#include "core.h"
#include<string>

class Shader 
{
public:
	Shader(const char* vertexPath, const char* fragmentPath);
	~Shader();
	/// <summary>
	/// 激活着色器程序
	/// </summary>
	void begin();
	/// <summary>
	/// 结束着色器程序
	/// </summary>
	void end();
#pragma region uniform变量设置函数
	void setFloat(const string& name, float value);
	void setVector3(const string& name, float x, float y, float z);
	void setVector3(const string& name, const float* values);
	void setVector3(const string& name, const vec3 value);
	void setInt(const string& name, int value);
	void setMatrix4x4(const string& name, mat4 value);
	void setMatrix4x4Array(const string& name, mat4* value, int count);
	void setMatrix3x3(const string& name, mat3 value);
	void setBool(const string& name, bool value);
#pragma endregion
private:
	/// <summary>
	/// 检查着色器编译和链接错误
	/// </summary>
	/// <param name="target">目标ID</param>
	/// <param name="type">错误类型</param>
	void checkShaderErrors(GLuint target, string type);
private:
	GLuint mProgram{ 0 }; // 存储着色器程序的ID
};