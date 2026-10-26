#version 330 core

out vec2 screenNdc;

void main()
{
	vec2 positions[3] = vec2[](
		vec2(-1.0, -1.0),
		vec2( 3.0, -1.0),
		vec2(-1.0,  3.0)
	);
	screenNdc = positions[gl_VertexID];
	gl_Position = vec4(screenNdc, 0.9999, 1.0);
}

