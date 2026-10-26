#pragma once
#include "../core.h"

enum class MaterialType
{
	NoLightMaterial, 
	PhongMaterial, 
	BlinnPhongMaterial, 
	WhiteMaterial,
	ScreenMaterial,
	DepthMaterial,
	GrassInstanceMaterial
};

class Material
{
public:
	Material();
	~Material();
public:
	MaterialType mType;
	// 深度缓存相关
	bool mDepthTest{ true };
	GLenum mDepthFunc{ GL_LEQUAL };
	bool mDepthWrite{ true };
	// 多边形偏移相关（解决Z_Fighting现象）
	bool mPolygonOffset{ false };
	unsigned int mPolygonOffsetType{ GL_POLYGON_OFFSET_FILL };
	float mFactor{ 0.0f };
	float mUnit{ 0.0f };
	// 模板缓存相关
	bool mStencilTest{ false };
	unsigned int mSFail{ GL_KEEP }; // 模板测试失败
	unsigned int mZFail{ GL_KEEP };	// 深度测试失败
	unsigned int mZPass{ GL_KEEP }; // 模板、深度测试都通过
	unsigned int mStencilMask{ 0xFF }; // 控制模板写入
	unsigned int mStencilFunc{ GL_ALWAYS };
	unsigned int mStencilRef{ 0 };
	unsigned int mStencilFuncMask{ 0xFF };
	// 颜色混合相关
	bool mBlend{ false };
	unsigned int mSFactor{ GL_SRC_ALPHA };
	unsigned int mDFactor{ GL_ONE_MINUS_SRC_ALPHA };
	float mOpacity{ 1.0f };
	// 面剔除相关
	bool mFaceCulling{ false };
	unsigned int mFrontFace{ GL_CCW };
	unsigned int mCullFace{ GL_BACK };
};