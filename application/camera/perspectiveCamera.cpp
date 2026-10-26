#include "perspectiveCamera.h"

PerspectiveCamera::PerspectiveCamera(float fovy, float aspect, float near, float far)
{
	mFovy = fovy;
	mAspect = aspect;
	mNear = near;
	mFar = far;
}

PerspectiveCamera::~PerspectiveCamera()
{

}

mat4 PerspectiveCamera::getProjectionMatrix()
{
	return perspective(radians(mFovy), mAspect, mNear, mFar);
}

void PerspectiveCamera::scale(float deltaScale)
{
	auto front = cross(mUp, mRight);
	mPosition += front * deltaScale;
}