#pragma once
#include"core.h"
#include <string>

class Texture 
{
public:
	/// <summary>
	/// 从文件路径加载纹理
	/// </summary>
	static Texture* createTexture(const string& path, unsigned int unit);
	/// <summary>
	/// 从内存中的数据创建纹理
	/// </summary>
	static Texture* createTextureFromMemory(
		const string& path, 
		unsigned int unit,
		unsigned char* dataIn,
		uint32_t widthIn,
		uint32_t heightIn
	);
	/// <summary>
	/// 创建颜色附件
	/// </summary>
	static Texture* createColorAttachment(
		unsigned int width,
		unsigned int height,
		unsigned int unit
	);
	/// <summary>
	/// 创建深度模板附件
	/// </summary>
	static Texture* createDepthStencilAttachment(
		unsigned int width,
		unsigned int height,
		unsigned int unit
	);
	Texture();
	Texture(const string& path, unsigned int unit);
	Texture(
		unsigned int unit,
		unsigned char* dataIn,
		uint32_t widthIn,
		uint32_t heightIn
	);
	Texture(unsigned int width, unsigned int height, unsigned int unit);
	Texture(const vector<string>& paths, unsigned int unit);
	~Texture();
	/// <summary>
	/// 激活纹理单元并绑定当前纹理对象
	/// </summary>
	void bind();
	void setUnit(unsigned int unit) { mUnit = unit; }
	int getWidth()const { return mWidth; }
	int getHeight()const { return mHeight; }
	GLuint getTexture()const { return mTexture; }
private:
	GLuint mTexture{ 0 };
	int mWidth{ 0 };
	int mHeight{ 0 };
	unsigned int mUnit{ 0 };
	unsigned int mTextureTarget{ GL_TEXTURE_2D };
	static map<string, Texture*> mTextureCache;
};