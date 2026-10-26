#version 330 core
in vec2 uv;
in vec3 normal;
in vec3 worldPosition;
out vec4 finalColor;
// 采样器
uniform sampler2D sampler;
uniform int specularMaskSamplerFlag;
uniform sampler2D specularMaskSampler;
uniform int opacityMaskSamplerFlag;
uniform sampler2D opacityMaskSampler;
uniform int envSamplerFlag;
uniform samplerCube envSampler;
// 光照相关
uniform float shiness; // 高光集中度
uniform vec3 ambientColor; // 环境光颜色
uniform vec3 cameraPosition; // 相机位置
// 透明度
uniform float opacity;
// 平行光
struct DirectionalLight
{
    vec3 direction;
    vec3 color;
    float specularIntensity;
};
// 点光源
struct PointLight
{
    vec3 position;
    vec3 color;
    float specularIntensity;
    // 控制系数
    float k2;
    float k1;
    float kc;
};
// 聚光灯
struct SpotLight
{
    vec3 position;
    vec3 targetDirection;
    vec3 color;
    float innerLine; // 绝对可视区
    float outerLine; // 可视区
    float specularIntensity;
};
// 生成光源对象
#define LIGHT_NUM 100
uniform DirectionalLight directionalLight;
uniform int pointLightNum;
uniform PointLight pointLights[LIGHT_NUM];
uniform int spotLightNum;
uniform SpotLight spotLights[LIGHT_NUM];
// 计算漫反射
vec3 calculateDiffuse(vec3 lightColor, vec3 objectColor, vec3 lightDir, vec3 normal)
{
    // 计算漫反射强度
    float diffuse = max(dot(-lightDir, normal), 0.0);
    // 漫反射颜色 =  光照颜色 * 物体颜色 * 漫反射强度
    vec3 diffuseColor = lightColor * objectColor * diffuse;
    return diffuseColor;
}
// 计算镜面反射
vec3 calculateSpecular(vec3 lightColor, vec3 lightDir, vec3 normal, vec3 viewDir, float intensity)
{
    float flag = step(0.0, dot(-lightDir, normal)); // 如果点积大于零，则镜面反射有效
    vec3 lightReflect = normalize(reflect(lightDir, normal)); // 计算反射方向
    float specular = pow(max(dot(lightReflect, -viewDir), 0.0), shiness); // 镜面反射强度
    // 读取镜面反射掩码（高光蒙版）
    float specularMask;
    specularMask += (1 - specularMaskSamplerFlag) * 1;
    specularMask += specularMaskSamplerFlag * texture(specularMaskSampler, uv).r;
    // 镜面反射颜色 = 镜面反射开启标记（0或1）* 光照颜色 * 镜面反射强度  * 镜面反射强度增强系数 * 镜面反射掩码
    vec3 specularColor = flag * lightColor * specular * intensity * specularMask ;
    return specularColor;
}
// 计算环境光
vec3 calculateEnv(vec3 normal, vec3 viewDir)
{
    vec3 reflectDir = normalize(reflect(viewDir, normal));
    vec3 color = texture(envSampler, reflectDir).rgb;
    return color;
}
// 计算平行光
vec3 calculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir)
{
    // 计算通用数据
    vec3 objectColor = texture(sampler, uv).xyz;
    vec3 lightDir = normalize(light.direction);
    // 计算漫反射
    vec3 diffuseColor = calculateDiffuse(light.color, objectColor, lightDir, normal);
    // 计算镜面反射
    vec3 specularColor = calculateSpecular(light.color, lightDir, normal, viewDir, light.specularIntensity);
    return diffuseColor + specularColor;
}
// 计算点光源
vec3 calculatePointLight(PointLight light, vec3 normal, vec3 viewDir)
{
    // 计算通用数据
    vec3 objectColor = texture(sampler, uv).xyz;
    vec3 lightDir = normalize(worldPosition - light.position);
    // 计算衰减
    float dist = length(worldPosition - light.position);
    float attenuation = 1.0 /(light.k2 * pow(dist, 2) + light.k1 * dist + light.kc); // 计算衰减系数
    // 计算漫反射
    vec3 diffuseColor = calculateDiffuse(light.color, objectColor, lightDir, normal);
    // 计算镜面反射
    vec3 specularColor = calculateSpecular(light.color, lightDir, normal, viewDir, light.specularIntensity);
    return (diffuseColor + specularColor) * attenuation;
}
// 计算聚光灯
vec3 calculateSpotLight(SpotLight light, vec3 normal, vec3 viewDir)
{
    // 计算通用数据
    vec3 objectColor = texture(sampler, uv).xyz;
    vec3 lightDir = normalize(worldPosition - light.position);
    vec3 targetDir = normalize(light.targetDirection);
    // 计算照射范围
    float cGamma = dot(lightDir, targetDir);
    float intensity = clamp((cGamma - light.outerLine) / (light.innerLine - light.outerLine), 0.0, 1.0);
    // 计算漫反射
    vec3 diffuseColor = calculateDiffuse(light.color, objectColor, lightDir, normal);
    // 计算镜面反射
    vec3 specularColor = calculateSpecular(light.color, lightDir, normal, viewDir, light.specularIntensity);
    return (diffuseColor + specularColor) * intensity;
}
// 主函数
void main()
{
    vec3 result = vec3(0.0, 0.0, 0.0);
    // 计算通用数据
    vec3 objectColor = texture(sampler, uv).xyz;
    vec3 normalN = normalize(normal);
    vec3 viewDir = normalize(worldPosition - cameraPosition);
    // 计算平行光
    result += calculateDirectionalLight(directionalLight, normalN, viewDir);
    // 计算点光源
    for (int i = 0; i < pointLightNum; i++)
    {
        result += calculatePointLight(pointLights[i], normalN, viewDir);
    }
    // 计算聚光灯
    for (int i = 0; i < spotLightNum; i++)
    {
        result += calculateSpotLight(spotLights[i], normalN, viewDir);
    }
    // 计算环境光
    result += envSamplerFlag * calculateEnv(normal, viewDir) * objectColor; 
    result += (1 - envSamplerFlag) * ambientColor * objectColor; 
    // 计算透明度
    float alpha;
    alpha += (1 - opacityMaskSamplerFlag) * texture(sampler, uv).a;
    alpha += opacityMaskSamplerFlag * texture(opacityMaskSampler, uv).r;
    // 最终的颜色 = 平行光 + 点光源 + 聚光灯 + 环境光
    finalColor = vec4(result, alpha * opacity);
}