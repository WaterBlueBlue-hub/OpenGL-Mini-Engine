#include "EditorShared.h"

#include <algorithm>
#include <cstdio>

namespace
{
	struct InspectorMeta
	{
		const char* title{ "Scene" };
		const char* type{ "Scene" };
		const char* icon{ "S" };
		ImU32 accent{ IM_COL32(100, 130, 170, 255) };
	};

	InspectorMeta GetInspectorMeta()
	{
		switch (editorSelection)
		{
		case EditorSelection::GrassField:
			return { Tr("草地", "Grass Field"), Tr("实例化网格", "Instanced Mesh"), "M", IM_COL32(59, 151, 105, 255) };
		case EditorSelection::DirectionalLight:
			return { "Directional Light", "Light", "L", IM_COL32(214, 161, 58, 255) };
		case EditorSelection::AmbientLight:
			return { "Ambient Light", "Light", "L", IM_COL32(91, 142, 201, 255) };
		case EditorSelection::Camera:
			return { "Main Camera", "Camera", "C", IM_COL32(74, 139, 216, 255) };
		case EditorSelection::CreatedObject:
			if (selectedCreatedObjectIndex >= 0 &&
				selectedCreatedObjectIndex < static_cast<int>(createdObjects.size()))
			{
				const CreatedObject& created = createdObjects[selectedCreatedObjectIndex];
				const char* type =
					created.kind == CreatedObjectKind::Empty ? Tr("空对象", "Empty") :
					created.kind == CreatedObjectKind::Quad ? Tr("2D 对象", "2D Object") :
					Tr("3D 对象", "3D Object");
				return {
					created.name.c_str(),
					type,
					created.kind == CreatedObjectKind::Empty ? "E" : "G",
					IM_COL32(120, 132, 148, 255)
				};
			}
			return { "GameObject", "Object", "G", IM_COL32(120, 132, 148, 255) };
		case EditorSelection::Scene:
			break;
		}
		return { Tr("场景", "Scene"), "Scene", "S", IM_COL32(100, 130, 170, 255) };
	}

	bool BeginSection(const char* label)
	{
		ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.13f, 0.16f, 0.21f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.18f, 0.24f, 0.32f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.20f, 0.28f, 0.38f, 1.0f));
		const bool open = ImGui::CollapsingHeader(
			label,
			ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth
		);
		ImGui::PopStyleColor(3);
		return open;
	}

	bool BeginTable(const char* id)
	{
		if (!ImGui::BeginTable(id, 2, ImGuiTableFlags_SizingStretchProp))
			return false;
		ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed, 84.0f);
		ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);
		return true;
	}

	void DrawLabel(const char* label)
	{
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::AlignTextToFramePadding();
		ImGui::TextDisabled("%s", label);
		ImGui::TableSetColumnIndex(1);
	}

	void DrawTextRow(const char* label, const char* value)
	{
		DrawLabel(label);
		ImGui::TextUnformatted(value);
	}

	bool DrawFloatRow(const char* label, float& value, float speed, float minimum, float maximum, const char* format = "%.2f")
	{
		DrawLabel(label);
		ImGui::PushID(label);
		ImGui::SetNextItemWidth(-1.0f);
		const bool changed = ImGui::DragFloat("##Value", &value, speed, minimum, maximum, format);
		ImGui::PopID();
		return changed;
	}

	bool DrawSliderRow(const char* label, float& value, float minimum, float maximum)
	{
		DrawLabel(label);
		ImGui::PushID(label);
		ImGui::SetNextItemWidth(-1.0f);
		const bool changed = ImGui::SliderFloat("##Value", &value, minimum, maximum, "%.3f");
		ImGui::PopID();
		return changed;
	}

	bool DrawColorRow(const char* label, vec3& value)
	{
		DrawLabel(label);
		ImGui::PushID(label);
		ImGui::SetNextItemWidth(-1.0f);
		const bool changed = ImGui::ColorEdit3("##Value", (float*)&value, ImGuiColorEditFlags_NoInputs);
		ImGui::PopID();
		return changed;
	}

	bool DrawVectorRow(const char* label, vec3& value, float speed, float minimum, float maximum)
	{
		DrawLabel(label);
		ImGui::PushID(label);
		const float available = ImGui::GetContentRegionAvail().x;
		const float fieldWidth = std::max(42.0f, (available - 28.0f) / 3.0f);
		const ImVec4 axisColors[] = {
			ImVec4(0.91f, 0.34f, 0.34f, 1.0f),
			ImVec4(0.36f, 0.84f, 0.44f, 1.0f),
			ImVec4(0.38f, 0.56f, 0.96f, 1.0f)
		};
		const char* axisLabels[] = { "X", "Y", "Z" };
		bool changed = false;
		for (int axis = 0; axis < 3; ++axis)
		{
			ImGui::PushID(axis);
			ImGui::TextColored(axisColors[axis], "%s", axisLabels[axis]);
			ImGui::SameLine(0.0f, 2.0f);
			ImGui::SetNextItemWidth(fieldWidth - 12.0f);
			changed |= ImGui::DragFloat("##Value", &value[axis], speed, minimum, maximum, "%.2f");
			ImGui::PopID();
			if (axis < 2)
				ImGui::SameLine(0.0f, 5.0f);
		}
		ImGui::PopID();
		return changed;
	}

	void DrawHeader(const InspectorMeta& meta)
	{
		ImGui::Text("%s", Tr("属性检视器", "INSPECTOR"));
		ImGui::Separator();
		const ImVec2 iconMin = ImGui::GetCursorScreenPos();
		ImGui::Dummy(ImVec2(36.0f, 36.0f));
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		drawList->AddRectFilled(iconMin, ImVec2(iconMin.x + 32.0f, iconMin.y + 32.0f), meta.accent, 5.0f);
		const ImVec2 iconTextSize = ImGui::CalcTextSize(meta.icon);
		drawList->AddText(
			ImVec2(iconMin.x + 16.0f - iconTextSize.x * 0.5f, iconMin.y + 16.0f - iconTextSize.y * 0.5f),
			IM_COL32(245, 248, 252, 255),
			meta.icon
		);
		ImGui::SameLine();
		ImGui::BeginGroup();
		ImGui::TextColored(ImVec4(0.78f, 0.87f, 1.0f, 1.0f), "%s", meta.title);
		ImGui::TextDisabled("%s", meta.type);
		ImGui::EndGroup();
		ImGui::Separator();
	}

	void DrawMaterialPreview()
	{
		const ImVec2 swatchMin = ImGui::GetCursorScreenPos();
		ImGui::Dummy(ImVec2(52.0f, 52.0f));
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		const ImVec2 center(swatchMin.x + 25.0f, swatchMin.y + 25.0f);
		drawList->AddCircleFilled(center, 23.0f, IM_COL32(46, 49, 55, 255), 24);
		drawList->AddCircleFilled(ImVec2(center.x - 5.0f, center.y - 6.0f), 17.0f, IM_COL32(150, 150, 150, 255), 24);
		drawList->AddCircleFilled(ImVec2(center.x - 8.0f, center.y - 9.0f), 10.0f, IM_COL32(202, 202, 202, 255), 24);
		drawList->AddCircle(center, 23.0f, IM_COL32(210, 215, 225, 90), 24, 1.0f);
		ImGui::SameLine();
		ImGui::BeginGroup();
		ImGui::TextUnformatted("Lit Gray");
		ImGui::TextDisabled("%s", Tr("默认材质", "Default Material"));
		ImGui::EndGroup();
	}
}

void DrawInspector(const EditorLayout& layout)
{
	ImGui::SetNextWindowPos(layout.inspectorPos);
	ImGui::SetNextWindowSize(layout.inspectorSize);
	ImGui::Begin("##Inspector", nullptr,
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoBringToFrontOnFocus
	);

	DrawHeader(GetInspectorMeta());
	const char* tool =
		currentTransformTool == TransformTool::Translate ? Tr("移动", "Move") :
		currentTransformTool == TransformTool::Rotate ? Tr("旋转", "Rotate") :
		Tr("缩放", "Scale");
	ImGui::TextDisabled("%s: %s", Tr("当前工具", "Active tool"), tool);
	ImGui::Spacing();

	switch (editorSelection)
	{
	case EditorSelection::Scene:
		if (BeginSection(Tr("环境", "Environment")) && BeginTable("##SceneEnvironment"))
		{
			DrawTextRow(Tr("天空盒", "Skybox"), Tr("程序化天空", "Procedural Sky"));
			DrawTextRow(Tr("渲染器", "Renderer"), "OpenGL 3.3 Core");
			DrawTextRow(Tr("绘制调用", "Draw calls"), grassSceneReady ? "2" : "0");
			ImGui::EndTable();
		}
		if (BeginSection(Tr("统计", "Statistics")) && BeginTable("##SceneStatistics"))
		{
			char fps[64];
			std::snprintf(fps, sizeof(fps), "%.1f FPS", ImGui::GetIO().Framerate);
			DrawTextRow(Tr("帧率", "Frame rate"), fps);
			DrawTextRow(Tr("视图", "Views"), Tr("场景 / 游戏", "Scene / Game"));
			ImGui::EndTable();
		}
		break;

	case EditorSelection::GrassField:
		if (BeginSection(Tr("网格渲染器", "Mesh Renderer")) && BeginTable("##GrassRenderer"))
		{
			DrawTextRow(Tr("类型", "Type"), Tr("实例化网格", "Instanced Mesh"));
			DrawTextRow(Tr("实例数量", "Instances"), "625");
			ImGui::EndTable();
		}
		if (BeginSection(Tr("变换", "Transform")) && BeginTable("##GrassTransform"))
		{
			if (DrawVectorRow(Tr("位置", "Position"), grassPosition, 0.02f, -100.0f, 100.0f))
				ApplyGrassTransform();
			if (DrawVectorRow(Tr("旋转", "Rotation"), grassRotation, 0.2f, -3600.0f, 3600.0f))
				ApplyGrassTransform();
			if (DrawVectorRow(Tr("缩放", "Scale"), grassScale, 0.01f, 0.1f, 6.0f))
				ApplyGrassTransform();
			ImGui::EndTable();
		}
		if (BeginSection(Tr("风场", "Wind")) && BeginTable("##GrassWind"))
		{
			DrawSliderRow(Tr("强度", "Strength"), grassMaterial->mWindScale, 0.0f, 0.2f);
			DrawSliderRow(Tr("相位", "Phase"), grassMaterial->mPhaseScale, 0.1f, 3.0f);
			DrawColorRow(Tr("方向", "Direction"), grassMaterial->mWindDirection);
			ImGui::EndTable();
		}
		if (BeginSection(Tr("云层", "Cloud Layer")) && BeginTable("##GrassCloud"))
		{
			DrawColorRow(Tr("颜色", "Color"), grassMaterial->mCloudBlackColor);
			DrawSliderRow(Tr("缩放", "Scale"), grassMaterial->mCloudUVScale, 1.0f, 10.0f);
			DrawSliderRow(Tr("速度", "Speed"), grassMaterial->mCloudSpeed, 0.0f, 1.0f);
			DrawSliderRow(Tr("混合", "Blend"), grassMaterial->mCloudLerp, 0.0f, 1.0f);
			ImGui::EndTable();
		}
		break;

	case EditorSelection::DirectionalLight:
		if (BeginSection(Tr("光照", "Light")) && BeginTable("##DirectionalLight"))
		{
			DrawColorRow(Tr("颜色", "Color"), dirLight->mColor);
			DrawSliderRow(Tr("强度", "Intensity"), dirLight->mIntensity, 0.0f, 1.0f);
			DrawVectorRow(Tr("方向", "Direction"), dirLight->mDirection, 0.02f, -10.0f, 10.0f);
			ImGui::EndTable();
		}
		if (BeginSection(Tr("时间预设", "Time Presets")))
		{
			const ImVec2 size(84.0f, 28.0f);
			if (ImGui::Button(Tr("清晨", "Morning"), size)) SetStaticTimeOfDay(TimeOfDay::EarlyMorning);
			ImGui::SameLine();
			if (ImGui::Button(Tr("正午", "Noon"), size)) SetStaticTimeOfDay(TimeOfDay::Noon);
			if (ImGui::Button(Tr("黄昏", "Dusk"), size)) SetStaticTimeOfDay(TimeOfDay::Dusk);
			ImGui::SameLine();
			if (ImGui::Button(Tr("夜晚", "Night"), size)) SetStaticTimeOfDay(TimeOfDay::Night);
		}
		break;

	case EditorSelection::AmbientLight:
		if (BeginSection(Tr("光照", "Light")) && BeginTable("##AmbientLight"))
		{
			DrawColorRow(Tr("颜色", "Color"), ambLight->mColor);
			ImGui::EndTable();
		}
		break;

	case EditorSelection::Camera:
		if (BeginSection(Tr("变换", "Transform")) && BeginTable("##CameraTransform"))
		{
			DrawVectorRow(Tr("位置", "Position"), camera->mPosition, 0.05f, -100.0f, 100.0f);
			if (DrawVectorRow(Tr("旋转", "Rotation"), mainCameraRotation, 0.2f, -3600.0f, 3600.0f))
				ApplyMainCameraRotation();
			DrawVectorRow(Tr("缩放", "Scale"), mainCameraScale, 0.01f, 0.01f, 10.0f);
			ImGui::EndTable();
		}
		if (BeginSection(Tr("相机", "Camera")) && BeginTable("##CameraSettings"))
		{
			DrawTextRow(
				Tr("投影", "Projection"),
				orthographicCameraMode ? Tr("正交", "Orthographic") : Tr("透视", "Perspective")
			);
			char range[64];
			std::snprintf(range, sizeof(range), "%.2f - %.1f", camera->mNear, camera->mFar);
			DrawTextRow(Tr("裁剪范围", "Clipping"), range);
			ImGui::EndTable();
			if (ImGui::Button(Tr("重置视角", "Reset View"), ImVec2(130.0f, 30.0f)))
				ResetSceneCamera();
		}
		break;

	case EditorSelection::CreatedObject:
		if (selectedCreatedObjectIndex >= 0 &&
			selectedCreatedObjectIndex < static_cast<int>(createdObjects.size()))
		{
			CreatedObject& created = createdObjects[selectedCreatedObjectIndex];
			if (BeginSection(Tr("变换", "Transform")) && BeginTable("##CreatedTransform"))
			{
				if (DrawVectorRow(Tr("位置", "Position"), created.position, 0.02f, -100.0f, 100.0f))
					ApplyCreatedObjectTransform(selectedCreatedObjectIndex);
				if (DrawVectorRow(Tr("旋转", "Rotation"), created.rotation, 0.2f, -3600.0f, 3600.0f))
					ApplyCreatedObjectTransform(selectedCreatedObjectIndex);
				if (DrawVectorRow(Tr("缩放", "Scale"), created.scale, 0.01f, 0.1f, 10.0f))
					ApplyCreatedObjectTransform(selectedCreatedObjectIndex);
				ImGui::EndTable();
			}
			if (created.kind != CreatedObjectKind::Empty && BeginSection(Tr("材质", "Material")))
				DrawMaterialPreview();
		}
		break;
	}
	ImGui::End();
}

