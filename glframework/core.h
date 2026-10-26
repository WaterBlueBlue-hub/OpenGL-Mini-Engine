#pragma once

// GLAD是一个OpenGL的加载库，用于加载OpenGL函数指针。它会根据你的OpenGL版本和平台自动加载适当的函数
#include <glad/glad.h> // glad在glfw引用之前引用
// GLFW 是一个跨平台的库，用于创建窗口、处理输入（键盘、鼠标等）和管理OpenGL上下文。它提供了简单的接口来管理OpenGL窗口和输入事件
#include <GLFW/glfw3.h>
// GLM 是一个基 C++的数学库，专门为图形编程设计，提供了向量、矩阵、变换等数学功能。glm.hpp包含了基本的向量和矩阵类型
#include <glm/glm.hpp>
// 该头文件提供了用于创建平移、旋转和缩放变换的函数。这些函数可以用来生成变换矩阵，以便在图形渲染中应用于物体的位置和方向
#include <glm/gtc/matrix_transform.hpp>
// 该头文件提供了将GLM向量和矩阵转换为原始指针的功能
#include <glm/gtc/type_ptr.hpp>
// 这条宏定义允许使用GLM的一些实验性功能
#define GLM_ENABLE_EXPERIMENTAL
// 该头文件提供了将GLM向量和矩阵转换为字符串表示的功能
#include <glm/gtx/string_cast.hpp>
// 引入GLM库的矩阵分解功能
#include <glm/gtx/matrix_decompose.hpp>
// 引入GLM库的欧拉角功能
#include <glm/gtx/euler_angles.hpp>
// 引入GLM库的四元数功能
#include <glm/gtx/quaternion.hpp>

#include <vector>
#include <map>
#include <iostream>

using namespace glm;
using namespace std;