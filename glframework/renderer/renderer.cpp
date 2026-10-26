#include "renderer.h"
#include "../material/noLightMaterial.h"
#include "../material/phongMaterial.h"
#include "../material/blinnPhongMaterial.h"
#include "../material/whiteMaterial.h"
#include "../material/screenMaterial.h"
#include "../material/grassInstanceMaterial.h"
#include <iostream>
#include <string>
#include <algorithm>

Renderer::Renderer()
{
	mPhongShader = new Shader("assets/shaders/phong.vert", "assets/shaders/phong.frag");
	mNoLightShader = new Shader("assets/shaders/noLight.vert", "assets/shaders/noLight.frag");
	mBlinnPhongShader = new Shader("assets/shaders/blinnPhong.vert", "assets/shaders/blinnPhong.frag");
	mWhiteShader = new Shader("assets/shaders/white.vert", "assets/shaders/white.frag");
	mScreenShader = new Shader("assets/shaders/screen.vert", "assets/shaders/screen.frag");
	mDepthShader = new Shader("assets/shaders/depth.vert", "assets/shaders/depth.frag");
	mGrassInstanceShader = new Shader("assets/shaders/grassInstance.vert", "assets/shaders/grassInstance.frag");
	mOutlineShader = new Shader("assets/shaders/outline.vert", "assets/shaders/outline.frag");
	mSkyShader = new Shader("assets/shaders/skybox.vert", "assets/shaders/skybox.frag");
	glGenVertexArrays(1, &mSkyVao);
}

Renderer::~Renderer()
{
	delete mPhongShader;
	delete mNoLightShader;
	delete mBlinnPhongShader;
	delete mWhiteShader;
	delete mScreenShader;
	delete mDepthShader;
	delete mGrassInstanceShader;
	delete mOutlineShader;
	delete mSkyShader;
	if (mSkyVao != 0)
		glDeleteVertexArrays(1, &mSkyVao);
}

void Renderer::setClearColor(vec3 color)
{
	glClearColor(color.r, color.g, color.b, 1.0f);
}

void Renderer::render(
	Scene* scene,
	Camera* camera,
	DirectionalLight* dirLight,
	const vector<PointLight*>& pointLights,
	const vector<SpotLight*>& spotLights,
	AmbientLight* ambLight,
	unsigned int fbo
) {
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	// 开启深度缓存
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
	// 关闭多边形偏移
	glDisable(GL_POLYGON_OFFSET_FILL);
	glDisable(GL_POLYGON_OFFSET_LINE);
	// 开启模板缓存
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glStencilMask(0xFF);
	// 关闭颜色混合
	glDisable(GL_BLEND);
	// 清理画布
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	// 清空两个队列
	mOpacityObjects.clear();
	mTransparentObjects.clear();
	renderSkybox(camera);
	projectObject(scene);
	sort(
		mTransparentObjects.begin(),
		mTransparentObjects.end(),
		[camera](const Mesh* a, const Mesh* b)
		{
			auto viewMatrix = camera->getViewMatrix();

			auto modelMatrixA = a->getModelMatrix();
			auto worldPositionA = modelMatrixA * vec4(0.0, 0.0, 0.0, 1.0);
			auto cameraPositionA = viewMatrix * worldPositionA;

			auto modelMatrixB = b->getModelMatrix();
			auto worldPositionB = modelMatrixB * vec4(0.0, 0.0, 0.0, 1.0);
			auto cameraPositionB = viewMatrix * worldPositionB;

			return cameraPositionA.z < cameraPositionB.z;
		}
	);
	renderSelectionOutline(camera);
	// 渲染两个队列
	for (int i = 0; i < mOpacityObjects.size(); i++)
	{
		renderObject(mOpacityObjects[i], camera, dirLight, pointLights, spotLights, ambLight);
	}
	for (int i = 0; i < mTransparentObjects.size(); i++)
	{
		renderObject(mTransparentObjects[i], camera, dirLight, pointLights, spotLights, ambLight);
	}
}

void Renderer::renderSkybox(Camera* camera)
{
	mSkyShader->begin();
	mSkyShader->setMatrix4x4(
		"inverseViewProjection",
		inverse(camera->getProjectionMatrix() * camera->getViewMatrix())
	);

	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glDisable(GL_STENCIL_TEST);
	glBindVertexArray(mSkyVao);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);

	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	mSkyShader->end();
}

void Renderer::renderSelectionOutline(Camera* camera)
{
	if (mSelectionObject == nullptr ||
		mSelectionObject->getType() != ObjectType::Mesh)
	{
		return;
	}

	Mesh* mesh = static_cast<Mesh*>(mSelectionObject);
	if (mesh->mGeometry == nullptr)
		return;

	mOutlineShader->begin();
	mOutlineShader->setMatrix4x4("modelMatrix", mesh->getModelMatrix());
	mOutlineShader->setMatrix4x4("viewMatrix", camera->getViewMatrix());
	mOutlineShader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix());
	mOutlineShader->setFloat("outlineWidth", 0.035f);

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_FALSE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glFrontFace(GL_CCW);
	glBindVertexArray(mesh->mGeometry->getVao());
	glDrawElements(GL_TRIANGLES, mesh->mGeometry->getIndicesCount(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glDepthMask(GL_TRUE);
	glCullFace(GL_BACK);
	glDisable(GL_CULL_FACE);
	mOutlineShader->end();
}

void Renderer::renderObject(
	Object* object,
	Camera* camera,
	DirectionalLight* dirLight,
	const vector<PointLight*>& pointLights,
	const vector<SpotLight*>& spotLights,
	AmbientLight* ambLight
){
	// 判断类型，如果是Mesh，则渲染
	if (object->getType() == ObjectType::Mesh || object->getType() == ObjectType::InstancedMesh)
	{
		// 解包mesh数据
		auto mesh = (Mesh*)object;
		auto geometry = mesh->mGeometry;
		// 检查是否拥有全局材质
		Material* material = nullptr;
		if (mGlobalMaterial != nullptr)
			material = mGlobalMaterial;
		else
			material = mesh->mMaterial;
		// 设置渲染状态
		setDepthState(material);
		setPolygonOffsetState(material);
		setStencilState(material);
		setBlendState(material);
		setFaceCullingState(material);
		// 决定使用的shader
		Shader* shader = pickShader(material->mType);
		// 更新uniform
		shader->begin(); // 初始化着色器
		switch (material->mType)
		{
		case MaterialType::NoLightMaterial:
		{
			NoLightMaterial* noLightMat = (NoLightMaterial*)material;
			shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix());
			shader->setMatrix4x4("viewMatrix", camera->getViewMatrix());
			shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix());
			shader->setInt("cubeSampler", 0);
			shader->setInt("sphericalSampler", 1);
			if (noLightMat->mDiffuseCube != nullptr)
			{
				mesh->setPosition(camera->mPosition);
				shader->setInt("cubeFlag", 1);
				noLightMat->mDiffuseCube->bind();
				glBindVertexArray(geometry->getVao());
				glDrawElements(GL_TRIANGLES, geometry->getIndicesCount(), GL_UNSIGNED_INT, 0);
			}
			else if (noLightMat->mDiffuseSphere != nullptr)
			{
				mesh->setPosition(camera->mPosition);
				shader->setInt("sphericalFlag", 1);
				noLightMat->mDiffuseSphere->bind();
				glBindVertexArray(geometry->getVao());
				glDrawElements(GL_TRIANGLES, geometry->getIndicesCount(), GL_UNSIGNED_INT, 0);
			}
			else
			{
				shader->setInt("cubeFlag", 0);
				shader->setInt("sphericalFlag", 0);
				glBindVertexArray(geometry->getVao());
				glDrawArrays(GL_POINTS, 0, geometry->getIndicesCount() / 3);
			}
		}
		break;
		case MaterialType::PhongMaterial:
		case MaterialType::BlinnPhongMaterial:
		{
			PhongMaterial* phongMat = (PhongMaterial*)material;
			auto normalMatrix = mat3(transpose(inverse(mesh->getModelMatrix())));
			shader->setInt("sampler", 0); // 设置漫反射纹理单元
			shader->setInt("specularMaskSampler", 1); // 设置高光蒙版纹理单元
			shader->setInt("opacityMaskSampler", 2); // 设置透明度蒙版纹理单元
			shader->setInt("envSampler", 3); // 设置环境贴图纹理单元
			phongMat->mDiffuse->bind();
			// 设置采样器
			{
				if (phongMat->mSpecularMask != nullptr)
				{
					shader->setInt("specularMaskSamplerFlag", 1);
					phongMat->mSpecularMask->bind();
				}
				else
					shader->setInt("specularMaskSamplerFlag", 0);
				if (phongMat->mOpacityMask != nullptr)
				{
					shader->setInt("opacityMaskSamplerFlag", 1);
					phongMat->mOpacityMask->bind();
				}
				else
					shader->setInt("opacityMaskSamplerFlag", 0);
				if (phongMat->mEnv != nullptr)
				{
					shader->setInt("envSamplerFlag", 1);
					phongMat->mEnv->bind();
				}
				else
				{
					shader->setInt("envSamplerFlag", 0);
					shader->setVector3("ambientColor", ambLight->mColor);
				}
			}
			// 设置变换矩阵
			shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix());
			shader->setMatrix4x4("viewMatrix", camera->getViewMatrix());
			shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix());
			shader->setMatrix3x3("normalMatrix", normalMatrix);
			if (object->getType() == ObjectType::InstancedMesh)
				shader->setInt("flag", 1);
			else
				shader->setInt("flag", 0);
			// 平行光的更新
			{
				shader->setVector3("directionalLight.color", dirLight->mColor);
				shader->setVector3("directionalLight.direction", dirLight->mDirection);
				shader->setFloat("directionalLight.specularIntensity", dirLight->mSpecularIntensity);
			}
			// 点光源的更新
			{
				shader->setInt("pointLightNum", pointLights.size());
				for (int i = 0; i < pointLights.size(); i++)
				{
					auto pointLight = pointLights[i];
					string baseName = "pointLights[";
					baseName.append(to_string(i));
					baseName.append("]");
					shader->setVector3(baseName + ".position", pointLight->getPosition());
					shader->setVector3(baseName + ".color", pointLight->mColor);
					shader->setFloat(baseName + ".specularIntensity", pointLight->mSpecularIntensity);
					shader->setFloat(baseName + ".k2", pointLight->mK2);
					shader->setFloat(baseName + ".k1", pointLight->mK1);
					shader->setFloat(baseName + ".kc", pointLight->mKc);
				}
			}
			// 聚光灯的更新
			{
				shader->setInt("spotLightNum", spotLights.size());
				for (int i = 0; i < spotLights.size(); i++)
				{
					auto spotLight = spotLights[i];
					string baseName = "spotLights[";
					baseName.append(to_string(i));
					baseName.append("]");
					shader->setVector3(baseName + ".position", spotLight->getPosition());
					shader->setVector3(baseName + ".color", spotLight->mColor);
					shader->setVector3(baseName + ".targetDirection", spotLight->mTargetDirection);
					shader->setFloat(baseName + ".specularIntensity", spotLight->mSpecularIntensity);
					shader->setFloat(baseName + ".innerLine", cos(radians(spotLight->mInnerAngle)));
					shader->setFloat(baseName + ".outerLine", cos(radians(spotLight->mOuterAngle)));
				}
			}
			// 设置灯光通用数据
			shader->setFloat("shiness", phongMat->mShiness);
			shader->setFloat("opacity", material->mOpacity);
			shader->setVector3("cameraPosition", camera->mPosition);
			// 绑定VAO
			glBindVertexArray(geometry->getVao());
			if (object->getType() == ObjectType::InstancedMesh)
			{
				InstancedMesh* im = (InstancedMesh*)mesh;
				glDrawElementsInstanced(GL_TRIANGLES, geometry->getIndicesCount(), GL_UNSIGNED_INT, 0, im->mInstanceCount);
			}
			else
				glDrawElements(GL_TRIANGLES, geometry->getIndicesCount(), GL_UNSIGNED_INT, 0);
		}
		break;
		case MaterialType::WhiteMaterial:
		{
			WhiteMaterial* whiteMat = (WhiteMaterial*)material;
			shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix());
			shader->setMatrix4x4("viewMatrix", camera->getViewMatrix());
			shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix());
			shader->setVector3("baseColor", whiteMat->mColor);
			shader->setVector3("directionalLightColor", dirLight->mColor * dirLight->mIntensity);
			shader->setVector3("directionalLightDirection", dirLight->mDirection);
			shader->setVector3("ambientColor", ambLight->mColor);
			glBindVertexArray(geometry->getVao());
			glDrawElements(GL_TRIANGLES, geometry->getIndicesCount(), GL_UNSIGNED_INT, 0);
		}
		break;
		case MaterialType::DepthMaterial:
		{
			shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix());
			shader->setMatrix4x4("viewMatrix", camera->getViewMatrix());
			shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix());
			shader->setFloat("near", camera->mNear);
			shader->setFloat("far", camera->mFar);
			glBindVertexArray(geometry->getVao());
			glDrawElements(GL_TRIANGLES, geometry->getIndicesCount(), GL_UNSIGNED_INT, 0);
		}
		break;
		case MaterialType::ScreenMaterial: 
		{
			ScreenMaterial* screenMat = (ScreenMaterial*)material;
			shader->setInt("screenTexSampler", 0);
			shader->setFloat("texWidth", screenMat->mTextureWidth);
			shader->setFloat("texHeight", screenMat->mTextureHeight);
			screenMat->mScreenTexture->bind();
			glBindVertexArray(geometry->getVao());
			glDrawElements(GL_TRIANGLES, geometry->getIndicesCount(), GL_UNSIGNED_INT, 0);
		}
		break;
		case MaterialType::GrassInstanceMaterial: 
		{
			GrassInstanceMaterial* grassMat = (GrassInstanceMaterial*)material;
			InstancedMesh* im = (InstancedMesh*)mesh;
			im->sortMatrices(camera->getViewMatrix());
			im->updateMatrices();
			shader->setInt("sampler", 0);
			grassMat->mDiffuse->bind();
			shader->setInt("opacityMask", 1);
			grassMat->mOpacityMask->bind();
			// mvp
			shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix());
			shader->setMatrix4x4("viewMatrix", camera->getViewMatrix());
			shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix());
			//光源参数的uniform更新
			//directionalLight 的更新
			shader->setVector3("directionalLight.color", dirLight->mColor * dirLight->mIntensity);
			shader->setVector3("directionalLight.direction", dirLight->mDirection);
			shader->setFloat("directionalLight.specularIntensity", dirLight->mSpecularIntensity);
			shader->setFloat("shiness", grassMat->mShiness);
			shader->setVector3("ambientColor", ambLight->mColor);
			//相机信息更新
			shader->setVector3("cameraPosition", camera->mPosition);
			//透明度
			shader->setFloat("opacity", material->mOpacity);
			//贴图特性
			shader->setFloat("uvScale", grassMat->mUVScale);
			shader->setFloat("brightness", grassMat->mBrightness);
			shader->setFloat("time", mSimulationTime);
			//风力
			shader->setFloat("windScale", grassMat->mWindScale);
			shader->setFloat("phaseScale", grassMat->mPhaseScale);
			shader->setVector3("windDirection", grassMat->mWindDirection);
			//云层
			shader->setInt("cloudMask", 2);
			grassMat->mCloudMask->bind();
			shader->setVector3("cloudWhiteColor", grassMat->mCloudWhiteColor);
			shader->setVector3("cloudBlackColor", grassMat->mCloudBlackColor);
			shader->setFloat("cloudUVScale", grassMat->mCloudUVScale);
			shader->setFloat("cloudSpeed", grassMat->mCloudSpeed);
			shader->setFloat("cloudLerp", grassMat->mCloudLerp);
			glBindVertexArray(geometry->getVao());
			if (object->getType() == ObjectType::InstancedMesh)
			{
				InstancedMesh* im = (InstancedMesh*)mesh;
				glDrawElementsInstanced(GL_TRIANGLES, geometry->getIndicesCount(), GL_UNSIGNED_INT, 0, im->mInstanceCount);
			}
			else
				glDrawElements(GL_TRIANGLES, geometry->getIndicesCount(), GL_UNSIGNED_INT, 0);
		}
		break;
		default:
			break;
		}
		// 解绑VAO
		glBindVertexArray(0);
		// 结束着色器
		shader->end();
	}
}

void Renderer::projectObject(Object* obj)
{
	if (obj->getType() == ObjectType::Mesh || obj->getType() == ObjectType::InstancedMesh)
	{
		Mesh* mesh = (Mesh*)obj;
		auto material = mesh->mMaterial;
		if (material->mBlend)
			mTransparentObjects.push_back(mesh);
		else
			mOpacityObjects.push_back(mesh);
	}

	auto children = obj->getChildren();
	for (int i = 0; i < children.size(); i++)
	{
		projectObject(children[i]);
	}
}

Shader* Renderer::pickShader(MaterialType type)
{
	Shader* result = nullptr;
	switch (type)
	{
	case MaterialType::NoLightMaterial:
		result = mNoLightShader;
		break;
	case MaterialType::PhongMaterial:
		result = mPhongShader;
		break;
	case MaterialType::BlinnPhongMaterial:
		result = mBlinnPhongShader;
		break;
	case MaterialType::WhiteMaterial:
		result = mWhiteShader;
		break;
	case MaterialType::DepthMaterial:
		result = mDepthShader;
		break;
	case MaterialType::ScreenMaterial:
		result = mScreenShader;
		break;
	case MaterialType::GrassInstanceMaterial:
		result = mGrassInstanceShader;
		break;
	default:
		cout << "找不到材质类型以挑选Shader" << endl;
		break;
	}
	return result;
}

void Renderer::setDepthState(Material* material)
{
	if (material->mDepthTest)
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(material->mDepthFunc);
	}
	else
		glDisable(GL_DEPTH_TEST);
	if (material->mDepthWrite)
		glDepthMask(GL_TRUE);
	else
		glDepthMask(GL_FALSE);
}

void Renderer::setPolygonOffsetState(Material* material)
{
	if (material->mPolygonOffset)
	{
		glEnable(material->mPolygonOffsetType);
		glPolygonOffset(material->mFactor, material->mUnit);
	}
	else
	{
		glDisable(GL_POLYGON_OFFSET_FILL);
		glDisable(GL_POLYGON_OFFSET_LINE);
	}
}

void Renderer::setStencilState(Material* material)
{
	if (material->mStencilTest)
	{
		glEnable(GL_STENCIL_TEST);
		glStencilOp(material->mSFail, material->mZFail, material->mZPass);
		glStencilMask(material->mStencilMask);
		glStencilFunc(material->mStencilFunc, material->mStencilRef, material->mStencilFuncMask);
	}
	else
		glDisable(GL_STENCIL_TEST);
}

void Renderer::setBlendState(Material* material)
{
	if (material->mBlend)
	{
		glEnable(GL_BLEND);
		glBlendFunc(material->mSFactor, material->mDFactor);
	}
	else
		glDisable(GL_BLEND);
}

void Renderer::setFaceCullingState(Material* material)
{
	if (material->mFaceCulling)
	{
		glEnable(GL_CULL_FACE);
		glFrontFace(material->mFrontFace);
		glCullFace(material->mCullFace);
	}
	else
		glDisable(GL_CULL_FACE);
}


	
