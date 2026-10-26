#pragma once

#include "camera.h"

using namespace glm;

class PerspectiveCamera : public Camera
{
public:
	PerspectiveCamera(float fovy, float aspect, float near, float far);
	~PerspectiveCamera();
	/// <summary>
	/// 计算并返回相机的投影矩阵
	/// </summary>
	mat4 getProjectionMatrix()override;
	void setAspect(float aspect) { mAspect = aspect; }
	/// <summary>
	/// 通过缩放模拟相机的前后移动
	/// </summary>
	void scale(float deltaScale)override;
private:
	float mFovy = 0.0f;
	float mAspect = 0.0f;
};
