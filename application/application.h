#pragma once

#include <iostream>
#include <cstdint>

#define glApp Application::getInstance()

class GLFWwindow;
// 定义函数指针类型
using ResizeCallback = void(*)(int width, int height);
using KeyBoardCallback = void(*)(int key, int action, int mods);
using MouseCallback = void(*)(int button, int action, int mods);
using CursorCallback = void(*)(double xpos, double ypos);
using ScrollCallback = void(*)(double offset);

class Application
{
public:
	~Application();
	/// <summary>
	/// 用于访问实例的静态函数
	/// </summary>
	static Application* getInstance();
	/// <summary>
	/// 访问对象宽度
	/// </summary>
	uint32_t getWidth()const { return mWidth; }
	/// <summary>
	/// 访问对象高度
	/// </summary>
	uint32_t getHeight()const { return mHeight; }
	/// <summary>
	/// 获取光标位置
	/// </summary>
	void getCursorPosition(double* x, double* y);
	/// <summary>
	/// 获取glfw窗体指针，用于和ImGui进行绑定
	/// </summary>
	GLFWwindow* getWindow()const { return mWindow; }
#pragma region 生命周期函数
	/// <summary>
	/// 初始化 OpenGL 应用程序的窗口和上下文，设置 OpenGL 的版本和模式，并注册各种输入事件的回调函数
	/// </summary>
	bool Init(const int& width = 800, const int& height = 600);
	/// <summary>
	/// 处理窗口的更新逻辑，包括检查窗口是否需要关闭和处理输入事件
	/// </summary>
	bool Updata();
	/// <summary>
	/// 提交当前帧并交换前后缓冲
	/// </summary>
	void SwapBuffers();
	/// <summary>
	/// 程序结束时进行必要的清理工作，确保 GLFW 正确终止并释放所有资源
	/// </summary>
	void Destroy();
#pragma endregion
#pragma region 事件回调设置函数
	void setResizeCallback(ResizeCallback callback) { mResizeCallback = callback; }
	void setKeyBoradCallback(KeyBoardCallback callback) { mKeyBoardCallback = callback; }
	void setMouseCallback(MouseCallback callback) { mMouseCallback = callback; }
	void setCursorCallback(CursorCallback callback) { mCursorCallback = callback; }
	void setScrollCallback(ScrollCallback callback) { mScrollCallback = callback; }
#pragma endregion
private:
#pragma region GLFW注册回调函数
	/// <summary>
	/// 处理帧缓冲大小变化的回调函数。当窗口的帧缓冲大小发生变化时，这个回调函数会被自动调用
	/// </summary>
	static void frameBufferSizeCallback(GLFWwindow* window, int width, int height);
	/// <summary>
	/// 处理键盘响应的回调函数。当键盘消息发生变化时，这个回调函数会被自动调用
	/// </summary>
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	/// <summary>
	/// 处理鼠标点击响应的回调函数。当鼠标点击消息发生变化时，这个回调函数会被自动调用
	/// </summary>
	static void mouseCallback(GLFWwindow* window, int button, int action, int mods);
	/// <summary>
	/// 处理鼠标移动响应的回调函数。当鼠标移动消息发生变化时，这个回调函数会被自动调用
	/// </summary>
	static void cursorCallback(GLFWwindow* window, double xpos, double ypos);
	/// <summary>
	/// 处理鼠标滚轮响应的回调函数。当鼠标滚轮消息发生变化时，这个回调函数会被自动调用
	/// </summary>
	static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
#pragma endregion
private:
	// 全局唯一的静态变量实例
	static Application* mInstance;

	uint32_t mWidth{ 0 }; // 记录窗体宽度
	uint32_t mHeight{ 0 }; // 记录窗体高度
	GLFWwindow* mWindow{ nullptr }; // 存储窗体指针
	// 存储回调函数指针
	ResizeCallback mResizeCallback{ nullptr };
	KeyBoardCallback mKeyBoardCallback{ nullptr };
	MouseCallback mMouseCallback{ nullptr };
	CursorCallback mCursorCallback{ nullptr };
	ScrollCallback mScrollCallback{ nullptr };
    // 用private限定，保证其是一个单例类，全局唯一
	Application();
};
