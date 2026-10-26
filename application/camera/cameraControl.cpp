#include "cameraControl.h"
#include <iostream>

CameraControl::CameraControl()
{

}

CameraControl::~CameraControl()
{

}
#pragma region 输入处理函数
void CameraControl::OnMouse(int button, int action, double xpos, double ypos)
{
	// 判断按键是否按下
	bool pressed = action == GLFW_PRESS ? true : false;
	// 如果按下，记录当前按下的位置
	if (pressed)
	{
		mCurrentX = xpos;
		mCurrentY = ypos;
	}
	// 根据按下的鼠标按键，更改按键状态
	switch (button)
	{
	case GLFW_MOUSE_BUTTON_LEFT:
		mLeftMouseDown = pressed;
		break;
	case GLFW_MOUSE_BUTTON_RIGHT:
		mRightMouseDown = pressed;
		break;
	case GLFW_MOUSE_BUTTON_MIDDLE:
		mMiddleMouseDown = pressed;
		break;
	default:
		break;
	}
}

void CameraControl::OnCursor(double xpos, double ypos)
{

}

void CameraControl::OnKey(int key, int action, int mods)
{
	// 过滤掉repeat（持续按住回调）的情况
	if (action == GLFW_REPEAT)
		return;
	// 判断按键是否按下
	bool pressed = action == GLFW_PRESS ? true : false;
	// 记录在KeyMap中
	mKeyMap[key] = pressed;
}

void CameraControl::OnScroll(float offset)
{

}
#pragma endregion
void CameraControl::update(float deltaTime)
{

}
