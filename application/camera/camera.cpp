#include"camera.h"

Camera::Camera()
{

}

Camera::Camera(vec3 position, vec3 up, vec3 right)
{
	mPosition = position;
	mUp = up;
	mRight = right;
}

Camera::~Camera()
{

}

mat4 Camera::getViewMatrix()
{
	vec3 front = cross(mUp, mRight);
	vec3 center = mPosition + front;
	// eye：当前摄像机所在的位置
	// center：当前摄像机看向的点
	// up：穹顶向量
	return lookAt(mPosition, center, mUp);
}

mat4 Camera::getProjectionMatrix()
{
	return identity<mat4>();
}

void Camera::scale(float deltaScale)
{

}