#version 330 core
out vec4 FragColor;
in vec2 uv;
uniform sampler2D screenTexSampler;
uniform float texWidth;
uniform float texHeight;
// 颜色反相
vec3 colorInvert(vec3 color)
{
	vec3 invertColor = vec3(1.0) - color;
	return invertColor;
}
// 灰度
vec3 gray(vec3 color)
{
	float avg = color.r * 0.2126 + color.g * 0.7152 + color.b * 0.0722;
	return vec3(avg);
}
// 模糊
vec3 blur(vec3 color)
{
	float du = 1.0 / texWidth;
	float dv = 1.0 / texHeight;
	// 偏移值
	vec2 offsets[9] = vec2[](
		vec2(-du, dv),
		vec2(0.0, dv),
		vec2(du, dv),
		vec2(-du, 0.0),
		vec2(0.0, 0.0),
		vec2(du, 0.0),
		vec2(-du, -dv),
		vec2(0.0, -dv),
		vec2(du, -dv)	
	);
	// 卷积核
	float kernel[9] = float[](
		1.0, 2.0, 1.0,
		2.0, 4.0, 2.0,
		1.0, 2.0, 1.0
	);
	// 加权平均
	vec3 sumColor = vec3(0.0);
	for (int i = 0; i < 9; i++)
	{
		vec3 sampleColor = texture(screenTexSampler, uv + offsets[i]).rgb;
		sumColor + sampleColor * kernel[i];
	}
	sumColor /= 16.0;
	return sumColor;
}

void main()
{
	vec3 color = texture(screenTexSampler, uv).rgb;
	FragColor = texture(screenTexSampler, uv);
}
