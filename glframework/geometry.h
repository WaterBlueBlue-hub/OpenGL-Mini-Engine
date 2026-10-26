#pragma once

#include "core.h"
#include <vector>
struct Color {
	uint8_t r, g, b, a;
};

class Geometry
{
public:
	Geometry();
	// 用于读取模型
	Geometry(
		const vector<float>& positions,
		const vector<float>& normals,
		const vector<float>& uvs,
		const vector<unsigned int>& indices
		);
	Geometry(
		const vector<float>& positions,
		const vector<float>& normals,
		const vector<float>& uvs,
		const vector<float>& colors,
		const vector<unsigned int>& indices
	);
	/// <summary>
	/// 清理与OpenGL相关的资源，包括VAO、VBO和EBO
	/// </summary>
	~Geometry();
#pragma region 几何体创建函数
	/// <summary>
	/// 创建立方体
	/// </summary>
	static Geometry* createBox(float size);
	/// <summary>
	/// 创建球体
	/// </summary>
	static Geometry* createSphere(float radius);
	/// <summary>
	/// 创建三角形
	/// </summary>
	static Geometry* createTriangle(float size);
	/// <summary>
	/// 创建平面
	/// </summary>
	static Geometry* createPlane(float Lenth, float Width, vec3 normal = vec3(0.0f, 1.0f, 0.0f));
	/// <summary>
	/// 创建屏幕
	/// </summary>
	static Geometry* createScreenPlane();
	/// <summary>
	/// 创建直线
	/// </summary>
	static Geometry* createBresenhamLine(int x0, int y0, int x1, int y1);
	/// <summary>
	/// 创建贝塞尔曲线
	/// </summary>
	static Geometry* createBezier(vector<vec3> ctrlPoints);
#pragma endregion
#pragma region 可重用函数
	/// <summary>
	/// 创建缓冲
	/// </summary>
	/// <param name="buffer">用于接收生成的缓冲对象ID</param>
	/// <param name="target">生成的目标缓冲对象</param>
	/// <param name="data">要传递给缓冲对象的数据</param>
	/// <param name="dataSize">数据的大小</param>
	/// <param name="usage">缓冲数据的使用方式</param>
	static void createBuffer(GLuint& buffer, GLenum target, const void* data, 
		size_t dataSize, GLenum usage = GL_STATIC_DRAW) {
		glGenBuffers(1, &buffer);               // 生成缓冲对象
		glBindBuffer(target, buffer);           // 绑定指定类型的缓冲对象
		glBufferData(target, dataSize, data, usage);  // 填充数据
	}
	/// <summary>
	/// 设置顶点属性指针
	/// </summary>
	/// <param name="vbo">传入的顶点缓冲对象的ID</param>
	/// <param name="index">顶点属性的索引</param>
	/// <param name="size">每个顶点属性的组件数量</param>
	/// <param name="stride">顶点之间的步长</param>
	/// <param name="type">数据类型</param>
	/// <param name="normalized">是否需要将数据归一化</param>
	/// <param name="pointer">指向数据的偏移指针</param>
	static void setupVertexAttribPointer(GLuint vbo, GLuint index, GLint size, 
		GLsizei stride, GLenum type = GL_FLOAT, GLboolean normalized = GL_FALSE, const void* pointer = (void*)0) {
		glBindBuffer(GL_ARRAY_BUFFER, vbo);                        // 绑定 VBO
		glEnableVertexAttribArray(index);                           // 启用顶点属性数组
		glVertexAttribPointer(index, size, type, normalized, stride, pointer);  // 设置顶点属性指针
	}
	/// <summary>
	/// 计算像素点到直线的距离
	/// </summary>
	static float distanceToLineSegment(float px, float py, float x1, float y1, float x2, float y2)
	{
		// 计算线段的长度
		float dx = x2 - x1;
		float dy = y2 - y1;
		if (dx == 0 && dy == 0)
		{
			// 线段的两个端点重合
			return sqrt((px - x1) * (px - x1) + (py - y1) * (py - y1));
		}
		// 计算参数 t
		float t = ((px - x1) * dx + (py - y1) * dy) / (dx * dx + dy * dy);
		t = std::max(0.0f, std::min(1.0f, t)); // 限制 t 的范围在 [0, 1]
		// 计算最近点的坐标
		float nearestX = x1 + t * dx;
		float nearestY = y1 + t * dy;
		// 计算距离
		float distance = sqrt((px - nearestX) * (px - nearestX) + (py - nearestY) * (py - nearestY));
		return distance;
	}
	/// <summary>
	/// 计算构成贝塞尔曲线的顶点
	/// </summary>
	static vec3 bezierPoint(vector<vec3> ctrlPoints, float t)
	{
		if (ctrlPoints.size() == 1)
			return ctrlPoints[0];
		vector<vec3> newCtrlPoints;
		for (int i = 0; i < ctrlPoints.size() - 1; i++)
		{
			newCtrlPoints.push_back(ctrlPoints[i] * (1 - t) + ctrlPoints[i + 1] * t);
		}
		return bezierPoint(newCtrlPoints, t);
	}
#pragma endregion
	GLuint getVao()const { return mVao; }
	uint32_t getIndicesCount()const { return mIndicesCount; }
private:
	GLuint mVao{ 0 }; // 存储顶点数组对象的ID
	GLuint mPosVbo{ 0 }; // 存储位置缓冲对象的ID
	GLuint mUvVbo{ 0 }; // 存储UV缓冲对象的ID
	GLuint mNormalVbo{ 0 }; // 存储法线缓冲对象的ID
	GLuint mColorVbo{ 0 }; // 存储颜色缓冲对象的ID
	GLuint mEbo{ 0 }; // 存储索引缓冲对象的ID
	uint32_t mIndicesCount{ 0 }; // 记录索引数量，用于绘制
};