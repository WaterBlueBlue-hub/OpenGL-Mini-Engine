#include "orthographicCamera.h"


OrthographicCamera::OrthographicCamera(float left, float right, float top, float bottom, float near, float far)
{
	mLeft = left;
	mRight = right;
	mTop = top;
	mBottom = bottom;
	mNear = near;
	mFar = far;
}

OrthographicCamera::~OrthographicCamera()
{

}

mat4 OrthographicCamera::getProjectionMatrix()
{
	float scale = pow(2.0f, mScale);
	return ortho(mLeft * scale, mRight * scale, mBottom * scale, mTop * scale, mNear, mFar);
}

void OrthographicCamera::scale(float deltaScale)
{
	mScale += deltaScale;
}
