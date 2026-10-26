#version 330 core

in vec2 screenNdc;
out vec4 finalColor;

uniform mat4 inverseViewProjection;

void main()
{
	vec4 nearPoint = inverseViewProjection * vec4(screenNdc, -1.0, 1.0);
	vec4 farPoint = inverseViewProjection * vec4(screenNdc, 1.0, 1.0);
	vec3 rayDirection = normalize(farPoint.xyz / farPoint.w - nearPoint.xyz / nearPoint.w);

	vec3 skyTop = vec3(0.20, 0.42, 0.72);
	vec3 horizon = vec3(0.72, 0.82, 0.91);
	vec3 lowerSky = vec3(0.38, 0.47, 0.56);

	float upperBlend = pow(clamp(rayDirection.y, 0.0, 1.0), 0.52);
	float lowerBlend = pow(clamp(-rayDirection.y, 0.0, 1.0), 0.45);
	vec3 color = mix(horizon, skyTop, upperBlend);
	color = mix(color, lowerSky, lowerBlend);

	vec3 sunDirection = normalize(vec3(0.35, 0.58, -0.72));
	float sunAmount = max(dot(rayDirection, sunDirection), 0.0);
	float sunDisc = smoothstep(0.9975, 0.9992, sunAmount);
	float sunGlow = pow(sunAmount, 96.0) * 0.32;
	color += vec3(1.0, 0.82, 0.58) * (sunDisc * 0.9 + sunGlow);

	finalColor = vec4(color, 1.0);
}

