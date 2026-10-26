#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 2) in vec3 aNormal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform float outlineWidth;

void main()
{
	vec4 worldPosition = modelMatrix * vec4(aPos, 1.0);
	vec3 worldNormal = normalize(mat3(transpose(inverse(modelMatrix))) * aNormal);
	worldPosition.xyz += worldNormal * outlineWidth;
	gl_Position = projectionMatrix * viewMatrix * worldPosition;
}

