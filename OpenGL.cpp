#include "OpenGL.h"
#include "editor/EditorShared.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

Renderer* renderer = nullptr;
Scene* sceneOffscreen = nullptr;
Framebuffer* sceneViewFramebuffer = nullptr;
Framebuffer* gameViewFramebuffer = nullptr;
GrassInstanceMaterial* grassMaterial = nullptr;
DirectionalLight* dirLight = nullptr;
AmbientLight* ambLight = nullptr;
Camera* camera = nullptr;
PerspectiveCamera* editorCamera = nullptr;
InstancedMesh* grassObject = nullptr;

const int WIDTH = 1600;
const int HEIGHT = 900;
vec3 clearColor{ 0.52f, 0.68f, 0.86f };

bool orthographicCameraMode = false;
bool dayCyclePlaying = false;
bool simulationPlaying = false;
bool simulationPaused = false;
bool simulationStepRequested = false;
bool grassSceneReady = false;
bool orbitDragging = false;
bool panDragging = false;
bool transformDragging = false;
float frameDeltaTime = 0.0f;
float transitionProgress = 1.0f;
const float transitionDuration = 4.0f;
float simulationTime = 0.0f;
const float simulationFixedStep = 1.0f / 60.0f;

vec3 grassPosition(0.0f);
vec3 grassRotation(0.0f);
vec3 grassScale(1.0f);

vec3 mainCameraRotation(-29.5f, -45.0f, 0.0f);
vec3 mainCameraScale(1.0f);

vec3 sceneFocusPoint(0.0f, 0.3f, 0.0f);
float sceneOrbitYaw = 45.0f;
float sceneOrbitPitch = 28.0f;
float sceneOrbitDistance = 9.0f;

TimeOfDay currentTime = TimeOfDay::Noon;
TimeOfDay targetTime = TimeOfDay::Dusk;
UiLanguage uiLanguage = UiLanguage::Chinese;

const char* Tr(const char* chinese, const char* english)
{
	return uiLanguage == UiLanguage::Chinese ? chinese : english;
}

EditorSelection editorSelection = EditorSelection::Scene;
int selectedCreatedObjectIndex = -1;
TransformTool currentTransformTool = TransformTool::Translate;
CenterView activeCenterView = CenterView::Scene;
bool sceneViewFloating = false;
bool gameViewFloating = false;
bool sceneFloatingContentVisible = true;
bool gameFloatingContentVisible = true;
bool viewportCollapsed = false;
ImVec2 sceneFloatingPos(360.0f, 170.0f);
ImVec2 gameFloatingPos(430.0f, 210.0f);
ImVec2 sceneFloatingContentPos(360.0f, 200.0f);
ImVec2 gameFloatingContentPos(430.0f, 240.0f);
ImVec2 sceneFloatingContentSize(720.0f, 400.0f);
ImVec2 gameFloatingContentSize(720.0f, 400.0f);
std::vector<CreatedObject> createdObjects;

void ResetSceneCamera();
void UpdateEditorCamera();
void FocusSelection();
void ApplyMainCameraRotation();

void SetSceneCameraPose(Camera* targetCamera)
{
	const vec3 position(4.8f, 2.4f, 4.8f);
	const vec3 lookTarget(0.0f, 0.35f, 0.0f);
	const vec3 worldUp(0.0f, 1.0f, 0.0f);
	const vec3 front = normalize(lookTarget - position);

	targetCamera->mPosition = position;
	targetCamera->mRight = normalize(cross(front, worldUp));
	targetCamera->mUp = normalize(cross(targetCamera->mRight, front));

	const float cameraPitch = degrees(asin(std::clamp(front.y, -1.0f, 1.0f)));
	const float cameraYaw = degrees(atan2(front.x, -front.z));
	mainCameraRotation = vec3(cameraPitch, cameraYaw, 0.0f);
}

void ApplyMainCameraRotation()
{
	const float pitch = radians(mainCameraRotation.x);
	const float yaw = radians(mainCameraRotation.y);
	const vec3 worldUp(0.0f, 1.0f, 0.0f);
	const vec3 front = normalize(vec3(
		cos(pitch) * sin(yaw),
		sin(pitch),
		-cos(pitch) * cos(yaw)
	));

	camera->mRight = normalize(cross(front, worldUp));
	camera->mUp = normalize(cross(camera->mRight, front));

	if (std::abs(mainCameraRotation.z) > 0.0001f)
	{
		const mat4 roll = rotate(mat4(1.0f), radians(mainCameraRotation.z), front);
		camera->mRight = normalize(vec3(roll * vec4(camera->mRight, 0.0f)));
		camera->mUp = normalize(vec3(roll * vec4(camera->mUp, 0.0f)));
	}
}

void ResetSceneCamera()
{
	SetSceneCameraPose(camera);
	sceneFocusPoint = vec3(0.0f, 0.3f, 0.0f);
	sceneOrbitYaw = 45.0f;
	sceneOrbitPitch = 28.0f;
	sceneOrbitDistance = 9.0f;
	UpdateEditorCamera();
}

void SetCameraLookAt(Camera* targetCamera, const vec3& position, const vec3& target)
{
	const vec3 worldUp(0.0f, 1.0f, 0.0f);
	const vec3 front = normalize(target - position);
	targetCamera->mPosition = position;
	targetCamera->mRight = normalize(cross(front, worldUp));
	targetCamera->mUp = normalize(cross(targetCamera->mRight, front));
}

void UpdateEditorCamera()
{
	const float yaw = radians(sceneOrbitYaw);
	const float pitch = radians(sceneOrbitPitch);
	const vec3 offset(
		cos(pitch) * sin(yaw),
		sin(pitch),
		cos(pitch) * cos(yaw)
	);
	SetCameraLookAt(editorCamera, sceneFocusPoint + offset * sceneOrbitDistance, sceneFocusPoint);
}

void UpdateFramebuffer(Framebuffer*& target, int width, int height)
{
	if (width <= 0 || height <= 0)
		return;

	if (target != nullptr &&
		target->mWidth == static_cast<unsigned int>(width) &&
		target->mHeight == static_cast<unsigned int>(height))
		return;

	delete target;
	target = new Framebuffer(width, height);
}

void OnResize(int width, int height)
{
	if (width <= 0 || height <= 0)
		return;

	glViewport(0, 0, width, height);
}

void OnEditorKey(int key, int action, int mods)
{
	if (action != GLFW_PRESS)
		return;
	if (ImGui::GetCurrentContext() != nullptr && ImGui::GetIO().WantTextInput)
		return;

	switch (key)
	{
	case GLFW_KEY_W:
		currentTransformTool = TransformTool::Translate;
		break;
	case GLFW_KEY_E:
		currentTransformTool = TransformTool::Rotate;
		break;
	case GLFW_KEY_R:
		currentTransformTool = TransformTool::Scale;
		break;
	case GLFW_KEY_F:
		FocusSelection();
		break;
	default:
		break;
	}
}

void SetInstanceMatrix(Object* object, int index, const mat4& matrix)
{
	if (object->getType() == ObjectType::InstancedMesh)
	{
		InstancedMesh* instancedMesh = static_cast<InstancedMesh*>(object);
		instancedMesh->mInstanceMatrices[index] = matrix;
	}

	for (Object* child : object->getChildren())
		SetInstanceMatrix(child, index, matrix);
}

void UpdateInstanceMatrices(Object* object)
{
	if (object->getType() == ObjectType::InstancedMesh)
	{
		static_cast<InstancedMesh*>(object)->updateMatrices();
	}

	for (Object* child : object->getChildren())
		UpdateInstanceMatrices(child);
}

void SetInstanceMaterial(Object* object, Material* material)
{
	if (object->getType() == ObjectType::InstancedMesh)
	{
		static_cast<InstancedMesh*>(object)->mMaterial = material;
	}

	for (Object* child : object->getChildren())
		SetInstanceMaterial(child, material);
}

void DrawGrass(int rowCount, int columnCount)
{
	InstancedMesh* grassModel = SimpleObjLoader::loadInstanced(
		"assets/Assets/Models/grassNew.obj",
		rowCount * columnCount
	);
	if (grassModel == nullptr)
	{
		cerr << "Grass model is missing. Place grassNew.obj under assets/Assets/Models/." << endl;
		return;
	}

	grassMaterial = new GrassInstanceMaterial();
	const float spacing = 0.22f;
	const float halfWidth = (columnCount - 1) * spacing * 0.5f;
	const float halfDepth = (rowCount - 1) * spacing * 0.5f;
	srand(static_cast<unsigned int>(glfwGetTime()));

	for (int row = 0; row < rowCount; ++row)
	{
		for (int column = 0; column < columnCount; ++column)
		{
			const vec3 position(
				column * spacing - halfWidth,
				0.0f,
				row * spacing - halfDepth
			);
			const mat4 translate = glm::translate(mat4(1.0f), position);
			const mat4 rotate = glm::rotate(
				radians(static_cast<float>(rand() % 360)),
				vec3(0.0f, 1.0f, 0.0f)
			);
			const float scaleValue = 0.85f + static_cast<float>(rand() % 31) / 100.0f;
			const mat4 scaleMatrix = glm::scale(mat4(1.0f), vec3(scaleValue));
			SetInstanceMatrix(grassModel, row * columnCount + column, translate * rotate * scaleMatrix);
		}
	}
	UpdateInstanceMatrices(grassModel);

	grassMaterial->mDiffuse = Texture::createTexture("assets/Assets/Textures/GRASS.PNG", 0);
	grassMaterial->mOpacityMask = Texture::createTexture("assets/Assets/Textures/grassMask.png", 1);
	grassMaterial->mCloudMask = Texture::createTexture("assets/Assets/Textures/CLOUD.PNG", 2);
	grassMaterial->mBlend = true;
	grassMaterial->mDepthWrite = false;
	grassMaterial->mUVScale = 32.0f;
	grassMaterial->mBrightness = 1.15f;
	grassMaterial->mWindScale = 0.08f;
	grassMaterial->mPhaseScale = 0.7f;
	grassMaterial->mCloudLerp = 0.18f;
	grassMaterial->mCloudUVScale = 2.5f;
	grassMaterial->mCloudSpeed = 0.12f;
	SetInstanceMaterial(grassModel, grassMaterial);
	sceneOffscreen->addChild(grassModel);
	grassObject = grassModel;
	grassSceneReady = true;
}

Geometry* CreateQuadGeometry()
{
	const vector<float> positions = {
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.5f,  0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f
	};
	const vector<float> normals = {
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f
	};
	const vector<float> uvs = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f
	};
	const vector<unsigned int> indices = { 0, 1, 2, 0, 2, 3 };
	return new Geometry(positions, normals, uvs, indices);
}

std::string MakeUniqueObjectName(const char* baseName)
{
	const std::string base(baseName);
	std::string candidate = base;
	int suffix = 1;
	while (std::any_of(
		createdObjects.begin(),
		createdObjects.end(),
		[&](const CreatedObject& object) { return object.name == candidate; }
	))
	{
		candidate = base + " (" + std::to_string(suffix++) + ")";
	}
	return candidate;
}

void ApplyCreatedObjectTransform(int index)
{
	if (index < 0 || index >= static_cast<int>(createdObjects.size()))
		return;

	CreatedObject& created = createdObjects[index];
	created.object->setPosition(created.position);
	created.object->setAngleX(created.rotation.x);
	created.object->setAngleY(created.rotation.y);
	created.object->setAngleZ(created.rotation.z);
	created.object->setScale(created.scale);
}

void CreateSceneObject(CreatedObjectKind kind, const char* baseName)
{
	const int slot = static_cast<int>(createdObjects.size());
	const vec3 spawnPosition = sceneFocusPoint + vec3(
		(static_cast<float>(slot % 3) - 1.0f) * 1.25f,
		1.35f,
		(static_cast<float>((slot / 3) % 3) - 1.0f) * 1.25f
	);

	Object* object = nullptr;
	Geometry* geometry = nullptr;
	switch (kind)
	{
	case CreatedObjectKind::Empty:
		object = new Object();
		break;
	case CreatedObjectKind::Quad:
		geometry = CreateQuadGeometry();
		break;
	case CreatedObjectKind::Cube:
		geometry = Geometry::createBox(1.0f);
		break;
	case CreatedObjectKind::Sphere:
		geometry = Geometry::createSphere(0.5f);
		break;
	case CreatedObjectKind::Plane:
		geometry = Geometry::createPlane(1.5f, 1.5f, vec3(0.0f, 1.0f, 0.0f));
		break;
	}

	if (geometry != nullptr)
	{
		auto* material = new WhiteMaterial();
		object = new Mesh(geometry, material);
	}

	if (object == nullptr)
		return;

	CreatedObject created;
	created.name = MakeUniqueObjectName(baseName);
	created.kind = kind;
	created.object = object;
	created.position = spawnPosition;
	created.scale = vec3(1.0f);
	createdObjects.push_back(created);

	sceneOffscreen->addChild(object);
	ApplyCreatedObjectTransform(static_cast<int>(createdObjects.size()) - 1);
	selectedCreatedObjectIndex = static_cast<int>(createdObjects.size()) - 1;
	editorSelection = EditorSelection::CreatedObject;
}

void ApplyLighting(TimeOfDay from, TimeOfDay to, float t)
{
	const float clampedT = std::clamp(t, 0.0f, 1.0f);
	switch (from)
	{
	case TimeOfDay::EarlyMorning:
		dirLight->mIntensity = lerp(0.3f, to == TimeOfDay::Noon ? 1.0f : 0.3f, clampedT);
		dirLight->mColor = lerp(
			vec3(1.0f, 0.875f, 0.729f),
			to == TimeOfDay::Noon ? vec3(1.0f) : vec3(1.0f, 0.369f, 0.302f),
			clampedT
		);
		if (grassMaterial != nullptr)
			grassMaterial->mCloudBlackColor = lerp(
				vec3(0.552f, 0.475f, 0.176f),
				to == TimeOfDay::Noon ? vec3(0.545f, 0.243f, 0.243f) : vec3(0.0f),
				clampedT
			);
		break;
	case TimeOfDay::Noon:
		dirLight->mIntensity = lerp(1.0f, to == TimeOfDay::Dusk ? 0.3f : 1.0f, clampedT);
		dirLight->mColor = lerp(
			vec3(1.0f),
			to == TimeOfDay::Dusk ? vec3(1.0f, 0.369f, 0.302f) : vec3(1.0f),
			clampedT
		);
		if (grassMaterial != nullptr)
			grassMaterial->mCloudBlackColor = lerp(
				vec3(0.545f, 0.243f, 0.243f),
				to == TimeOfDay::Dusk ? vec3(0.994f, 0.3f, 0.426f) : vec3(0.545f, 0.243f, 0.243f),
				clampedT
			);
		break;
	case TimeOfDay::Dusk:
		dirLight->mIntensity = lerp(0.3f, to == TimeOfDay::Night ? 0.0f : 0.3f, clampedT);
		dirLight->mColor = lerp(
			vec3(1.0f, 0.369f, 0.302f),
			to == TimeOfDay::Night ? vec3(0.196f, 0.196f, 0.392f) : vec3(1.0f, 0.369f, 0.302f),
			clampedT
		);
		if (grassMaterial != nullptr)
			grassMaterial->mCloudBlackColor = lerp(
				vec3(0.994f, 0.3f, 0.426f),
				to == TimeOfDay::Night ? vec3(0.0f) : vec3(0.994f, 0.3f, 0.426f),
				clampedT
			);
		break;
	case TimeOfDay::Night:
		dirLight->mIntensity = lerp(0.0f, to == TimeOfDay::EarlyMorning ? 0.3f : 0.0f, clampedT);
		dirLight->mColor = lerp(
			vec3(0.196f, 0.196f, 0.392f),
			to == TimeOfDay::EarlyMorning ? vec3(1.0f, 0.875f, 0.729f) : vec3(0.196f, 0.196f, 0.392f),
			clampedT
		);
		if (grassMaterial != nullptr)
			grassMaterial->mCloudBlackColor = lerp(
				vec3(0.0f),
				to == TimeOfDay::EarlyMorning ? vec3(0.552f, 0.475f, 0.176f) : vec3(0.0f),
				clampedT
			);
		break;
	}
}

void SetStaticTimeOfDay(TimeOfDay time)
{
	dayCyclePlaying = false;
	currentTime = time;
	targetTime = time;
	transitionProgress = 1.0f;
	ApplyLighting(time, time, 0.0f);
}

void LoadScenePreset(const std::string& assetPath)
{
	std::ifstream input(assetPath);
	if (!input)
	{
		cerr << "Scene preset is missing: " << assetPath << endl;
		return;
	}

	std::string sceneType;
	int rows = 25;
	int columns = 25;
	std::string line;
	while (std::getline(input, line))
	{
		const std::size_t separator = line.find('=');
		if (separator == std::string::npos)
			continue;

		const std::string key = line.substr(0, separator);
		const std::string value = line.substr(separator + 1);
		try
		{
			if (key == "type")
				sceneType = value;
			else if (key == "rows")
				rows = std::clamp(std::stoi(value), 1, 100);
			else if (key == "columns")
				columns = std::clamp(std::stoi(value), 1, 100);
		}
		catch (const std::exception&)
		{
			cerr << "Invalid scene preset value: " << line << endl;
			return;
		}
	}

	if (sceneType != "grass")
	{
		cerr << "Unsupported scene preset: " << assetPath << endl;
		return;
	}

	if (!grassSceneReady)
		DrawGrass(rows, columns);
	if (!grassSceneReady)
		return;

	SetStaticTimeOfDay(TimeOfDay::Noon);
	editorSelection = EditorSelection::GrassField;
	sceneFocusPoint = vec3(0.0f, 0.3f, 0.0f);
	sceneOrbitDistance = 6.5f;
	UpdateEditorCamera();
}

void StartDayCycle()
{
	currentTime = TimeOfDay::EarlyMorning;
	targetTime = TimeOfDay::Noon;
	transitionProgress = 0.0f;
	dayCyclePlaying = true;
}

TimeOfDay NextTimeOfDay(TimeOfDay time)
{
	switch (time)
	{
	case TimeOfDay::EarlyMorning:
		return TimeOfDay::Noon;
	case TimeOfDay::Noon:
		return TimeOfDay::Dusk;
	case TimeOfDay::Dusk:
		return TimeOfDay::Night;
	case TimeOfDay::Night:
		return TimeOfDay::EarlyMorning;
	}
	return TimeOfDay::Noon;
}

void UpdateDayCycle(float deltaTime)
{
	if (!dayCyclePlaying)
		return;

	transitionProgress += deltaTime / transitionDuration;
	if (transitionProgress >= 1.0f)
	{
		currentTime = targetTime;
		targetTime = NextTimeOfDay(currentTime);
		transitionProgress = 0.0f;
	}
	ApplyLighting(currentTime, targetTime, transitionProgress);
}

void StartSimulation()
{
	if (!simulationPlaying)
	{
		simulationPlaying = true;
		simulationPaused = false;
		simulationTime = 0.0f;
		StartDayCycle();
	}
	else
	{
		simulationPaused = false;
	}
}

void PauseSimulation()
{
	if (simulationPlaying)
		simulationPaused = true;
}

void StepSimulation()
{
	if (!simulationPlaying)
	{
		simulationPlaying = true;
		simulationPaused = true;
		simulationTime = 0.0f;
		StartDayCycle();
	}
	else
	{
		simulationPaused = true;
	}
	simulationStepRequested = true;
}

void UpdateSimulation()
{
	float deltaTime = 0.0f;
	if (simulationPlaying && !simulationPaused)
		deltaTime = frameDeltaTime;
	if (simulationStepRequested)
	{
		deltaTime = simulationFixedStep;
		simulationStepRequested = false;
	}
	if (deltaTime <= 0.0f)
		return;

	simulationTime += deltaTime;
	UpdateDayCycle(deltaTime);
}

void ToggleProjection()
{
	Camera* replacement = nullptr;
	if (!orthographicCameraMode)
	{
		replacement = new OrthographicCamera(-4.0f, 4.0f, 2.25f, -2.25f, 0.1f, 100.0f);
	}
	else
	{
		replacement = new PerspectiveCamera(
			50.0f,
			static_cast<float>(glApp->getWidth()) / static_cast<float>(glApp->getHeight()),
			0.1f,
			100.0f
		);
	}

	replacement->mPosition = camera->mPosition;
	replacement->mUp = camera->mUp;
	replacement->mRight = camera->mRight;
	delete camera;
	camera = replacement;
	orthographicCameraMode = !orthographicCameraMode;
}


void PrepareRenderer()
{
	renderer = new Renderer();
	sceneOffscreen = new Scene();
	sceneViewFramebuffer = new Framebuffer(WIDTH / 2, HEIGHT);
	gameViewFramebuffer = new Framebuffer(WIDTH / 3, HEIGHT);

	dirLight = new DirectionalLight();
	dirLight->mDirection = normalize(vec3(-0.45f, -1.0f, -0.25f));
	dirLight->mColor = vec3(1.0f);
	dirLight->mSpecularIntensity = 0.15f;
	ambLight = new AmbientLight();
	ambLight->mColor = vec3(0.24f);
	SetStaticTimeOfDay(TimeOfDay::Noon);
}

void PrepareCamera()
{
	camera = new PerspectiveCamera(
		50.0f,
		static_cast<float>(WIDTH) / static_cast<float>(HEIGHT),
		0.1f,
		100.0f
	);
	SetSceneCameraPose(camera);

	editorCamera = new PerspectiveCamera(
		50.0f,
		static_cast<float>(WIDTH) / static_cast<float>(HEIGHT),
		0.1f,
		100.0f
	);
	UpdateEditorCamera();
}

void InitializeImGui()
{
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGuiIO& io = ImGui::GetIO();
	const char* fontCandidates[] = {
		"C:/Windows/Fonts/msyh.ttc",
		"C:/Windows/Fonts/simhei.ttf",
		"C:/Windows/Fonts/simsun.ttc",
		"C:/Windows/Fonts/Deng.ttf"
	};
	bool fontLoaded = false;
	for (const char* fontPath : fontCandidates)
	{
		std::error_code error;
		if (!std::filesystem::exists(fontPath, error))
			continue;

		ImFontConfig fontConfig;
		fontConfig.OversampleH = 2;
		fontConfig.OversampleV = 2;
		fontConfig.PixelSnapH = true;
		if (io.Fonts->AddFontFromFileTTF(
			fontPath,
			17.0f,
			&fontConfig,
			io.Fonts->GetGlyphRangesChineseSimplifiedCommon()
		) != nullptr)
		{
			fontLoaded = true;
			break;
		}
	}
	if (!fontLoaded)
		io.Fonts->AddFontDefault();

	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowRounding = 0.0f;
	style.FrameRounding = 3.0f;
	style.GrabRounding = 3.0f;
	style.WindowBorderSize = 1.0f;
	style.FrameBorderSize = 0.0f;
	style.WindowPadding = ImVec2(10.0f, 9.0f);
	style.ItemSpacing = ImVec2(8.0f, 7.0f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.075f, 0.085f, 0.105f, 1.0f);
	style.Colors[ImGuiCol_ChildBg] = ImVec4(0.065f, 0.075f, 0.092f, 1.0f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.18f, 0.21f, 0.26f, 1.0f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.18f, 0.31f, 0.48f, 1.0f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.24f, 0.42f, 0.64f, 1.0f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.16f, 0.21f, 0.29f, 1.0f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.22f, 0.34f, 0.50f, 1.0f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.26f, 0.44f, 0.68f, 1.0f);
	ImGui_ImplGlfw_InitForOpenGL(glApp->getWindow(), true);
	ImGui_ImplOpenGL3_Init("#version 330");
	InitializeContentBrowser();
}

void ShutdownImGui()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

int main()
{
	if (!glApp->Init(WIDTH, HEIGHT))
		return EXIT_FAILURE;

	glApp->setResizeCallback(OnResize);
	glApp->setKeyBoradCallback(OnEditorKey);

	glViewport(0, 0, WIDTH, HEIGHT);
	glClearColor(clearColor.r, clearColor.g, clearColor.b, 1.0f);

	PrepareCamera();
	PrepareRenderer();
	InitializeImGui();

	double previousTime = glfwGetTime();
	while (glApp->Updata())
	{
		const double currentFrameTime = glfwGetTime();
		frameDeltaTime = std::min(static_cast<float>(currentFrameTime - previousTime), 0.05f);
		previousTime = currentFrameTime;

		UpdateSimulation();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		const EditorLayout layout = BuildEditorLayout();
		const ImGuiIO& io = ImGui::GetIO();
		HandleSceneViewInput(layout);

		const bool sceneViewNeeded =
			(sceneViewFloating && sceneFloatingContentVisible) ||
			(activeCenterView == CenterView::Scene && !viewportCollapsed);
		const bool gameViewNeeded =
			(gameViewFloating && gameFloatingContentVisible) ||
			(activeCenterView == CenterView::Game && !viewportCollapsed);
		const ImVec2 sceneRenderSize = sceneViewFloating
			? sceneFloatingContentSize
			: layout.sceneViewSize;
		const ImVec2 gameRenderSize = gameViewFloating
			? gameFloatingContentSize
			: layout.gameViewSize;
		const int sceneViewportWidth = std::max(
			1,
			static_cast<int>(sceneRenderSize.x * io.DisplayFramebufferScale.x)
		);
		const int sceneViewportHeight = std::max(
			1,
			static_cast<int>(sceneRenderSize.y * io.DisplayFramebufferScale.y)
		);
		const int gameViewportWidth = std::max(
			1,
			static_cast<int>(gameRenderSize.x * io.DisplayFramebufferScale.x)
		);
		const int gameViewportHeight = std::max(
			1,
			static_cast<int>(gameRenderSize.y * io.DisplayFramebufferScale.y)
		);
		if (sceneViewNeeded)
			UpdateFramebuffer(sceneViewFramebuffer, sceneViewportWidth, sceneViewportHeight);
		if (gameViewNeeded)
			UpdateFramebuffer(gameViewFramebuffer, gameViewportWidth, gameViewportHeight);

		if (gameViewNeeded && !orthographicCameraMode && camera != nullptr)
		{
			auto perspectiveCamera = dynamic_cast<PerspectiveCamera*>(camera);
			if (perspectiveCamera != nullptr)
				perspectiveCamera->setAspect(static_cast<float>(gameViewportWidth) / gameViewportHeight);
		}
		if (sceneViewNeeded)
			editorCamera->setAspect(static_cast<float>(sceneViewportWidth) / sceneViewportHeight);

		Object* outlineObject = nullptr;
		if (editorSelection == EditorSelection::CreatedObject &&
			selectedCreatedObjectIndex >= 0 &&
			selectedCreatedObjectIndex < static_cast<int>(createdObjects.size()))
		{
			outlineObject = createdObjects[selectedCreatedObjectIndex].object;
		}

		renderer->setSimulationTime(simulationTime);
		renderer->setClearColor(clearColor);
		if (sceneViewNeeded)
		{
			renderer->setSelectionObject(outlineObject);
			glBindFramebuffer(GL_FRAMEBUFFER, sceneViewFramebuffer->mFBO);
			glViewport(0, 0, sceneViewFramebuffer->mWidth, sceneViewFramebuffer->mHeight);
			renderer->render(sceneOffscreen, editorCamera, dirLight, {}, {}, ambLight, sceneViewFramebuffer->mFBO);
		}

		if (gameViewNeeded)
		{
			renderer->setSelectionObject(nullptr);
			glBindFramebuffer(GL_FRAMEBUFFER, gameViewFramebuffer->mFBO);
			glViewport(0, 0, gameViewFramebuffer->mWidth, gameViewFramebuffer->mHeight);
			renderer->render(sceneOffscreen, camera, dirLight, {}, {}, ambLight, gameViewFramebuffer->mFBO);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		int windowWidth = 0;
		int windowHeight = 0;
		glfwGetFramebufferSize(glApp->getWindow(), &windowWidth, &windowHeight);
		glViewport(0, 0, windowWidth, windowHeight);
		glClearColor(0.045f, 0.052f, 0.065f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		DrawEditorUI(layout);
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glApp->SwapBuffers();
	}

	ShutdownImGui();
	glApp->Destroy();
	return EXIT_SUCCESS;
}
