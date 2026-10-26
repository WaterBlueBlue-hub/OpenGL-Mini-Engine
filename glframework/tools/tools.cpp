#include "tools.h"

void Tools::decompose(mat4 matrix, vec3& position, vec3& eulerAngle, vec3& scale)
{
	// 四元数
	quat quaternion;
	vec3 skew;
	vec4 perspective;
	glm::decompose(matrix, scale, quaternion, position, skew, perspective);
	// 将四元数还原为欧拉角
	mat4 rotation = toMat4(quaternion);
	extractEulerAngleXYZ(rotation, eulerAngle.x, eulerAngle.y, eulerAngle.z);
	eulerAngle.x = degrees(eulerAngle.x);
	eulerAngle.y = degrees(eulerAngle.y);
	eulerAngle.z = degrees(eulerAngle.z);
}