#pragma once
// 引入标准输入输出流、字符串处理和断言库
#include <iostream>
#include <string.h>
#include <assert.h>
// 引入驱动层OpenGL相关库与框架
#include "glframework/core.h" // 核心库
#include "glframework/shader.h" // 着色器
#include "glframework/texture.h" // 纹理
#include "glframework/geometry.h" // 几何体信息
#include "glframework/object.h" // 模型三维变换信息
#include "glframework/mesh/mesh.h" // 网格数据
#include "glframework/mesh/instancedMesh.h" // 实例绘制网格数据
#include "glframework/scene.h" // 场景数据
#include "glframework/renderer/renderer.h" //渲染器
#include "glframework/light/pointLight.h" //点光源
#include "glframework/light/spotLight.h" //聚光灯
#include "glframework/material/noLightMaterial.h" // 无光照材质
#include "glframework/material/phongMaterial.h" // 冯氏材质
#include "glframework/material/blinnPhongMaterial.h" // 布林冯材质
#include "glframework/material/whiteMaterial.h" // 纯白材质
#include "glframework/material/grassInstanceMaterial.h" // 草地材质
#include "glframework/material/screenMaterial.h" // 贴屏渲染材质
#include "glframework/material/depthMaterial.h" // 深度缓存材质
#include "glframework/tools/tools.h" // 工具类
#include "glframework/framebuffer/framebuffer.h" // 帧缓冲
// 引入应用层
#include "application/application.h"
// 错误检查
#include "wrapper/checkError.h" 
// 引入imgui
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
// 引入相机与控制器
#include "application/camera/perspectiveCamera.h"
#include "application/camera/orthographicCamera.h"
#include "application/camera/trackBallCameraControl.h"
#include "application/camera/gameCameraControl.h"
// 引入模型读取工具
#include "glframework/mesh/simpleObjLoader.h"

using namespace std;
using namespace glm;
using namespace ImGui;


