#pragma once
#include "material.h"
#include "../texture.h"

class ScreenMaterial :public Material 
{
public:
	ScreenMaterial();
	~ScreenMaterial();
public:
	Texture* mScreenTexture{ nullptr };
	float mTextureWidth{ 1600.0f };
	float mTextureHeight{ 900.0f };
};
