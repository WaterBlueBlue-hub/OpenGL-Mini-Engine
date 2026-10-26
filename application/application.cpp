#include "application.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

using namespace std;
// 初始化Application的静态变量
Application* Application::mInstance = nullptr;
Application* Application::getInstance() 
{
	// 保证instance实例化
	if (mInstance == nullptr)
		mInstance = new Application();
	return mInstance;
}

Application::Application()
{

}

Application::~Application()
{

}

void Application::getCursorPosition(double* x, double* y)
{
	// 为x, y赋予当前窗体光标的位置信息
	glfwGetCursorPos(mWindow, x, y);
}
#pragma region 生命周期函数
bool Application::Init(const int& width, const int& height)
{
	mWidth = width;
	mHeight = height;
	// 初始化(Init)GLFW基本环境
	if (!glfwInit())
	{
		cerr << "GLFW initialization failed." << endl;
		return false;
	}
	// 设置OpenGL版本号
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // 主版本号
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // 次版本号
	// 设置OpenGL启用核心模式（非立即渲染）
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// 创建窗体对象
	mWindow = glfwCreateWindow(mWidth, mHeight, "OpenGL Mini Engine", NULL, NULL);
	if (mWindow == NULL)
		return false;
	glfwSetWindowSizeLimits(mWindow, 1280, 720, GLFW_DONT_CARE, GLFW_DONT_CARE);
	// 设置当前窗体对象为OpenGL的绘制舞台
	glfwMakeContextCurrent(mWindow);
	/* 使用glad加载所有当前版本OpenGL的函数
	 gladLoadGLLoader：用于加载OpenGL的函数指针。它接收一个函数指针作为参数，该函数指针用于获取OpenGL函数的地址。
	 glfwGetProcAddress：用于获取OpenGL函数的地址。在使用GLFW创建窗口和上下文后，可以通过这个函数来获取OpenGL的扩展和核心函数。*/
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cout << "初始化GLAD失败" << endl;
		return false;
	}
	// this就是当前全局唯一的Application对象，把this指针存入当前窗体
	glfwSetWindowUserPointer(mWindow, this);
	glfwSwapInterval(1);
	// 监听帧缓冲大小响应。将frameBufferSizeCallback函数指针与当前窗口绑定，它会在窗口大小变化时被调用。
	glfwSetFramebufferSizeCallback(mWindow, frameBufferSizeCallback);
	// 监听键盘响应。将keyCallback函数指针与当前窗口绑定，它会在存在键盘消息时被调用。
	glfwSetKeyCallback(mWindow, keyCallback);
	// 监听鼠标点击响应。将mouseCallback函数指针与当前窗口绑定，它会在存在鼠标点击消息时被调用。
	glfwSetMouseButtonCallback(mWindow, mouseCallback);
	// 监听鼠标移动响应。将cursorCallback函数指针与当前窗口绑定，它会在存在鼠标移动消息时被调用。
	glfwSetCursorPosCallback(mWindow, cursorCallback);
	// 监听鼠标滚轮响应。将scrollCallback函数指针与当前窗口绑定，它会在存在鼠标滚轮消息时被调用。
	glfwSetScrollCallback(mWindow, scrollCallback);
	return true;
}

bool Application::Updata()
{
	if (glfwWindowShouldClose(mWindow)) // 点击关闭按键返回true
		return false;
	/*接收并分发窗体消息
	检查消息队列是否有待处理的消息
	如果有，则将消息批量处理，清空队列*/
	glfwPollEvents();
	return true;
}

void Application::SwapBuffers()
{
	glfwSwapBuffers(mWindow);
}

void Application::Destroy()
{
	// 退出程序前做相关清理
	if (mWindow != nullptr)
	{
		glfwDestroyWindow(mWindow);
		mWindow = nullptr;
	}
	glfwTerminate();
}
#pragma endregion
#pragma region GLFW注册回调函数
void Application::frameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
	// 获取Application实例
	// glfwGetWindowUserPointer：获取与窗口关联的用户数据。在创建窗口时，可以使用 glfwSetWindowUserPointer 将一个指针存储在窗口中。
	Application* self = (Application*)glfwGetWindowUserPointer(window);
	if (width > 0)
		self->mWidth = static_cast<uint32_t>(width);
	if (height > 0)
		self->mHeight = static_cast<uint32_t>(height);
	if (self->mResizeCallback != nullptr)
		self->mResizeCallback(width, height);
}

void Application::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Application* self = (Application*)glfwGetWindowUserPointer(window);
	if (self->mKeyBoardCallback != nullptr)
		self->mKeyBoardCallback(key, action, mods);
}

void Application::mouseCallback(GLFWwindow* window, int button, int action, int mods)
{
	Application* self = (Application*)glfwGetWindowUserPointer(window);
	if (self->mMouseCallback != nullptr)
		self->mMouseCallback(button, action, mods);
}

void Application::cursorCallback(GLFWwindow* window, double xpos, double ypos)
{
	Application* self = (Application*)glfwGetWindowUserPointer(window);
	if (self->mCursorCallback != nullptr)
		self->mCursorCallback(xpos, ypos);
}

void Application::scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	Application* self = (Application*)glfwGetWindowUserPointer(window);
	if (self->mScrollCallback != nullptr)
		self->mScrollCallback(yoffset);
}
#pragma endregion


