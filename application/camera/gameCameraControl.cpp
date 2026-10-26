#include "gameCameraControl.h"

GameCameraControl::GameCameraControl()
{

}

GameCameraControl::~GameCameraControl()
{

}

void GameCameraControl::OnCursor(double xpos, double ypos)
{
	float deltaX = (xpos - mCurrentX) * mSensitivity;
	float deltaY = (ypos - mCurrentY) * mSensitivity;
	if (mRightMouseDown)
	{
		pitch(-deltaY);
		yaw(-deltaX);
	}
	mCurrentX = xpos;
	mCurrentY = ypos;
}

void GameCameraControl::pitch(float angle)
{
	mPitch += angle;
	if (mPitch > 89.0f || mPitch < -89.0f)
	{
		mPitch -= angle;
		return;
	}
	// 计算旋转矩阵
	auto mat = rotate(mat4(1.0f), radians(angle), mCamera->mRight);
	// 计算新的up向量
	mCamera->mUp = mat * vec4(mCamera->mUp, 0.0f);
}

void GameCameraControl::yaw(float angle)
{
	// 计算旋转矩阵
	auto mat = rotate(mat4(1.0f), radians(angle), vec3(0.0f, 1.0f, 0.0f));
	// 计算新的up向量和right向量
	mCamera->mUp = mat * vec4(mCamera->mUp, 0.0f);
	mCamera->mRight = mat * vec4(mCamera->mRight, 0.0f);
}

void GameCameraControl::update(float deltaTime)
{
	vec3 direction(0.0f);
	auto front = cross(mCamera->mUp, mCamera->mRight);
	auto right = mCamera->mRight;
	if (mKeyMap[GLFW_KEY_W])
		direction += front;
	if (mKeyMap[GLFW_KEY_S])
		direction -= front;
	if (mKeyMap[GLFW_KEY_A])
		direction -= right;
	if (mKeyMap[GLFW_KEY_D])
		direction += right;
	if (length(direction) != 0)
	{
		direction = normalize(direction);
		mCamera->mPosition += direction * mSpeed * deltaTime;
	}
}
