#pragma once
#include "material.h"
#include "../texture.h"

class PhongMaterial : public Material
{
public:
	PhongMaterial();
	~PhongMaterial();
public:
	Texture* mDiffuse{ nullptr };
	Texture* mSpecularMask{ nullptr }; // 高光蒙版
	Texture* mOpacityMask{ nullptr }; // 透明度蒙版
	Texture* mEnv{ nullptr }; // 环境贴图
	float mShiness{ 1.0f }; // 高光集中度
};