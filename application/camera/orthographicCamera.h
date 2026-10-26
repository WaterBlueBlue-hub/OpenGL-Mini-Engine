#pragma once

#include "camera.h"

class OrthographicCamera : public Camera
{
public:
	/// <summary>
	/// 接受六个参数，用于定义相机的视口边界
	/// </summary>
	OrthographicCamera(float left, float right, float top, float bottom, float near, float far);
	~OrthographicCamera();
	/// <summary>
	/// 计算并返回相机的投影矩阵
	/// </summary>
	mat4 getProjectionMatrix()override;
	/// <summary>
	/// 调整相机的缩放级别
	/// </summary>
	void scale(float deltaScale)override;
private:
	// 定义相机视口的边界
	float mLeft = 0.0f;
	float mRight = 0.0f;
	float mTop = 0.0f;
	float mBottom = 0.0f;
	// 当前的缩放因子，用于调整视口大小
	float mScale{ 0.0f };
};