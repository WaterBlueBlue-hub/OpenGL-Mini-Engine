#version 330 core
layout(location = 0)in vec3 aPos;
layout(location = 1)in vec2 aUV;
layout(location = 2)in vec3 aNormal;
out vec2 uv;
out vec3 worldNormal;
out vec3 worldPosition;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
void main()
{
	vec4 transformPosition = vec4(aPos, 1.0);
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * transformPosition;
	uv = aUV;
	worldPosition = vec3(modelMatrix * transformPosition);
	worldNormal = normalize(mat3(transpose(inverse(modelMatrix))) * aNormal);
}
