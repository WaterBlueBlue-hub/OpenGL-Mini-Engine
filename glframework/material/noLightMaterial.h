#pragma once
#include "material.h"
#include "../texture.h"

class NoLightMaterial : public Material
{
public:
	NoLightMaterial();
	~NoLightMaterial();
public:
	Texture* mDiffuseCube{ nullptr };
	Texture* mDiffuseSphere{ nullptr };
};