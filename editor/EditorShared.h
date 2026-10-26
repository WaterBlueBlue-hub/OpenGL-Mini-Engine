#pragma once

#include "../OpenGL.h"

enum class TimeOfDay
{
	EarlyMorning,
	Noon,
	Dusk,
	Night
};

enum class UiLanguage
{
	Chinese,
	English
};

enum class EditorSelection
{
	Scene,
	GrassField,
	DirectionalLight,
	AmbientLight,
	Camera,
	CreatedObject
};

enum class CreatedObjectKind
{
	Empty,
	Quad,
	Cube,
	Sphere,
	Plane
};

enum class TransformTool
{
	Translate,
	Rotate,
	Scale
};

enum class CenterView
{
	Scene,
	Game
};

struct EditorLayout
{
	ImVec2 toolbarPos;
	ImVec2 toolbarSize;
	ImVec2 hierarchyPos;
	ImVec2 hierarchySize;
	ImVec2 viewportPos;
	ImVec2 viewportSize;
	ImVec2 sceneViewPos;
	ImVec2 sceneViewSize;
	ImVec2 gameViewPos;
	ImVec2 gameViewSize;
	ImVec2 inspectorPos;
	ImVec2 inspectorSize;
	ImVec2 contentPos;
	ImVec2 contentSize;
};

struct CreatedObject
{
	std::string name;
	CreatedObjectKind kind{ CreatedObjectKind::Empty };
	Object* object{ nullptr };
	vec3 position{ 0.0f };
	vec3 rotation{ 0.0f };
	vec3 scale{ 1.0f };
};

extern Renderer* renderer;
extern Scene* sceneOffscreen;
extern Framebuffer* sceneViewFramebuffer;
extern Framebuffer* gameViewFramebuffer;
extern GrassInstanceMaterial* grassMaterial;
extern DirectionalLight* dirLight;
extern AmbientLight* ambLight;
extern Camera* camera;
extern PerspectiveCamera* editorCamera;
extern InstancedMesh* grassObject;

extern bool orthographicCameraMode;
extern bool dayCyclePlaying;
extern bool simulationPlaying;
extern bool simulationPaused;
extern bool simulationStepRequested;
extern bool grassSceneReady;
extern bool orbitDragging;
extern bool panDragging;
extern bool transformDragging;

extern float frameDeltaTime;
extern float transitionProgress;
extern float simulationTime;

extern vec3 grassPosition;
extern vec3 grassRotation;
extern vec3 grassScale;
extern vec3 mainCameraRotation;
extern vec3 mainCameraScale;
extern vec3 sceneFocusPoint;
extern float sceneOrbitYaw;
extern float sceneOrbitPitch;
extern float sceneOrbitDistance;

extern UiLanguage uiLanguage;
extern EditorSelection editorSelection;
extern int selectedCreatedObjectIndex;
extern TransformTool currentTransformTool;
extern CenterView activeCenterView;
extern bool sceneViewFloating;
extern bool gameViewFloating;
extern bool sceneFloatingContentVisible;
extern bool gameFloatingContentVisible;
extern bool viewportCollapsed;
extern ImVec2 sceneFloatingPos;
extern ImVec2 gameFloatingPos;
extern ImVec2 sceneFloatingContentPos;
extern ImVec2 gameFloatingContentPos;
extern ImVec2 sceneFloatingContentSize;
extern ImVec2 gameFloatingContentSize;
extern std::vector<CreatedObject> createdObjects;

const char* Tr(const char* chinese, const char* english);
EditorLayout BuildEditorLayout();
void UpdateEditorCamera();
void ResetSceneCamera();
void ApplyMainCameraRotation();
void ApplyGrassTransform();
void ApplyCreatedObjectTransform(int index);
void CreateSceneObject(CreatedObjectKind kind, const char* baseName);
void LoadScenePreset(const std::string& assetPath);
void SetStaticTimeOfDay(TimeOfDay time);
void StartSimulation();
void StopSimulation();
void PauseSimulation();
void StepSimulation();
void ToggleProjection();
void DrawInspector(const EditorLayout& layout);
void FocusSelection();
void HandleSceneViewInput(const EditorLayout& layout);
void DrawOrientationGizmo(const ImVec2& viewportPosition, const ImVec2& viewportSize);
void DrawViewport(const EditorLayout& layout);
void InitializeContentBrowser();
void DrawEditorUI(const EditorLayout& layout);
void DrawContentBrowser(const EditorLayout& layout);

