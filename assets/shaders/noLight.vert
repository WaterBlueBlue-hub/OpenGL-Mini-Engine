#version 330 core
layout(location = 0)in vec3 aPos;
layout(location = 1)in vec4 aColor;
out vec4 color;
out vec3 uvw;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
void main()
{
	vec4 transformPosition = vec4(aPos, 1.0);
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * transformPosition;
	gl_Position = gl_Position.xyww;
	uvw = aPos;
	color = aColor;
}
