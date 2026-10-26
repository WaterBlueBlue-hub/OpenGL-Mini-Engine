#include "EditorShared.h"

#include <algorithm>
#include <cmath>
#include <cstdio>

const ImGuiWindowFlags PanelFlags =
	ImGuiWindowFlags_NoTitleBar |
	ImGuiWindowFlags_NoResize |
	ImGuiWindowFlags_NoMove |
	ImGuiWindowFlags_NoCollapse |
	ImGuiWindowFlags_NoSavedSettings |
	ImGuiWindowFlags_NoBringToFrontOnFocus;

vec3 GetSelectionWorldPosition(EditorSelection selection)
{
	switch (selection)
	{
	case EditorSelection::GrassField:
		return grassPosition + vec3(0.0f, 0.3f, 0.0f);
	case EditorSelection::DirectionalLight:
		return vec3(2.2f, 2.4f, 1.2f);
	case EditorSelection::AmbientLight:
		return vec3(-2.2f, 1.8f, -1.2f);
	case EditorSelection::Camera:
		return camera->mPosition;
	case EditorSelection::CreatedObject:
		if (selectedCreatedObjectIndex >= 0 &&
			selectedCreatedObjectIndex < static_cast<int>(createdObjects.size()))
		{
			return createdObjects[selectedCreatedObjectIndex].position;
		}
		return sceneFocusPoint;
	case EditorSelection::Scene:
		return sceneFocusPoint;
	}
	return vec3(0.0f);
}

const char* GetSelectionDisplayName(EditorSelection selection)
{
	switch (selection)
	{
	case EditorSelection::GrassField:
		return Tr("草地", "Grass Field");
	case EditorSelection::DirectionalLight:
		return "Directional Light";
	case EditorSelection::AmbientLight:
		return "Ambient Light";
	case EditorSelection::Camera:
		return "Main Camera";
	case EditorSelection::CreatedObject:
		if (selectedCreatedObjectIndex >= 0 &&
			selectedCreatedObjectIndex < static_cast<int>(createdObjects.size()))
		{
			return createdObjects[selectedCreatedObjectIndex].name.c_str();
		}
		return "GameObject";
	case EditorSelection::Scene:
		return Tr("场景", "Scene");
	}
	return "";
}

bool ProjectWorldToScreen(
	const vec3& worldPosition,
	Camera* viewCamera,
	const ImVec2& viewportPosition,
	const ImVec2& viewportSize,
	ImVec2& screenPosition
)
{
	const vec4 clip = viewCamera->getProjectionMatrix() * viewCamera->getViewMatrix() * vec4(worldPosition, 1.0f);
	if (clip.w <= 0.0f)
		return false;

	const vec3 ndc = vec3(clip) / clip.w;
	if (ndc.x < -1.2f || ndc.x > 1.2f || ndc.y < -1.2f || ndc.y > 1.2f)
		return false;

	screenPosition = ImVec2(
		viewportPosition.x + (ndc.x * 0.5f + 0.5f) * viewportSize.x,
		viewportPosition.y + (0.5f - ndc.y * 0.5f) * viewportSize.y
	);
	return true;
}

bool ScreenPointNear(const ImVec2& point, const ImVec2& target, float radius)
{
	const float dx = point.x - target.x;
	const float dy = point.y - target.y;
	return dx * dx + dy * dy <= radius * radius;
}

bool RayIntersectsSelection(
	const ImVec2& mouse,
	const ImVec2& viewportPosition,
	const ImVec2& viewportSize,
	EditorSelection selection
)
{
	if (selection == EditorSelection::Scene)
		return false;
	if (selection == EditorSelection::GrassField && grassObject == nullptr)
		return false;

	ImVec2 iconPosition;
	if (selection != EditorSelection::GrassField &&
		ProjectWorldToScreen(
			GetSelectionWorldPosition(selection),
			editorCamera,
			viewportPosition,
			viewportSize,
			iconPosition
		))
	{
		if (ScreenPointNear(mouse, iconPosition, 18.0f))
			return true;
	}

	const float localX = (mouse.x - viewportPosition.x) / viewportSize.x;
	const float localY = (mouse.y - viewportPosition.y) / viewportSize.y;
	const float ndcX = localX * 2.0f - 1.0f;
	const float ndcY = 1.0f - localY * 2.0f;
	const mat4 inverseViewProjection =
		inverse(editorCamera->getProjectionMatrix() * editorCamera->getViewMatrix());
	vec4 nearPoint = inverseViewProjection * vec4(ndcX, ndcY, -1.0f, 1.0f);
	vec4 farPoint = inverseViewProjection * vec4(ndcX, ndcY, 1.0f, 1.0f);
	nearPoint /= nearPoint.w;
	farPoint /= farPoint.w;

	const vec3 rayOrigin(nearPoint);
	const vec3 rayDirection = normalize(vec3(farPoint) - rayOrigin);
	const vec3 center = GetSelectionWorldPosition(selection);
	const float radius = selection == EditorSelection::GrassField
		? 3.4f * std::max(grassScale.x, 1.0f)
		: selection == EditorSelection::CreatedObject ? 0.5f : 0.35f;
	const vec3 offset = rayOrigin - center;
	const float b = dot(offset, rayDirection);
	const float c = dot(offset, offset) - radius * radius;
	const float discriminant = b * b - c;
	if (discriminant < 0.0f)
		return false;

	const float hitDistance = -b - std::sqrt(discriminant);
	return hitDistance >= 0.0f;
}

void PickSceneObject(
	const ImVec2& viewportPosition,
	const ImVec2& viewportSize,
	const ImVec2& mouse
)
{
	const EditorSelection candidates[] = {
		EditorSelection::CreatedObject,
		EditorSelection::Camera,
		EditorSelection::DirectionalLight,
		EditorSelection::AmbientLight,
		EditorSelection::GrassField
	};

	for (EditorSelection candidate : candidates)
	{
		if (candidate == EditorSelection::CreatedObject)
		{
			for (int i = static_cast<int>(createdObjects.size()) - 1; i >= 0; --i)
			{
				ImVec2 iconPosition;
				if (ProjectWorldToScreen(
					createdObjects[i].position,
					editorCamera,
					viewportPosition,
					viewportSize,
					iconPosition
				) &&
					ScreenPointNear(mouse, iconPosition, 20.0f))
				{
					selectedCreatedObjectIndex = i;
					editorSelection = EditorSelection::CreatedObject;
					return;
				}
			}
			continue;
		}

		if (RayIntersectsSelection(mouse, viewportPosition, viewportSize, candidate))
		{
			editorSelection = candidate;
			return;
		}
	}
	editorSelection = EditorSelection::Scene;
}

void ApplyGrassTransform()
{
	if (grassObject == nullptr)
		return;

	grassObject->setPosition(grassPosition);
	grassObject->setAngleX(grassRotation.x);
	grassObject->setAngleY(grassRotation.y);
	grassObject->setAngleZ(grassRotation.z);
	grassObject->setScale(grassScale);
}

void ApplySelectionTransform(float deltaX, float deltaY)
{
	if (editorSelection == EditorSelection::Scene ||
		editorSelection == EditorSelection::AmbientLight)
	{
		return;
	}

	if (currentTransformTool == TransformTool::Translate)
	{
		const vec3 translation =
			(editorCamera->mRight * deltaX - editorCamera->mUp * deltaY) *
			std::max(0.002f, sceneOrbitDistance * 0.0012f);
		if (editorSelection == EditorSelection::GrassField)
			grassPosition += translation;
		else if (editorSelection == EditorSelection::Camera)
			camera->mPosition += translation;
		else if (editorSelection == EditorSelection::CreatedObject &&
			selectedCreatedObjectIndex >= 0 &&
			selectedCreatedObjectIndex < static_cast<int>(createdObjects.size()))
		{
			createdObjects[selectedCreatedObjectIndex].position += translation;
		}
	}
	else if (currentTransformTool == TransformTool::Rotate)
	{
		const float rotation = -deltaX * 0.5f;
		if (editorSelection == EditorSelection::GrassField)
			grassRotation.y += rotation;
		else if (editorSelection == EditorSelection::Camera)
		{
			mainCameraRotation.y += rotation;
			ApplyMainCameraRotation();
		}
		else if (editorSelection == EditorSelection::CreatedObject &&
			selectedCreatedObjectIndex >= 0 &&
			selectedCreatedObjectIndex < static_cast<int>(createdObjects.size()))
		{
			createdObjects[selectedCreatedObjectIndex].rotation.y += rotation;
		}
		else if (editorSelection == EditorSelection::DirectionalLight)
		{
			const mat4 matrix = rotate(mat4(1.0f), radians(rotation), vec3(0.0f, 1.0f, 0.0f));
			dirLight->mDirection = normalize(vec3(matrix * vec4(dirLight->mDirection, 0.0f)));
		}
	}
	else if (editorSelection == EditorSelection::GrassField)
	{
		const float factor = std::clamp(1.0f + (deltaX - deltaY) * 0.01f, 0.85f, 1.15f);
		grassScale = clamp(grassScale * factor, vec3(0.2f), vec3(6.0f));
	}
	else if (editorSelection == EditorSelection::CreatedObject &&
		selectedCreatedObjectIndex >= 0 &&
		selectedCreatedObjectIndex < static_cast<int>(createdObjects.size()))
	{
		const float factor = std::clamp(1.0f + (deltaX - deltaY) * 0.01f, 0.85f, 1.15f);
		CreatedObject& created = createdObjects[selectedCreatedObjectIndex];
		created.scale = clamp(created.scale * factor, vec3(0.15f), vec3(10.0f));
	}

	ApplyGrassTransform();
	if (editorSelection == EditorSelection::CreatedObject)
		ApplyCreatedObjectTransform(selectedCreatedObjectIndex);
}

void FocusSelection()
{
	if (editorSelection == EditorSelection::Scene)
		return;

	sceneFocusPoint = GetSelectionWorldPosition(editorSelection);
	sceneOrbitDistance = editorSelection == EditorSelection::GrassField ? 6.5f : 4.0f;
	UpdateEditorCamera();
}

void HandleSceneViewInput(const EditorLayout& layout)
{
	const bool sceneVisible =
		(sceneViewFloating && sceneFloatingContentVisible) ||
		(activeCenterView == CenterView::Scene && !viewportCollapsed);
	if (!sceneVisible)
	{
		orbitDragging = false;
		panDragging = false;
		transformDragging = false;
		return;
	}

	const ImVec2 viewportPosition = sceneViewFloating
		? sceneFloatingContentPos
		: layout.sceneViewPos;
	const ImVec2 viewportSize = sceneViewFloating
		? sceneFloatingContentSize
		: layout.sceneViewSize;
	ImGuiIO& io = ImGui::GetIO();
	const ImVec2 mouse = io.MousePos;
	const bool altDown =
		glfwGetKey(glApp->getWindow(), GLFW_KEY_LEFT_ALT) == GLFW_PRESS ||
		glfwGetKey(glApp->getWindow(), GLFW_KEY_RIGHT_ALT) == GLFW_PRESS;
	const bool hovered =
		mouse.x >= viewportPosition.x &&
		mouse.y >= viewportPosition.y &&
		mouse.x <= viewportPosition.x + viewportSize.x &&
		mouse.y <= viewportPosition.y + viewportSize.y &&
		!(
			sceneViewFloating &&
			mouse.x >= viewportPosition.x + viewportSize.x - 82.0f &&
			mouse.y <= viewportPosition.y + 34.0f
		);

	if (hovered && io.MouseWheel != 0.0f)
	{
		sceneOrbitDistance = std::clamp(
			sceneOrbitDistance * std::exp(-io.MouseWheel * 0.12f),
			0.8f,
			60.0f
		);
		UpdateEditorCamera();
	}

	if (hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
	{
		if (altDown || io.KeyAlt)
			orbitDragging = true;
		else
		{
			PickSceneObject(viewportPosition, viewportSize, mouse);
			ImVec2 selectedPoint;
			transformDragging =
				editorSelection != EditorSelection::Scene &&
				ProjectWorldToScreen(
					GetSelectionWorldPosition(editorSelection),
					editorCamera,
					viewportPosition,
					viewportSize,
					selectedPoint
				) &&
				ScreenPointNear(mouse, selectedPoint, 14.0f);
		}
	}
	if (hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Middle))
		panDragging = true;

	if (orbitDragging && ImGui::IsMouseDown(ImGuiMouseButton_Left))
	{
		sceneOrbitYaw -= io.MouseDelta.x * 0.35f;
		sceneOrbitPitch = std::clamp(
			sceneOrbitPitch + io.MouseDelta.y * 0.3f,
			-85.0f,
			85.0f
		);
		UpdateEditorCamera();
	}

	if (panDragging && ImGui::IsMouseDown(ImGuiMouseButton_Middle))
	{
		const float panScale = std::max(0.001f, sceneOrbitDistance * 0.0015f);
		sceneFocusPoint -= editorCamera->mRight * (io.MouseDelta.x * panScale);
		sceneFocusPoint += editorCamera->mUp * (io.MouseDelta.y * panScale);
		UpdateEditorCamera();
	}

	if (transformDragging && !altDown && !io.KeyAlt && ImGui::IsMouseDown(ImGuiMouseButton_Left))
		ApplySelectionTransform(io.MouseDelta.x, io.MouseDelta.y);

	if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
	{
		orbitDragging = false;
		transformDragging = false;
	}
	if (ImGui::IsMouseReleased(ImGuiMouseButton_Middle))
		panDragging = false;
}


void DrawViewImage(
	Framebuffer* target,
	bool sceneView,
	const ImVec2& viewportPosition,
	const ImVec2& viewportSize
)
{
	if (target == nullptr || viewportSize.x <= 0.0f || viewportSize.y <= 0.0f)
		return;

	ImDrawList* drawList = ImGui::GetWindowDrawList();
	drawList->AddImage(
		(ImTextureID)(intptr_t)target->mColorAttachment->getTexture(),
		viewportPosition,
		ImVec2(viewportPosition.x + viewportSize.x, viewportPosition.y + viewportSize.y),
		ImVec2(0.0f, 1.0f),
		ImVec2(1.0f, 0.0f)
	);

	if (sceneView)
	{
		auto drawWorldIcon = [&](EditorSelection selection, ImU32 color)
		{
			ImVec2 point;
			if (!ProjectWorldToScreen(
				GetSelectionWorldPosition(selection),
				editorCamera,
				viewportPosition,
				viewportSize,
				point
			))
			{
				return;
			}
			drawList->AddCircleFilled(point, 5.0f, color);
			drawList->AddText(
				ImVec2(point.x + 8.0f, point.y - 9.0f),
				IM_COL32(220, 230, 245, 255),
				GetSelectionDisplayName(selection)
			);
		};

		drawWorldIcon(EditorSelection::Camera, IM_COL32(90, 170, 255, 255));
		drawWorldIcon(EditorSelection::DirectionalLight, IM_COL32(255, 205, 95, 255));
		drawWorldIcon(EditorSelection::AmbientLight, IM_COL32(155, 210, 255, 255));
		for (const CreatedObject& created : createdObjects)
		{
			ImVec2 point;
			if (!ProjectWorldToScreen(
				created.position,
				editorCamera,
				viewportPosition,
				viewportSize,
				point
			))
			{
				continue;
			}
			drawList->AddCircleFilled(point, 4.0f, IM_COL32(190, 214, 240, 255));
			drawList->AddText(
				ImVec2(point.x + 7.0f, point.y - 9.0f),
				IM_COL32(215, 226, 240, 255),
				created.name.c_str()
			);
		}

		if (editorSelection != EditorSelection::Scene)
		{
			ImVec2 selectedPoint;
			const vec3 selectedPosition = GetSelectionWorldPosition(editorSelection);
			if (ProjectWorldToScreen(selectedPosition, editorCamera, viewportPosition, viewportSize, selectedPoint))
			{
				drawList->AddCircle(selectedPoint, 11.0f, IM_COL32(255, 170, 60, 255), 24, 2.0f);
				auto drawTransformAxis = [&](const vec3& direction, const char* label, ImU32 color)
				{
					ImVec2 axisEnd;
					if (!ProjectWorldToScreen(
						selectedPosition + direction,
						editorCamera,
						viewportPosition,
						viewportSize,
						axisEnd
					))
					{
						return;
					}

					drawList->AddLine(selectedPoint, axisEnd, color, 3.0f);
					const float dx = axisEnd.x - selectedPoint.x;
					const float dy = axisEnd.y - selectedPoint.y;
					const float length = std::max(0.001f, std::sqrt(dx * dx + dy * dy));
					const ImVec2 axisDirection(dx / length, dy / length);
					const ImVec2 perpendicular(-axisDirection.y, axisDirection.x);

					if (currentTransformTool == TransformTool::Translate)
					{
						drawList->AddTriangleFilled(
							axisEnd,
							ImVec2(
								axisEnd.x - axisDirection.x * 9.0f + perpendicular.x * 4.5f,
								axisEnd.y - axisDirection.y * 9.0f + perpendicular.y * 4.5f
							),
							ImVec2(
								axisEnd.x - axisDirection.x * 9.0f - perpendicular.x * 4.5f,
								axisEnd.y - axisDirection.y * 9.0f - perpendicular.y * 4.5f
							),
							color
						);
					}
					else if (currentTransformTool == TransformTool::Rotate)
					{
						drawList->AddCircle(axisEnd, 5.0f, color, 16, 2.0f);
					}
					else
					{
						drawList->AddRect(
							ImVec2(axisEnd.x - 5.0f, axisEnd.y - 5.0f),
							ImVec2(axisEnd.x + 5.0f, axisEnd.y + 5.0f),
							color,
							0.0f,
							0,
							2.0f
						);
					}

					drawList->AddText(
						ImVec2(axisEnd.x + 6.0f, axisEnd.y - 12.0f),
						color,
						label
					);
				};
				drawTransformAxis(vec3(0.72f, 0.0f, 0.0f), "X", IM_COL32(235, 85, 85, 255));
				drawTransformAxis(vec3(0.0f, 0.72f, 0.0f), "Y", IM_COL32(90, 220, 110, 255));
				drawTransformAxis(vec3(0.0f, 0.0f, 0.72f), "Z", IM_COL32(90, 145, 255, 255));

				const char* selectedName = GetSelectionDisplayName(editorSelection);
				const ImVec2 labelSize = ImGui::CalcTextSize(selectedName);
				drawList->AddRectFilled(
					ImVec2(selectedPoint.x - labelSize.x * 0.5f - 5.0f, selectedPoint.y - 33.0f),
					ImVec2(selectedPoint.x + labelSize.x * 0.5f + 5.0f, selectedPoint.y - 13.0f),
					IM_COL32(18, 21, 27, 220)
				);
				drawList->AddText(
					ImVec2(selectedPoint.x - labelSize.x * 0.5f, selectedPoint.y - 31.0f),
					IM_COL32(255, 205, 120, 255),
					selectedName
				);
			}
		}

		DrawOrientationGizmo(viewportPosition, viewportSize);
	}

	const vec3& cameraPosition = sceneView ? editorCamera->mPosition : camera->mPosition;
	char cameraText[160];
	std::snprintf(
		cameraText,
		sizeof(cameraText),
		uiLanguage == UiLanguage::Chinese
			? "相机  %.2f, %.2f, %.2f   |   %.1f FPS"
			: "Camera  %.2f, %.2f, %.2f   |   %.1f FPS",
		cameraPosition.x,
		cameraPosition.y,
		cameraPosition.z,
		ImGui::GetIO().Framerate
	);
	drawList->AddRectFilled(
		ImVec2(viewportPosition.x + 10.0f, viewportPosition.y + viewportSize.y - 32.0f),
		ImVec2(viewportPosition.x + 330.0f, viewportPosition.y + viewportSize.y - 8.0f),
		IM_COL32(18, 21, 27, 195)
	);
	drawList->AddText(
		ImVec2(viewportPosition.x + 18.0f, viewportPosition.y + viewportSize.y - 27.0f),
		IM_COL32(205, 220, 240, 255),
		cameraText
	);
}

void DrawViewport(const EditorLayout& layout)
{
	ImGuiIO& io = ImGui::GetIO();
	const float tabBarHeight = 34.0f;
	const float hostHeight = viewportCollapsed ? tabBarHeight : layout.viewportSize.y;

	ImGui::SetNextWindowPos(layout.viewportPos);
	ImGui::SetNextWindowSize(ImVec2(layout.viewportSize.x, hostHeight));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin(
		"##ViewportDock",
		nullptr,
		PanelFlags | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse
	);

	auto drawTab = [&](CenterView view, const char* label, bool& floating, ImVec2& floatingPos, float& cursorX)
	{
		if (floating)
			return;

		const bool active = activeCenterView == view;
		if (active)
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.18f, 0.31f, 0.48f, 1.0f));
		else
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.09f, 0.11f, 0.14f, 1.0f));

		ImGui::SetCursorPos(ImVec2(cursorX, 4.0f));
		const bool clicked = ImGui::Button(label, ImVec2(104.0f, 26.0f));
		const bool dragging = ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left, 8.0f);
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("%s", Tr(
				"点击切换视图，向外拖动可变为浮动窗口",
				"Click to switch views. Drag out to float this view."
			));
		}
		ImGui::PopStyleColor();
		cursorX += 110.0f;

		if (clicked)
		{
			activeCenterView = view;
			viewportCollapsed = false;
		}
		if (dragging)
		{
			floating = true;
			floatingPos = ImVec2(io.MousePos.x - 36.0f, io.MousePos.y + 12.0f);
			viewportCollapsed = false;
			if (view == CenterView::Scene && !gameViewFloating)
				activeCenterView = CenterView::Game;
			else if (view == CenterView::Game && !sceneViewFloating)
				activeCenterView = CenterView::Scene;
		}
	};

	const char* sceneLabel = Tr("场景", "Scene");
	const char* gameLabel = Tr("游戏", "Game");
	float tabCursorX = 6.0f;
	drawTab(CenterView::Scene, sceneLabel, sceneViewFloating, sceneFloatingPos, tabCursorX);
	drawTab(CenterView::Game, gameLabel, gameViewFloating, gameFloatingPos, tabCursorX);

	if (sceneViewFloating && gameViewFloating)
	{
		ImGui::SetCursorPos(ImVec2(226.0f, 8.0f));
		ImGui::TextDisabled("%s", Tr("两个视图均已浮动", "Both views are floating"));
	}

	const bool activeFloating =
		(activeCenterView == CenterView::Scene && sceneViewFloating) ||
		(activeCenterView == CenterView::Game && gameViewFloating);
	if (activeFloating)
	{
		ImGui::SameLine();
		if (ImGui::SmallButton(Tr("停靠当前视图", "Dock Active View")))
		{
			if (activeCenterView == CenterView::Scene)
				sceneViewFloating = false;
			else
				gameViewFloating = false;
			viewportCollapsed = false;
		}
	}

	ImGui::SetCursorPos(ImVec2(layout.viewportSize.x - 76.0f, 4.0f));
	if (ImGui::Button(viewportCollapsed ? Tr("展开", "Open") : Tr("收起", "Fold"), ImVec2(70.0f, 26.0f)))
		viewportCollapsed = !viewportCollapsed;

	if (!viewportCollapsed)
	{
		const ImVec2 contentPosition(
			layout.viewportPos.x,
			layout.viewportPos.y + tabBarHeight
		);
		const ImVec2 contentSize(
			layout.viewportSize.x,
			layout.viewportSize.y - tabBarHeight
		);

		if (activeFloating)
		{
			ImDrawList* drawList = ImGui::GetWindowDrawList();
			drawList->AddRectFilled(
				contentPosition,
				ImVec2(contentPosition.x + contentSize.x, contentPosition.y + contentSize.y),
				IM_COL32(18, 21, 27, 255)
			);
			ImGui::SetCursorScreenPos(ImVec2(
				contentPosition.x + contentSize.x * 0.5f - 105.0f,
				contentPosition.y + contentSize.y * 0.5f - 10.0f
			));
			ImGui::TextDisabled("%s", Tr(
				"当前视图已浮动，可拖动标题栏或点击“停靠当前视图”",
				"Active view is floating. Drag its title bar or dock it."
			));
		}
		else if (activeCenterView == CenterView::Scene)
		{
			DrawViewImage(sceneViewFramebuffer, true, contentPosition, contentSize);
		}
		else
		{
			DrawViewImage(gameViewFramebuffer, false, contentPosition, contentSize);
		}
	}

	ImGui::End();
	ImGui::PopStyleVar();

	auto drawFloatingView = [&](CenterView view, bool& floating, ImVec2& initialPosition, ImVec2& contentPosition, ImVec2& contentSize)
	{
		if (!floating)
			return;

		const char* title = view == CenterView::Scene
			? Tr("场景###SceneFloating", "Scene###SceneFloating")
			: Tr("游戏###GameFloating", "Game###GameFloating");
		bool open = true;
		ImGui::SetNextWindowPos(initialPosition, ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(720.0f, 440.0f), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSizeConstraints(ImVec2(320.0f, 220.0f), io.DisplaySize);
		const bool expanded = ImGui::Begin(title, &open, ImGuiWindowFlags_NoSavedSettings);
		if (expanded)
		{
			contentPosition = ImGui::GetCursorScreenPos();
			contentSize = ImGui::GetContentRegionAvail();
			Framebuffer* target = view == CenterView::Scene ? sceneViewFramebuffer : gameViewFramebuffer;
			DrawViewImage(target, view == CenterView::Scene, contentPosition, contentSize);

			ImGui::SetCursorScreenPos(ImVec2(
				contentPosition.x + contentSize.x - 76.0f,
				contentPosition.y + 6.0f
			));
			if (ImGui::Button(Tr("停靠", "Dock"), ImVec2(68.0f, 24.0f)))
			{
				floating = false;
				activeCenterView = view;
				viewportCollapsed = false;
			}
		}
		ImGui::End();

		if (view == CenterView::Scene)
			sceneFloatingContentVisible = expanded;
		else
			gameFloatingContentVisible = expanded;

		if (!open)
		{
			floating = false;
			activeCenterView = view;
			viewportCollapsed = false;
		}
	};

	drawFloatingView(
		CenterView::Scene,
		sceneViewFloating,
		sceneFloatingPos,
		sceneFloatingContentPos,
		sceneFloatingContentSize
	);
	drawFloatingView(
		CenterView::Game,
		gameViewFloating,
		gameFloatingPos,
		gameFloatingContentPos,
		gameFloatingContentSize
	);
}




