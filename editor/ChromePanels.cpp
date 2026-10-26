#include "EditorShared.h"

#include <algorithm>
#include <cstdio>
#include <string>

namespace
{
	const ImGuiWindowFlags PanelFlags =
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoBringToFrontOnFocus;

	void DrawToolbar(const EditorLayout& layout)
	{
		ImGui::SetNextWindowPos(layout.toolbarPos);
		ImGui::SetNextWindowSize(layout.toolbarSize);
		ImGui::Begin("##Toolbar", nullptr, PanelFlags | ImGuiWindowFlags_NoScrollbar);

		ImGui::SetCursorPosY(11.0f);
		ImGui::TextColored(ImVec4(0.55f, 0.78f, 1.0f, 1.0f), "OPENGL MINI ENGINE");
		ImGui::SameLine(205.0f);

		const bool playActive = simulationPlaying && !simulationPaused;
		const bool pauseActive = simulationPlaying && simulationPaused;
		auto transportButton = [](const char* id, int icon, bool active, const char* tooltip)
		{
			ImGui::InvisibleButton(id, ImVec2(40.0f, 30.0f));
			const bool pressed = ImGui::IsItemClicked();
			const bool hovered = ImGui::IsItemHovered();
			const ImVec2 min = ImGui::GetItemRectMin();
			const ImVec2 max = ImGui::GetItemRectMax();
			const ImVec2 center((min.x + max.x) * 0.5f, (min.y + max.y) * 0.5f);
			ImDrawList* drawList = ImGui::GetWindowDrawList();

			ImU32 background = IM_COL32(41, 53, 73, 255);
			if (active)
				background = icon == 0 ? IM_COL32(36, 126, 72, 255) : IM_COL32(158, 111, 31, 255);
			else if (hovered)
				background = IM_COL32(57, 83, 122, 255);
			drawList->AddRectFilled(min, max, background, 3.0f);

			const ImU32 iconColor = IM_COL32(235, 240, 248, 255);
			if (icon == 0)
			{
				drawList->AddTriangleFilled(
					ImVec2(center.x - 5.0f, center.y - 8.0f),
					ImVec2(center.x - 5.0f, center.y + 8.0f),
					ImVec2(center.x + 8.0f, center.y),
					iconColor
				);
			}
			else if (icon == 1)
			{
				drawList->AddRectFilled(ImVec2(center.x - 7.0f, center.y - 8.0f), ImVec2(center.x - 2.0f, center.y + 8.0f), iconColor, 1.0f);
				drawList->AddRectFilled(ImVec2(center.x + 2.0f, center.y - 8.0f), ImVec2(center.x + 7.0f, center.y + 8.0f), iconColor, 1.0f);
			}
			else
			{
				drawList->AddTriangleFilled(
					ImVec2(center.x - 8.0f, center.y - 8.0f),
					ImVec2(center.x - 8.0f, center.y + 8.0f),
					ImVec2(center.x + 4.0f, center.y),
					iconColor
				);
				drawList->AddRectFilled(ImVec2(center.x + 7.0f, center.y - 8.0f), ImVec2(center.x + 10.0f, center.y + 8.0f), iconColor);
			}
			if (hovered)
				ImGui::SetTooltip("%s", tooltip);
			return pressed;
		};

		if (transportButton("##Play", 0, playActive, Tr("播放", "Play")))
			StartSimulation();
		ImGui::SameLine();
		if (transportButton("##Pause", 1, pauseActive, Tr("暂停", "Pause")))
			PauseSimulation();
		ImGui::SameLine();
		if (transportButton("##Step", 2, false, Tr("单帧步进", "Step one frame")))
			StepSimulation();

		ImGui::SameLine();
		if (ImGui::Button(
			orthographicCameraMode ? Tr("透视视图", "Perspective") : Tr("正交视图", "Orthographic"),
			ImVec2(108.0f, 30.0f)
		))
		{
			ToggleProjection();
		}
		ImGui::SameLine();
		if (ImGui::Button(Tr("重置相机", "Reset Camera"), ImVec2(108.0f, 30.0f)))
			ResetSceneCamera();

		ImGui::SameLine();
		auto toolButton = [](const char* label, TransformTool tool)
		{
			if (currentTransformTool == tool)
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.27f, 0.48f, 0.74f, 1.0f));
			if (ImGui::Button(label, ImVec2(68.0f, 30.0f)))
				currentTransformTool = tool;
			if (currentTransformTool == tool)
				ImGui::PopStyleColor();
		};
		toolButton(Tr("W 移动", "W Move"), TransformTool::Translate);
		ImGui::SameLine();
		toolButton(Tr("E 旋转", "E Rotate"), TransformTool::Rotate);
		ImGui::SameLine();
		toolButton(Tr("R 缩放", "R Scale"), TransformTool::Scale);

		ImGui::SameLine();
		if (ImGui::Button(
			uiLanguage == UiLanguage::Chinese ? "English" : "中文",
			ImVec2(80.0f, 30.0f)
		))
		{
			uiLanguage = uiLanguage == UiLanguage::Chinese ? UiLanguage::English : UiLanguage::Chinese;
		}

		ImGui::SameLine();
		ImGui::SetCursorPosX(layout.toolbarSize.x - 205.0f);
		ImGui::TextDisabled("FPS %.1f  |  1600x900", ImGui::GetIO().Framerate);
		ImGui::End();
	}

	void DrawHierarchy(const EditorLayout& layout)
	{
		ImGui::SetNextWindowPos(layout.hierarchyPos);
		ImGui::SetNextWindowSize(layout.hierarchySize);
		ImGui::Begin("##SceneHierarchy", nullptr, PanelFlags);

		ImGui::Text("%s", Tr("场景层级", "SCENE HIERARCHY"));
		ImGui::SameLine();
		const std::size_t objectCount =
			4 + createdObjects.size() + (grassSceneReady ? 1 : 0);
		ImGui::TextDisabled(
			uiLanguage == UiLanguage::Chinese ? "%zu 个对象" : "%zu objects",
			objectCount
		);
		ImGui::Separator();

		if (ImGui::Button(Tr("+ 创建", "+ Create"), ImVec2(92.0f, 28.0f)))
			ImGui::OpenPopup("##CreateObjectMenu");

		auto drawCreateOptions = []()
		{
			if (ImGui::MenuItem(Tr("创建空对象", "Create Empty")))
				CreateSceneObject(CreatedObjectKind::Empty, "GameObject");
			if (ImGui::BeginMenu(Tr("2D 对象", "2D Object")))
			{
				if (ImGui::MenuItem(Tr("四边形", "Quad")))
					CreateSceneObject(CreatedObjectKind::Quad, "Quad");
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu(Tr("3D 对象", "3D Object")))
			{
				if (ImGui::MenuItem(Tr("立方体", "Cube")))
					CreateSceneObject(CreatedObjectKind::Cube, "Cube");
				if (ImGui::MenuItem(Tr("球体", "Sphere")))
					CreateSceneObject(CreatedObjectKind::Sphere, "Sphere");
				if (ImGui::MenuItem(Tr("平面", "Plane")))
					CreateSceneObject(CreatedObjectKind::Plane, "Plane");
				ImGui::EndMenu();
			}
		};

		if (ImGui::BeginPopup("##CreateObjectMenu"))
		{
			drawCreateOptions();
			ImGui::EndPopup();
		}
		if (ImGui::BeginPopupContextWindow("##HierarchyCreateContext", ImGuiPopupFlags_MouseButtonRight))
		{
			drawCreateOptions();
			ImGui::EndPopup();
		}

		const bool sceneOpen = ImGui::TreeNodeEx(
			Tr("场景", "Scene"),
			ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth
		);
		if (ImGui::IsItemClicked())
			editorSelection = EditorSelection::Scene;

		if (sceneOpen)
		{
			auto item = [](const char* label, EditorSelection selection)
			{
				ImGuiTreeNodeFlags flags =
					ImGuiTreeNodeFlags_Leaf |
					ImGuiTreeNodeFlags_NoTreePushOnOpen |
					ImGuiTreeNodeFlags_SpanAvailWidth;
				if (editorSelection == selection)
					flags |= ImGuiTreeNodeFlags_Selected;
				ImGui::TreeNodeEx(label, flags);
				if (ImGui::IsItemClicked())
					editorSelection = selection;
			};

			if (grassSceneReady)
				item(Tr("[网格] 草地", "[Mesh] Grass Field"), EditorSelection::GrassField);
			item("[Light] Directional Light", EditorSelection::DirectionalLight);
			item("[Light] Ambient Light", EditorSelection::AmbientLight);
			item("[Camera] Main Camera", EditorSelection::Camera);

			for (size_t i = 0; i < createdObjects.size(); ++i)
			{
				const CreatedObject& created = createdObjects[i];
				const char* prefix =
					created.kind == CreatedObjectKind::Empty ? "[Empty] " :
					created.kind == CreatedObjectKind::Quad ? "[2D] " : "[3D] ";
				const std::string label = std::string(prefix) + created.name;
				ImGuiTreeNodeFlags flags =
					ImGuiTreeNodeFlags_Leaf |
					ImGuiTreeNodeFlags_NoTreePushOnOpen |
					ImGuiTreeNodeFlags_SpanAvailWidth;
				if (editorSelection == EditorSelection::CreatedObject &&
					selectedCreatedObjectIndex == static_cast<int>(i))
				{
					flags |= ImGuiTreeNodeFlags_Selected;
				}
				ImGui::TreeNodeEx(label.c_str(), flags);
				if (ImGui::IsItemClicked())
				{
					editorSelection = EditorSelection::CreatedObject;
					selectedCreatedObjectIndex = static_cast<int>(i);
				}
			}
			ImGui::TreePop();
		}
		ImGui::End();
	}
}

EditorLayout BuildEditorLayout()
{
	const ImVec2 display = ImGui::GetIO().DisplaySize;
	const float toolbarHeight = 54.0f;
	const float leftWidth = 250.0f;
	const float rightWidth = 330.0f;
	const float bottomHeight = 220.0f;
	const float workHeight = std::max(280.0f, display.y - toolbarHeight - bottomHeight);
	const float centerWidth = std::max(360.0f, display.x - leftWidth - rightWidth);
	const float viewportTabHeight = 34.0f;

	EditorLayout layout;
	layout.toolbarPos = ImVec2(0.0f, 0.0f);
	layout.toolbarSize = ImVec2(display.x, toolbarHeight);
	layout.hierarchyPos = ImVec2(0.0f, toolbarHeight);
	layout.hierarchySize = ImVec2(leftWidth, workHeight);
	layout.viewportPos = ImVec2(leftWidth, toolbarHeight);
	layout.viewportSize = ImVec2(centerWidth, workHeight);
	layout.sceneViewPos = ImVec2(layout.viewportPos.x, layout.viewportPos.y + viewportTabHeight);
	layout.sceneViewSize = ImVec2(layout.viewportSize.x, layout.viewportSize.y - viewportTabHeight);
	layout.gameViewPos = layout.sceneViewPos;
	layout.gameViewSize = layout.sceneViewSize;
	layout.inspectorPos = ImVec2(leftWidth + centerWidth, toolbarHeight);
	layout.inspectorSize = ImVec2(display.x - leftWidth - centerWidth, workHeight);
	layout.contentPos = ImVec2(0.0f, display.y - bottomHeight);
	layout.contentSize = ImVec2(display.x, bottomHeight);
	return layout;
}

void DrawEditorUI(const EditorLayout& layout)
{
	DrawToolbar(layout);
	DrawHierarchy(layout);
	DrawInspector(layout);
	DrawViewport(layout);
	DrawContentBrowser(layout);
}

