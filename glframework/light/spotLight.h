#pragma once
#include "light.h"
#include "../object.h"

class SpotLight : public Light, public Object
{
public:
	SpotLight();
	~SpotLight();
public:
	vec3 mTargetDirection{ -1.0f };
	float mInnerAngle{ 0.0f }; // 绝对可视区
	float mOuterAngle{ 0.0f }; // 可视区（绝对可视区 + 边缘模糊区）
};