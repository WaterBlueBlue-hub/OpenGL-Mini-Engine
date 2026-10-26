#include "geometry.h"
#include <vector>
Geometry::Geometry()
{

}

Geometry::Geometry(
    const vector<float>& positions,
    const vector<float>& normals,
    const vector<float>& uvs,
    const vector<unsigned int>& indices
) {
    mIndicesCount = indices.size();
    // 创建VBO
    createBuffer(mPosVbo, GL_ARRAY_BUFFER, positions.data(), positions.size() * sizeof(float));
    createBuffer(mUvVbo, GL_ARRAY_BUFFER, uvs.data(), uvs.size() * sizeof(float));
    createBuffer(mNormalVbo, GL_ARRAY_BUFFER, normals.data(), normals.size() * sizeof(float));
    // 创建EBO
    createBuffer(mEbo, GL_ELEMENT_ARRAY_BUFFER, indices.data(), indices.size() * sizeof(unsigned int));
    // 创建VAO
    glGenVertexArrays(1, &mVao);
    glBindVertexArray(mVao);
    // 绑定VBO
    setupVertexAttribPointer(mPosVbo, 0, 3, sizeof(float) * 3);
    setupVertexAttribPointer(mUvVbo, 1, 2, sizeof(float) * 2);
    setupVertexAttribPointer(mNormalVbo, 2, 3, sizeof(float) * 3);
    // 加入ebo到当前vao
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
    glBindVertexArray(0);
}

Geometry::Geometry(
    const vector<float>& positions,
    const vector<float>& normals,
    const vector<float>& uvs,
    const vector<float>& colors,
    const vector<unsigned int>& indices
){
    mIndicesCount = indices.size();
    // 创建VBO
    createBuffer(mPosVbo, GL_ARRAY_BUFFER, positions.data(), positions.size() * sizeof(float));
    createBuffer(mUvVbo, GL_ARRAY_BUFFER, uvs.data(), uvs.size() * sizeof(float));
    createBuffer(mNormalVbo, GL_ARRAY_BUFFER, normals.data(), normals.size() * sizeof(float));
    createBuffer(mColorVbo, GL_ARRAY_BUFFER, colors.data(), colors.size() * sizeof(float));
    // 创建EBO
    createBuffer(mEbo, GL_ELEMENT_ARRAY_BUFFER, indices.data(), indices.size() * sizeof(unsigned int));
    // 创建VAO
    glGenVertexArrays(1, &mVao);
    glBindVertexArray(mVao);
    // 绑定VBO
    setupVertexAttribPointer(mPosVbo, 0, 3, sizeof(float) * 3);
    setupVertexAttribPointer(mUvVbo, 1, 2, sizeof(float) * 2);
    setupVertexAttribPointer(mNormalVbo, 2, 3, sizeof(float) * 3);
    setupVertexAttribPointer(mColorVbo, 3, 3, sizeof(float) * 3);
    // 加入ebo到当前vao
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
    glBindVertexArray(0);
}

Geometry::~Geometry()
{
	if (mVao != 0)
		glDeleteVertexArrays(1, &mVao);
	if (mPosVbo != 0)
		glDeleteBuffers(1, &mPosVbo);
	if (mUvVbo != 0)
		glDeleteBuffers(1, &mUvVbo);
    if (mNormalVbo != 0)
        glDeleteBuffers(1, &mNormalVbo);
    if (mColorVbo != 0)
        glDeleteBuffers(1, &mColorVbo);
	if (mEbo != 0)
		glDeleteBuffers(1, &mEbo);
}

Geometry* Geometry::createBox(float size)
{
	Geometry* geometry = new Geometry();
    geometry->mIndicesCount = 36;
	float halfSize = size / 2.0f;
    float positions[] = {
        // 前面
        -halfSize, -halfSize, halfSize,  halfSize, -halfSize, halfSize,  halfSize, halfSize, halfSize,  -halfSize, halfSize, halfSize,
        // 后面
        -halfSize, -halfSize, -halfSize, -halfSize, halfSize, -halfSize,  halfSize, halfSize, -halfSize,  halfSize, -halfSize, -halfSize,
        // 左面
        -halfSize, -halfSize, -halfSize, -halfSize, -halfSize, halfSize, -halfSize, halfSize, halfSize, -halfSize, halfSize, -halfSize,
        // 右面
        halfSize, -halfSize, -halfSize,  halfSize, halfSize, -halfSize,  halfSize, halfSize, halfSize,  halfSize, -halfSize, halfSize,
        // 上面
        -halfSize, halfSize, -halfSize, -halfSize, halfSize, halfSize,  halfSize, halfSize, halfSize,  halfSize, halfSize, -halfSize,
        // 下面
        -halfSize, -halfSize, -halfSize,  halfSize, -halfSize, -halfSize,  halfSize, -halfSize, halfSize,  -halfSize, -halfSize, halfSize
    };
    float uvs[] = {
        // 前面
        0.0f, 0.0f,  1.0f, 0.0f,  1.0f, 1.0f,  0.0f, 1.0f,
        // 后面
        0.0f, 0.0f,  1.0f, 0.0f,  1.0f, 1.0f,  0.0f, 1.0f,
        // 左面
        0.0f, 0.0f,  1.0f, 0.0f,  1.0f, 1.0f,  0.0f, 1.0f,
        // 右面
        0.0f, 0.0f,  1.0f, 0.0f,  1.0f, 1.0f,  0.0f, 1.0f,
        // 上面
        0.0f, 0.0f,  1.0f, 0.0f,  1.0f, 1.0f,  0.0f, 1.0f,
        // 下面
        0.0f, 0.0f,  1.0f, 0.0f,  1.0f, 1.0f,  0.0f, 1.0f,
    };
    float normals[] = {
        // 前面
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        // 后面
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        // 左面
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        // 右面
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        // 上面
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        // 下面
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f
    };
    unsigned int indices[] = {
        0, 1, 2, 2, 3, 0, // 前面
        4, 5, 6, 6, 7, 4, // 后面
        8, 9, 10, 10, 11, 8, // 左面
        12, 13, 14, 14, 15, 12, // 右面
        16, 17, 18, 18, 19, 16, // 上面
        20, 21, 22, 22, 23, 20  // 下面
    };
    // 创建VBO
    GLuint& posVbo = geometry->mPosVbo, uvVbo = geometry->mUvVbo, normalVbo = geometry->mNormalVbo;
    createBuffer(posVbo, GL_ARRAY_BUFFER, positions, sizeof(positions));
    createBuffer(uvVbo, GL_ARRAY_BUFFER, uvs, sizeof(uvs));
    createBuffer(normalVbo, GL_ARRAY_BUFFER, normals, sizeof(normals));
    // 创建EBO
    GLuint& ebo = geometry->mEbo;
    createBuffer(ebo, GL_ELEMENT_ARRAY_BUFFER, indices, sizeof(indices));
    // 创建VAO
    GLuint& vao = geometry->mVao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    // 绑定VBO
    setupVertexAttribPointer(posVbo, 0, 3, sizeof(float) * 3);
    setupVertexAttribPointer(uvVbo, 1, 2, sizeof(float) * 2);
    setupVertexAttribPointer(normalVbo, 2, 3, sizeof(float) * 3);
    // 加入ebo到当前vao
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBindVertexArray(0);
    return geometry;
}

Geometry* Geometry::createSphere(float radius)
{
	Geometry* geometry = new Geometry();
    vector<GLfloat> positions{};
    vector<GLfloat> uvs{};
    vector<GLfloat> normals{};
    vector<GLuint> indices{};
    int numLatLines = 60; // 纬线数量
    int numLongLines = 60; // 经线数量
    // 计算顶点属性
    for (int i = 0; i <= numLatLines; i++)
    {
        for (int j = 0; j <= numLongLines; j++)
        {
            float phi = i * pi<float>() / numLatLines; // 经向角
            float theta = j * 2 * pi<float>() / numLongLines; // 纬向角
            // 计算顶点位置坐标
            float y = radius * cos(phi);
            float x = radius * sin(phi) * cos(theta);
            float z = radius * sin(phi) * sin(theta);
            // 记录至位置数组
            positions.push_back(x);
            positions.push_back(y);
            positions.push_back(z);
            // 记录法线数据
            normals.push_back(x);
            normals.push_back(y);
            normals.push_back(z);
            // 计算顶点uv坐标
            float u = 1.0 - (float)j / (float)numLongLines;
            float v = 1.0 - (float)i / (float)numLatLines;
            // 记录至uv数组
            uvs.push_back(u);
            uvs.push_back(v);
        }
    }
    for (int i = 0; i < numLatLines; i++)
    {
        for (int j = 0; j < numLongLines; j++)
        {
            // 计算顶点索引
            int p1 = i * (numLongLines + 1) + j;
            int p2 = p1 + numLongLines + 1;
            int p3 = p1 + 1;
            int p4 = p2 + 1;
            // 记录至索引数组
            indices.push_back(p1);
            indices.push_back(p2);
            indices.push_back(p3);

            indices.push_back(p3);
            indices.push_back(p2);
            indices.push_back(p4);
        }
    }
    // 创建VBO
    GLuint& posVbo = geometry->mPosVbo, uvVbo = geometry->mUvVbo, normalVbo = geometry->mNormalVbo;
    createBuffer(posVbo, GL_ARRAY_BUFFER, positions.data(), positions.size() * sizeof(float));
    createBuffer(uvVbo, GL_ARRAY_BUFFER, uvs.data(), uvs.size() * sizeof(float));
    createBuffer(normalVbo, GL_ARRAY_BUFFER, normals.data(), normals.size() * sizeof(float));
    // 创建EBO
    GLuint& ebo = geometry->mEbo;
    createBuffer(ebo, GL_ELEMENT_ARRAY_BUFFER, indices.data(), indices.size() * sizeof(GLuint));
    // 创建VAO
    GLuint& vao = geometry->mVao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    // 绑定VBO
    setupVertexAttribPointer(posVbo, 0, 3, sizeof(float) * 3);
    setupVertexAttribPointer(uvVbo, 1, 2, sizeof(float) * 2);
    setupVertexAttribPointer(normalVbo, 2, 3, sizeof(float) * 3);
    // 加入ebo到当前vao
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBindVertexArray(0);
    geometry->mIndicesCount = indices.size();
	return geometry;
}

Geometry* Geometry::createTriangle(float size)
{
    Geometry* geometry = new Geometry();
    geometry->mIndicesCount = 3;
    float halfSize = size / 2.0f;
    float positions[] = {
        0.0f, halfSize, 0.0f, // 顶点
        -halfSize, -halfSize, 0.0f, // 左下角
        halfSize, -halfSize, 0.0f // 右下角
    };
    float uvs[] = {
        0.5f, 1.0f, // 顶点的UV坐标
        0.0f, 0.0f, // 左下角的UV坐标
        1.0f, 0.0f  // 右下角的UV坐标
    };
    float normals[] = {
    0.0f, 0.0f, 1.0f, 
    0.0f, 0.0f, 1.0f, 
    0.0f, 0.0f, 1.0f
    };
    // 索引数据
    unsigned int indices[] = {
        0, 1, 2 // 三角形的三个顶点索引
    };
    // 创建VBO
    GLuint& posVbo = geometry->mPosVbo, uvVbo = geometry->mUvVbo, normalVbo = geometry->mNormalVbo;
    createBuffer(posVbo, GL_ARRAY_BUFFER, positions, sizeof(positions));
    createBuffer(uvVbo, GL_ARRAY_BUFFER, uvs, sizeof(uvs));
    createBuffer(normalVbo, GL_ARRAY_BUFFER, normals, sizeof(normals));
    // 创建EBO
    GLuint& ebo = geometry->mEbo;
    createBuffer(ebo, GL_ELEMENT_ARRAY_BUFFER, indices, sizeof(indices));
    // 创建VAO
    GLuint& vao = geometry->mVao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    // 绑定VBO
    setupVertexAttribPointer(posVbo, 0, 3, sizeof(float) * 3);
    setupVertexAttribPointer(uvVbo, 1, 2, sizeof(float) * 2);
    setupVertexAttribPointer(posVbo, 2, 3, sizeof(float) * 3);
    // 加入EBO到当前VAO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBindVertexArray(0); // 解绑VAO
    return geometry;
}

Geometry* Geometry::createPlane(float Length, float Width, vec3 normal)
{
    Geometry* geometry = new Geometry();
    geometry->mIndicesCount = 6;
    float halfLength = Length / 2.0f;
    float halfWidth = Width / 2.0f;
    // 定义平面在未旋转状态下的顶点位置
    vec3 positions[] = {
        {-halfLength, 0.0f, halfWidth},  // 左上角
        {halfLength, 0.0f, halfWidth},   // 右上角
        {halfLength, 0.0f, -halfWidth},  // 右下角
        {-halfLength, 0.0f, -halfWidth}  // 左下角
    };
    // UV坐标
    vec2 uvs[] = {
        {0.0f, 1.0f}, // 左上角的UV坐标
        {1.0f, 1.0f}, // 右上角的UV坐标
        {1.0f, 0.0f}, // 右下角的UV坐标
        {0.0f, 0.0f}  // 左下角的UV坐标
    };
    // 计算旋转矩阵
    mat4 Rotate;
    vec3 normalN = normalize(normal);
    float angle_rad = acos(dot(vec3(0.0f, 0.0f, 1.0f), normalN));
    vec3 rotation_axis = cross(vec3(0.0f, 0.0f, 1.0f), normalN);
    if (length(rotation_axis) < 1e-6)
        Rotate = mat4(1.0f); // 如果法线与 Z 轴平行，则不需要旋转
    else
        Rotate = rotate(mat4(1.0f), angle_rad, normalize(rotation_axis));
    // 应用旋转矩阵到顶点位置
    for (int i = 0; i < 4; ++i) {
        vec4 rotatedPosition = Rotate * vec4(positions[i], 1.0f);
        positions[i] = vec3(rotatedPosition);
    }
    // 法线（根据法线方向设置）
    vec3 normals[] = {
        normalN,
        normalN,
        normalN,
        normalN
    };
    // 索引数据，两个三角形组成一个矩形
    unsigned int indices[] = {
        0, 1, 2, // 第一个三角形
        0, 2, 3  // 第二个三角形
    };
    // 创建VBO
    GLuint& posVbo = geometry->mPosVbo, uvVbo = geometry->mUvVbo, normalVbo = geometry->mNormalVbo;
    createBuffer(posVbo, GL_ARRAY_BUFFER, positions, sizeof(positions));
    createBuffer(uvVbo, GL_ARRAY_BUFFER, uvs, sizeof(uvs));
    createBuffer(normalVbo, GL_ARRAY_BUFFER, normals, sizeof(normals));
    // 创建EBO
    GLuint& ebo = geometry->mEbo;
    createBuffer(ebo, GL_ELEMENT_ARRAY_BUFFER, indices, sizeof(indices));
    // 创建VAO
    GLuint& vao = geometry->mVao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    // 绑定VBO
    setupVertexAttribPointer(posVbo, 0, 3, sizeof(float) * 3);
    setupVertexAttribPointer(uvVbo, 1, 2, sizeof(float) * 2);
    setupVertexAttribPointer(normalVbo, 2, 3, sizeof(float) * 3); // 使用法线VBO
    // 加入EBO到当前VAO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBindVertexArray(0); // 解绑VAO
    return geometry;
}

Geometry* Geometry::createScreenPlane() {
    Geometry* geometry = new Geometry();
    geometry->mIndicesCount = 6;
    float positions[] = {
        -1.0f,  1.0f,
        -1.0f, -1.0f,
         1.0f, -1.0f,
         1.0f,  1.0f,
    };
    float uvs[] = {
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f
    };
    unsigned int indices[] = {
        0, 1, 2,
        0, 2, 3
    };
    // 创建VBO
    GLuint& posVbo = geometry->mPosVbo, uvVbo = geometry->mUvVbo, normalVbo = geometry->mNormalVbo;
    createBuffer(posVbo, GL_ARRAY_BUFFER, positions, sizeof(positions));
    createBuffer(uvVbo, GL_ARRAY_BUFFER, uvs, sizeof(uvs));
    // 创建EBO
    GLuint& ebo = geometry->mEbo;
    createBuffer(ebo, GL_ELEMENT_ARRAY_BUFFER, indices, sizeof(indices));
    // 创建VAO
    GLuint& vao = geometry->mVao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    // 绑定VBO
    setupVertexAttribPointer(posVbo, 0, 2, sizeof(float) * 2);
    setupVertexAttribPointer(uvVbo, 1, 2, sizeof(float) * 2);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry->mEbo);
    glBindVertexArray(0);
    return geometry;
}
 
Geometry* Geometry::createBresenhamLine(int startX, int startY, int endX, int endY)
{
    Geometry* geometry = new Geometry();
    vector<GLfloat> points{};
    vector<GLfloat> colors{};
    Color lineColor = { 255, 0, 0, 255 }; // 红色线段
    // 确保x方向是从小到大的
    if (startX > endX)
    {
        int temp = startX;
        startX = endX;
        endX = temp;
    }
    points.push_back(startX / 1000);
    points.push_back(startY / 1000);
    points.push_back(0);
    // 确保y方向也是从小到大的，如需翻转，必须记录
    bool flipY = false;
    // 关于y轴做一次翻转
    if (startY > endY)
    {
        startY *= -1.0f;
        endY *= -1.0f;
        flipY = true;
    }
    // 确保斜率在0-1之间，如需要调整，必须记录
    int deltaX = static_cast<int>(endX - startX);
    int deltaY = static_cast<int>(endY - startY);
    bool swapXY = false;
    // 关于y=x轴做一次翻转
    if (deltaX < deltaY)
    {
        swap(startX, startY);
        swap(endX, endY);
        swap(deltaX, deltaY);
        swapXY = true;
    }
    // Bresenham二维直线绘制
    int currentX = static_cast<int>(startX);
    int currentY = static_cast<int>(startY);
    int resultX = 0;
    int resultY = 0;
    vec2 currentPoint;
    int p = 2 * deltaY - deltaX;
    const int sampleCount = 4; // 4x4 采样
    const float sampleOffset = 1.0f / sampleCount; // 每个采样点的偏移量
    for (int i = 0; i <= deltaX; i++)
    {
        // 累加器用于平均颜色
        float accumulatedR = 0.0f;
        float accumulatedG = 0.0f;
        float accumulatedB = 0.0f;
        float accumulatedA = 0.0f;
        int sampleCountTotal = 0;
        for (int sx = 0; sx < sampleCount; sx++)
        {
            for (int sy = 0; sy < sampleCount; sy++)
            {
                // 计算当前采样点的覆盖程度
                float coverage = 1.0f; // 默认覆盖程度
                float pixelCenterX = currentX + (sx + 0.5f) * sampleOffset; // 当前采样点的 X 坐标
                float pixelCenterY = currentY + (sy + 0.5f) * sampleOffset; // 当前采样点的 Y 坐标
                // 计算当前采样点到线段的距离
                float distance = distanceToLineSegment(pixelCenterX, pixelCenterY, startX, startY, endX, endY);
                // 根据距离计算覆盖程度
                if (distance < 0.5f) // 如果距离小于 0.5，表示部分覆盖
                    coverage = 1.0f - distance; // 线性插值，距离越近覆盖越高
                else
                    coverage = 0.0f; // 超过阈值则不覆盖
                // 将颜色分量归一化并乘以覆盖程度
                if (coverage > 0.0f)
                {
                    accumulatedR += lineColor.r / 255.0f * coverage;
                    accumulatedG += lineColor.g / 255.0f * coverage;
                    accumulatedB += lineColor.b / 255.0f * coverage;
                    accumulatedA += 1.0f * coverage; // alpha 通道
                    sampleCountTotal++;
                }
            }
        }
        // 计算平均颜色并保存
        if (sampleCountTotal > 0)
        {
            colors.push_back(accumulatedR / sampleCountTotal);
            colors.push_back(accumulatedG / sampleCountTotal);
            colors.push_back(accumulatedB / sampleCountTotal);
            colors.push_back(accumulatedA / sampleCountTotal); // alpha 通道
        }
    }
    for (int i = 0; i < deltaX; i++)
    {
        if (p >= 0)
        {
            currentY += 1;
            p -= 2 * deltaX;
        }
        currentX += 1;
        p += 2 * deltaY;
        // 处理新xy，flip and swap
        resultX = currentX;
        resultY = currentY;
        if (swapXY)
        {
            swap(resultX, resultY);
        }
        if (flipY)
        {
            resultY *= -1;
        }
        // 产生新顶点
        currentPoint.x = resultX;
        currentPoint.y = resultY;
        points.push_back(currentPoint.x / 1000);
        points.push_back(currentPoint.y / 1000);
        points.push_back(0);
    }
    geometry->mIndicesCount = points.size();
    // 创建 VBO
    GLuint& posVbo = geometry->mPosVbo;
    createBuffer(posVbo, GL_ARRAY_BUFFER, points.data(), points.size() * sizeof(float));
    GLuint colorVbo;
    createBuffer(colorVbo, GL_ARRAY_BUFFER, colors.data(), colors.size() * sizeof(float));
    // 创建 VAO
    GLuint& vao = geometry->mVao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    // 绑定VBO
    setupVertexAttribPointer(posVbo, 0, 3, sizeof(float) * 3);
    setupVertexAttribPointer(colorVbo, 1, 4, sizeof(float) * 4);
    glBindVertexArray(0); // 解绑VAO
    return geometry;
}

Geometry* Geometry::createBezier(vector<vec3> ctrlPoints)
{
    Geometry* geometry = new Geometry();
    vector<GLfloat> points{};
    for (int i = 0; i <= 1024; i++)
    {
        float t = (float)i / 1024.0f;
        vec3 point = bezierPoint(ctrlPoints, t);
        points.push_back(point.x);
        points.push_back(point.y);
        points.push_back(point.z);
    }
    geometry->mIndicesCount = points.size();
    // 创建 VBO
    GLuint& posVbo = geometry->mPosVbo;
    createBuffer(posVbo, GL_ARRAY_BUFFER, points.data(), points.size() * sizeof(float));
    // 创建 VAO
    GLuint& vao = geometry->mVao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    // 绑定VBO
    setupVertexAttribPointer(posVbo, 0, 3, sizeof(float) * 3);
    glBindVertexArray(0); // 解绑VAO
    return geometry;
}



