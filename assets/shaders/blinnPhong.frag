#version 330 core
in vec2 uv;
in vec3 normal;
in vec3 worldPosition;
out vec4 finalColor;
uniform sampler2D sampler;
uniform sampler2D specularMaskSampler;
uniform vec3 lightDirection; // 光照方向
uniform vec3 lightColor; // 光照颜色
uniform float specularIntensity; // 镜面反射强度
uniform float shiness; // 高光集中度
uniform vec3 ambientColor; // 环境光
uniform vec3 cameraPosition; // 相机位置
void main()
{
    vec3 objectColor = vec3(1.0);
    objectColor = texture(sampler, uv).xyz; // 使用纹理采样来获取物体颜色
    // 计算相关向量
    vec3 normalN = normalize(normal);
    vec3 lightDirN = normalize(lightDirection);
    vec3 lightReflect = normalize(reflect(lightDirN, normalN)); // 计算反射方向
    vec3 viewDir = normalize(worldPosition - cameraPosition); // 计算视线方向
    // 计算漫反射分量
    float diffuse = max(dot(-lightDirN, normalN), 0.0);
    // 漫反射颜色 = 光照颜色 * 漫反射强度 * 物体颜色
    vec3 diffuseColor = lightColor * diffuse * objectColor;
    // 计算镜面反射分量
    float flag = step(0.0, dot(-lightDirN, normalN)); // 如果点积大于零，则镜面反射有效
    // 计算Blinn-Phong模型的镜面反射分量
    vec3 halfwayDir = normalize(lightDirN + viewDir); // 半程向量 = normalize(光源方向 + 视线方向)
    float specular = pow(max(dot(-halfwayDir, normalN), 0.0), shiness); // 镜面反射强度
    // 读取镜面反射掩码（高光蒙版）
    float specularMask = texture(specularMaskSampler, uv).r;
    // 镜面反射颜色 = 镜面反射开启标记（0或1）* 光照颜色 * 镜面反射强度  * 镜面反射强度增强系数 * 镜面反射掩码
    vec3 specularColor = flag * lightColor * specular * specularIntensity * specularMask ;
    // 计算环境光分量
    vec3 ambientColor = ambientColor * objectColor; 
    // 最终的颜色 = 漫反射 + 镜面反射 + 环境光
    finalColor = vec4(diffuseColor + specularColor + ambientColor, 1.0);
}