#include "checkError.h"
#include <iostream>

void CheckError()
{
	GLenum errorCode = glGetError();
	string error = " ";
	if (errorCode != GL_NO_ERROR)
	{
		switch (errorCode)
		{
		case GL_INVALID_ENUM: error = "枚举错误"; break;
		case GL_INVALID_VALUE: error = "值错误"; break;
		case GL_INVALID_OPERATION: error = "操作错误"; break;
		case GL_OUT_OF_MEMORY: error = "内存错误";  break;
		default:
			error = "其它错误"; break;
		}
		cout << error << endl;
		// 根据传入的bool值，决定程序是否停止
		assert(false);
	}
}