#pragma once
#include "material.h"
#include "../texture.h"

class BlinnPhongMaterial : public Material
{
public:
	BlinnPhongMaterial();
	~BlinnPhongMaterial();
public:
	Texture* mDiffuse{ nullptr };
	Texture* mSpecularMask{ nullptr }; // 高光蒙版
	float mShiness{ 1.0f }; // 高光集中度
};