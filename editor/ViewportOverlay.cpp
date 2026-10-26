#include "EditorShared.h"

void DrawOrientationGizmo(const ImVec2& viewportPosition, const ImVec2& viewportSize)
{
	ImDrawList* drawList = ImGui::GetWindowDrawList();
	const ImVec2 center(viewportPosition.x + viewportSize.x - 58.0f, viewportPosition.y + 70.0f);
	const float axisLength = 32.0f;

	drawList->AddCircleFilled(center, 43.0f, IM_COL32(18, 21, 27, 175), 32);
	drawList->AddCircle(center, 43.0f, IM_COL32(120, 135, 155, 100), 32, 1.0f);

	struct AxisDisplay
	{
		const char* label;
		vec3 direction;
		ImU32 color;
	};
	const AxisDisplay axes[] = {
		{ "X", vec3(1.0f, 0.0f, 0.0f), IM_COL32(232, 82, 82, 255) },
		{ "Y", vec3(0.0f, 1.0f, 0.0f), IM_COL32(93, 214, 105, 255) },
		{ "Z", vec3(0.0f, 0.0f, 1.0f), IM_COL32(91, 137, 245, 255) }
	};

	for (const AxisDisplay& axis : axes)
	{
		const ImVec2 end(
			center.x + dot(axis.direction, editorCamera->mRight) * axisLength,
			center.y - dot(axis.direction, editorCamera->mUp) * axisLength
		);
		drawList->AddLine(center, end, axis.color, 3.0f);
		drawList->AddCircleFilled(end, 4.0f, axis.color);

		const ImVec2 labelSize = ImGui::CalcTextSize(axis.label);
		const ImVec2 labelPosition(
			end.x + (end.x >= center.x ? 5.0f : -labelSize.x - 5.0f),
			end.y - labelSize.y * 0.5f
		);
		drawList->AddText(labelPosition, axis.color, axis.label);
	}

	const ImVec2 cubeTop(center.x, center.y - 8.0f);
	const ImVec2 cubeRight(center.x + 9.0f, center.y);
	const ImVec2 cubeBottom(center.x, center.y + 8.0f);
	const ImVec2 cubeLeft(center.x - 9.0f, center.y);
	drawList->AddQuadFilled(cubeTop, cubeRight, cubeBottom, cubeLeft, IM_COL32(230, 234, 240, 255));
	drawList->AddTriangleFilled(cubeTop, cubeLeft, center, IM_COL32(185, 195, 210, 255));
	drawList->AddTriangleFilled(cubeTop, center, cubeRight, IM_COL32(210, 218, 230, 255));

	const char* perspectiveLabel = "Persp";
	const ImVec2 perspectiveSize = ImGui::CalcTextSize(perspectiveLabel);
	drawList->AddText(
		ImVec2(center.x - perspectiveSize.x * 0.5f, center.y + 48.0f),
		IM_COL32(190, 202, 218, 210),
		perspectiveLabel
	);
}

