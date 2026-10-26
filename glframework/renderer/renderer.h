#pragma once
#include "../core.h"
#include "../mesh/mesh.h"
#include "../mesh/instancedMesh.h"
#include "../scene.h"
#include "../../application/camera/camera.h"
#include "../light/directionalLight.h"
#include "../light/ambientLight.h"
#include "../light/pointLight.h"
#include "../light/spotLight.h"
#include "../shader.h"
#include <vector>

class Renderer
{
public:
	Renderer();
	~Renderer();
	void render(
		Scene* scene, 
		Camera* camera,
		DirectionalLight* dirLight,
		const vector<PointLight*>& pointLights,
		const vector<SpotLight*>& spotLights,
		AmbientLight* ambLight,
		unsigned int fbo = 0
	);
	/// <summary>
	/// 渲染单个物体
	/// </summary>
	void renderObject(
		Object* object,
		Camera* camera,
		DirectionalLight* dirLight,
		const vector<PointLight*>& pointLights,
		const vector<SpotLight*>& spotLights,
		AmbientLight* ambLight
	);

	void setClearColor(vec3 color);
	void setSimulationTime(float seconds) { mSimulationTime = seconds; }
	void setSelectionObject(Object* object) { mSelectionObject = object; }
public:
	// 全局材质，用于给模型的Mesh施加相同的材质
	Material* mGlobalMaterial{ nullptr };
private:
	void projectObject(Object* obj);
	Shader* pickShader(MaterialType type);
	void setDepthState(Material* material);
	void setPolygonOffsetState(Material* material);
	void setStencilState(Material* material);
	void setBlendState(Material* material);
	void setFaceCullingState(Material* material);
	void renderSelectionOutline(Camera* camera);
	void renderSkybox(Camera* camera);
private:
	Shader* mNoLightShader{ nullptr }; // 无光照模型
	Shader* mPhongShader{ nullptr }; // 冯氏光照模型
	Shader* mBlinnPhongShader{ nullptr }; // 布林冯光照模型
	Shader* mWhiteShader{ nullptr }; // 纯白模型
	Shader* mScreenShader{ nullptr }; // 贴屏渲染模型
	Shader* mDepthShader{ nullptr }; // 深度缓存模型
	Shader* mGrassInstanceShader{ nullptr }; // 草地模型
	Shader* mOutlineShader{ nullptr };
	Shader* mSkyShader{ nullptr };
	unsigned int mSkyVao{ 0 };
	float mSimulationTime{ 0.0f };
	Object* mSelectionObject{ nullptr };
	// 不透明与透明物体队列
	vector<Mesh*> mOpacityObjects{};
	vector<Mesh*> mTransparentObjects{};
};
