#pragma once

#include "../../glframework/core.h"
#include "camera.h"
#include <map>

class CameraControl
{
public:
	CameraControl();
	~CameraControl();
#pragma region 输入处理函数
	/// <summary>
	/// 处理鼠标按键事件
	/// </summary>
	virtual void OnMouse(int button, int action, double xpos, double ypos);
	/// <summary>
	/// 处理鼠标光标移动事件
	/// </summary>
	virtual void OnCursor(double xpos, double ypos);
	/// <summary>
	/// 处理键盘按键事件
	/// </summary>
	virtual void OnKey(int key, int action, int mods);
	/// <summary>
	/// 处理鼠标滚轮滚动事件
	/// </summary>
	virtual void OnScroll(float offset);
#pragma endregion
	// 帧更新
	virtual void update(float deltaTime = 0.0f);
	// 设置当前相机
	void setCamera(Camera* camera) { mCamera = camera; }
	// 设置鼠标敏感度
	void setSensitivity(float sensitivity) { mSensitivity = sensitivity; }
	// 设置相机缩放速度
	void setScaleSpeed(float scaleSpeed) { mScaleSpeed = scaleSpeed; }
	// 获取键盘状态
	bool getKeyMap(int key)const {
		auto it = mKeyMap.find(key);
		return it != mKeyMap.end() ? it->second : false;
	}
protected:
	// 鼠标按键状态
	bool mLeftMouseDown = false;
	bool mRightMouseDown = false;
	bool mMiddleMouseDown = false;
	// 鼠标位置状态
	float mCurrentX = 0.0f, mCurrentY = 0.0f;
	// 敏感度
	float mSensitivity = 0.2f;
	// 键盘按键状态
	map<int, bool> mKeyMap;
	// 存储当前控制的摄像机
	Camera* mCamera = nullptr;
	// 存储相机的缩放速度
	float mScaleSpeed = 0.2f;
};
