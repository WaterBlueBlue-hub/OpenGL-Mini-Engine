#pragma once
#include "material.h"

class WhiteMaterial : public Material
{
public:
	WhiteMaterial();
	~WhiteMaterial();
public:
	vec3 mColor{ 0.58f, 0.58f, 0.58f };
};
