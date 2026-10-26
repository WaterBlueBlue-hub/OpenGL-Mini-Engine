#pragma once

#include "cameraControl.h"

class GameCameraControl : public CameraControl
{
public:
	GameCameraControl();
	~GameCameraControl();
	/// <summary>
	/// 处理鼠标光标移动事件
	/// </summary>
	void OnCursor(double xpos, double ypos)override;
	/// <summary>
	/// 帧更新
	/// </summary>
	void update(float deltaTime = 0.0f)override;
	/// <summary>
	/// 设置相机的移动速度
	/// </summary>
	void setSpeed(float speed) { mSpeed = speed; }
	void resetPitch() { mPitch = 0.0f; }
private:
	/// <summary>
	/// 更新相机的俯仰角度。如果新的俯仰角度超出范围（-89°到89°），则不更新
	/// </summary>
	void pitch(float angle);
	/// <summary>
	/// 更新相机的偏航角度
	/// </summary>
	void yaw(float angle);
private:
	float mPitch = { 0.0f }; // 记录相机的俯仰角度
	float mSpeed = { 0.001f }; // 记录相机移动的速度
};
