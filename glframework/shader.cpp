#include "shader.h"
#include "../wrapper/checkError.h"
#include<string>
// 提供了用于文件输入输出的功能
#include<fstream>
// 提供了用于字符串流处理的功能
#include<sstream>
#include<iostream>

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
	// 声明装入shader代码字符串的两个string
	string vertexCode;
	string fragmentCode;
	// 声明用于读取vs跟fs文件的inFileStream
	ifstream vShaderFile;
	ifstream fShaderFile;
	// 保证ifstream遇到问题的时候可以抛出异常
	vShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
	fShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
	try
	{
		// 打开文件
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		// 将文件输入流当中的字符串输入到stringStream里面
		stringstream vShaderStream, fShaderStream;
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// 关闭文件
		vShaderFile.close();
		fShaderFile.close();
		// 将字符串从stringStream中读取出来，转化到code String当中
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (ifstream::failure& e)
	{
		cout << "Shader文件错误:" << e.what() << endl;
	}
	const char* vertexShaderSource = vertexCode.c_str();
	const char* fragmentShaderSource = fragmentCode.c_str();
	// 创建Shader程序
	GLuint vertex, fragment;
	vertex = glCreateShader(GL_VERTEX_SHADER);
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	// 输入Shader代码
	glShaderSource(vertex, 1, &vertexShaderSource, NULL);
	glShaderSource(fragment, 1, &fragmentShaderSource, NULL);
	// 执行Shader代码编译，并检查编译结果
	glCompileShader(vertex);
	checkShaderErrors(vertex, "编译");
	glCompileShader(fragment);
	checkShaderErrors(fragment, "编译");
	// 创建Program
	mProgram = glCreateProgram();
	// 将vs与fs编译好的结果放到Program
	glAttachShader(mProgram, vertex);
	glAttachShader(mProgram, fragment);
	// 执行Program的链接操作，形成最终可执行Shader程序
	glLinkProgram(mProgram);
	// 检查链接错误
	checkShaderErrors(mProgram, "链接");
	// 清理
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

Shader::~Shader()
{
	if (mProgram != 0)
		glDeleteProgram(mProgram);
}

void Shader::begin()
{
	// 设置绘制所使用的shader程序
	GL_CALL(glUseProgram(mProgram));
}

void Shader::end()
{
	GL_CALL(glUseProgram(0));
}
#pragma region uniform变量设置函数
void Shader::setFloat(const string& name, float value)
{
	//  通过名称拿到Uniform变量的位置Location
	GLint location = GL_CALL(glGetUniformLocation(mProgram, name.c_str()));
	// 通过Location更新Uniform变量的值
	GL_CALL(glUniform1f(location, value));
}

void Shader::setVector3(const string& name, float x, float y, float z)
{
	GLint location = GL_CALL(glGetUniformLocation(mProgram, name.c_str()));
	GL_CALL(glUniform3f(location, x, y, z));
}

void Shader::setVector3(const string& name, const float* values)
{
	GLint location = GL_CALL(glGetUniformLocation(mProgram, name.c_str()));
	GL_CALL(glUniform3fv(location, 1, values));
}

void Shader::setVector3(const string& name, const vec3 value) {
	GLint location = GL_CALL(glGetUniformLocation(mProgram, name.c_str()));
	GL_CALL(glUniform3f(location, value.x, value.y, value.z));
}

void Shader::setInt(const string& name, int value)
{
	GLint location = GL_CALL(glGetUniformLocation(mProgram, name.c_str()));
	GL_CALL(glUniform1i(location, value));
}

void Shader::setMatrix4x4(const string& name, mat4 value)
{
	GLint location = GL_CALL(glGetUniformLocation(mProgram, name.c_str()));
	GL_CALL(glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(value)));
}

void Shader::setMatrix4x4Array(const string& name, mat4* value, int count)
{
	GLint location = GL_CALL(glGetUniformLocation(mProgram, name.c_str()));
	GL_CALL(glUniformMatrix4fv(location, count, GL_FALSE, value_ptr(value[0])));
}

void Shader::setMatrix3x3(const string& name, mat3 value)
{
	GLint location = GL_CALL(glGetUniformLocation(mProgram, name.c_str()));
	GL_CALL(glUniformMatrix3fv(location, 1, GL_FALSE, value_ptr(value)));
}

void Shader::setBool(const string& name, bool value)
{
	GLint location = GL_CALL(glGetUniformLocation(mProgram, name.c_str()));
	GL_CALL(glUniform1i(location, value));
}
#pragma endregion
void Shader::checkShaderErrors(GLuint target, string type)
{
	int success = 0;
	// 存储错误信息
	char infoLog[1024];
	if (type == "编译")
	{
		glGetShaderiv(target, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(target, 1024, NULL, infoLog);
			cout << "错误: Shader编译错误" << "\n" << infoLog << endl;
		}
	}
	else if (type == "链接")
	{
		glGetProgramiv(target, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(target, 1024, NULL, infoLog);
			cout << "错误: Shader链接错误" << "\n" << infoLog << endl;
		}
	}
	else
	{
		cout << "错误:检查shader错误类型错误" << endl;
	}
}
