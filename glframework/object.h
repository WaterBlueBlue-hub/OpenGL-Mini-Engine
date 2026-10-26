#pragma once
#include "core.h"

enum class ObjectType
{
	Object,
	Mesh,
	InstancedMesh,
	Scene
};

class Object
{
public:
	Object();
	~Object();
#pragma region 变换参数设置函数
	void setPosition(vec3 pos);
	// 设置增量旋转
	void rotateX(float angle);
	void rotateY(float angle);
	void rotateZ(float angle);
	// 设置旋转角
	void setAngleX(float angle);
	void setAngleY(float angle);
	void setAngleZ(float angle);
	void setScale(vec3 scale);
#pragma endregion
	mat4 getModelMatrix()const;
	vec3 getPosition() const { return mPosition; }
	vector<Object*> getChildren() const { return mChildren; }
	Object* getParent() const { return mParent; }
	ObjectType getType()const { return mType; }
	/// <summary>
	/// 添加子级
	/// </summary>
	void addChild(Object* obj);
protected:
	vec3 mPosition{ 0.0f };
	// unity旋转标准：pitch yaw roll
	float mAngleX{ 0.0f };
	float mAngleY{ 0.0f };
	float mAngleZ{ 0.0f };
	vec3 mScale{ 1.0f };
	// 实现父子关系
	vector<Object*> mChildren{};
	Object* mParent{ nullptr };
	// 类型记录
	ObjectType mType;
};