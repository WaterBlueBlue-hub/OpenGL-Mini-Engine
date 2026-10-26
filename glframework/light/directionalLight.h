#pragma once
#include "light.h"

class DirectionalLight : public Light
{
public:
	DirectionalLight();
	~DirectionalLight();
public:
	vec3 mDirection{ -1.0 };
};