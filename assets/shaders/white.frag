#version 330 core
in vec2 uv;
in vec3 worldNormal;
in vec3 worldPosition;
out vec4 finalColor;

uniform vec3 baseColor;
uniform vec3 directionalLightColor;
uniform vec3 directionalLightDirection;
uniform vec3 ambientColor;

void main()
{
    vec3 normal = normalize(worldNormal);
    vec3 lightDirection = normalize(-directionalLightDirection);
    float diffuse = max(dot(normal, lightDirection), 0.0);
    float hemisphere = 0.5 + 0.5 * normal.y;
    vec3 lighting = ambientColor + vec3(hemisphere * 0.16) + directionalLightColor * diffuse;
    finalColor = vec4(baseColor * lighting, 1.0);
}
