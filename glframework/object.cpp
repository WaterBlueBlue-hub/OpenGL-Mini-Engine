#include "object.h"

Object::Object()
{
	mType = ObjectType::Object;
}

Object::~Object()
{

}
#pragma region 变换参数设置函数
void Object::setPosition(vec3 pos)
{
	mPosition = pos;
}

void Object::rotateX(float angle)
{
	mAngleX += angle;
}

void Object::rotateY(float angle)
{
	mAngleY += angle;
}

void Object::rotateZ(float angle)
{
	mAngleZ += angle;
}

void Object::setAngleX(float angle)
{
	mAngleX = angle;
}

void Object::setAngleY(float angle)
{
	mAngleY = angle;
}

void Object::setAngleZ(float angle)
{
	mAngleZ = angle;
}

void Object::setScale(vec3 scale)
{
	mScale = scale;
}
#pragma endregion
mat4 Object::getModelMatrix()const
{
	// 获取父级的变换矩阵
	mat4 parentModelMatrix{ 1.0f };
	if (mParent != nullptr)
		parentModelMatrix = mParent->getModelMatrix();
	// unity模型变换标准：缩放 旋转 平移
	mat4 transform{ 1.0f };
	// 缩放
	transform = scale(transform, mScale);
	// 旋转
	transform = rotate(transform, radians(mAngleX), vec3(1.0f, 0.0f, 0.0f));
	transform = rotate(transform, radians(mAngleY), vec3(0.0f, 1.0f, 0.0f));
	transform = rotate(transform, radians(mAngleZ), vec3(0.0f, 0.0f, 1.0f));
	// 平移
	transform = parentModelMatrix * translate(mat4(1.0f), mPosition) * transform;
	return transform;
}

void Object::addChild(Object* obj)
{
	// 检查是否加入过子级（返回迭代器）
	auto iter = find(mChildren.begin(), mChildren.end(), obj);
	if (iter != mChildren.end())
	{
		cerr << "加入了重复的子级" << endl;
		return;
	}
	// 加入子级
	mChildren.push_back(obj);
	// 添加父级名称
	obj->mParent = this;
}
