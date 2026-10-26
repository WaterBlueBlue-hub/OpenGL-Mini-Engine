#pragma once

#include "cameraControl.h"

using namespace glm;

class TrackBallCameraControl : public CameraControl
{
public:
	TrackBallCameraControl();
	~TrackBallCameraControl();

	void OnCursor(double xpos, double ypos)override;
	void OnScroll(float offest)override;
	void setMoveSpeed(float moveSpeed) { mMoveSpeed = moveSpeed; }
private:
	void pitch(float angle);
	void yaw(float angle);
private:
	float mMoveSpeed = 0.01f;
};