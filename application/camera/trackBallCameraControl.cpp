#include "trackBallCameraControl.h"

TrackBallCameraControl::TrackBallCameraControl()
{

}

TrackBallCameraControl::~TrackBallCameraControl()
{

}

void TrackBallCameraControl::OnCursor(double xpos, double ypos)
{
	if (mLeftMouseDown)
	{
		// 计算经纬方向旋转的增量
		float deltaX = (xpos - mCurrentX) * mSensitivity;
		float deltaY = (ypos - mCurrentY) * mSensitivity;
		// 分别计算pitch和yaw角
		pitch(-deltaY);
		yaw(-deltaX);
	}
	else if (mMiddleMouseDown)
	{
		// 计算up和right方向平移的增量
		float deltaX = (xpos - mCurrentX) * mMoveSpeed;
		float deltaY = (ypos - mCurrentY) * mMoveSpeed;
		mCamera->mPosition += mCamera->mUp * deltaY;
		mCamera->mPosition -= mCamera->mRight * deltaX;
	}
	mCurrentX = xpos;
	mCurrentY = ypos;

}

void TrackBallCameraControl::OnScroll(float offest)
{
	mCamera->scale(mScaleSpeed * offest);
}

void TrackBallCameraControl::pitch(float angle)
{
	// 计算旋转矩阵
	auto mat = rotate(mat4(1.0f), radians(angle), mCamera->mRight);
	// 计算新的up向量和位置
	mCamera->mUp = mat * vec4(mCamera->mUp, 0.0f);
	mCamera->mPosition = mat * vec4(mCamera->mPosition, 1.0f);
}

void TrackBallCameraControl::yaw(float angle)
{
	// 计算旋转矩阵
	auto mat = rotate(mat4(1.0f), radians(angle), vec3(0.0f, 1.0f, 0.0f));
	// 计算新的up、right向量和位置
	mCamera->mUp = mat * vec4(mCamera->mUp, 0.0f);
	mCamera->mRight = mat * vec4(mCamera->mRight, 0.0f);
	mCamera->mPosition = mat * vec4(mCamera->mPosition, 1.0f);
}