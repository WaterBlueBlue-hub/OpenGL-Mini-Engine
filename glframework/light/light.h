#pragma once
#include "../core.h"

class Light
{
public:
	Light();
	~Light();
public:
	vec3 mColor{ 1.0f }; // 光照强度
	float mSpecularIntensity{ 1.0f }; // 镜面反射强度增强系数;
	float mIntensity{ 1.0f };
};