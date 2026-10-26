#pragma once

#include "../../glframework/core.h"

class Camera
{
public:
	Camera();
	/// <summary>
	/// 初始化相机的位置、上方向和右方向
	/// </summary>
	Camera(vec3 position, vec3 up,vec3 right);
	~Camera();
	/// <summary>
	/// 计算并返回相机的视图矩阵
	/// </summary>
	mat4 getViewMatrix();
	/// <summary>
	/// 计算并返回投影矩阵
	/// </summary>
	virtual mat4 getProjectionMatrix();
	/// <summary>
	/// 相机缩放函数
	/// </summary>
	virtual void scale(float deltaScale);
public:
	vec3 mPosition{ 0.0f, 1.0f, 2.0f };
	vec3 mUp{ 0.0f, 1.0f, 0.0f };
	vec3 mRight{ 1.0f, 0.0f, 0.0f };
	// 用于计算线性深度缓存
	float mNear = 0.0f;
	float mFar = 0.0f;
};