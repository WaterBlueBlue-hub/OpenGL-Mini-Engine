#pragma once
#include "../core.h"

class Tools
{
public:
	/// <summary>
	/// 传入一个矩阵，解构位置、旋转（欧拉角）、缩放信息
	/// </summary>
	static void decompose(mat4 matrix, vec3& position, vec3& eulerAngle, vec3& scale);
};