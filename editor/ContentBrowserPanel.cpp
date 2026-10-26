#include "EditorShared.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <cstdio>
#include <filesystem>
#include <string>
#include <vector>

namespace
{
	enum class AssetCategory
	{
		Scenes,
		Materials,
		Models,
		Textures
	};

	enum class AssetFolder
	{
		Root,
		Scenes,
		Materials,
		Models,
		Textures
	};

	std::array<std::vector<std::string>, 4> assetNames;
	AssetCategory selectedCategory = AssetCategory::Models;
	AssetFolder currentAssetFolder = AssetFolder::Root;
	std::string selectedAsset;
	char assetSearch[128]{};
	bool assetListView = true;
	float assetZoom = 1.0f;

	const char* GetCategoryName(AssetCategory category)
	{
		switch (category)
		{
		case AssetCategory::Scenes:
			return Tr("场景", "Scenes");
		case AssetCategory::Materials:
			return Tr("材质", "Materials");
		case AssetCategory::Models:
			return Tr("模型", "Models");
		case AssetCategory::Textures:
			return Tr("纹理", "Textures");
		}
		return Tr("资源", "Assets");
	}

	void RefreshAssetLists()
	{
		namespace fs = std::filesystem;
		for (auto& list : assetNames)
			list.clear();

		auto collect = [](const fs::path& directory, std::vector<std::string>& output)
		{
			std::error_code error;
			if (!fs::exists(directory, error) || !fs::is_directory(directory, error))
				return;
			for (const auto& entry : fs::directory_iterator(directory, error))
			{
				if (error)
					break;
				if (entry.is_regular_file())
				{
					const std::string filename = entry.path().filename().string();
					if (filename.empty() || filename.front() == '.')
						continue;
					output.push_back(filename);
				}
			}
			std::sort(output.begin(), output.end());
		};

		collect("assets/Assets/Scenes", assetNames[0]);
		collect("assets/Assets/Materials", assetNames[1]);
		collect("assets/Assets/Models", assetNames[2]);
		collect("assets/Assets/Textures", assetNames[3]);
	}

	bool ContainsSearch(const std::string& name, const std::string& loweredSearch)
	{
		if (loweredSearch.empty())
			return true;
		std::string lowered = name;
		std::transform(
			lowered.begin(),
			lowered.end(),
			lowered.begin(),
			[](unsigned char character) { return static_cast<char>(std::tolower(character)); }
		);
		return lowered.find(loweredSearch) != std::string::npos;
	}

	void DrawAssetIcon(const ImVec2& position, float size, int kind)
	{
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		if (kind == 0)
		{
			const ImU32 folderColor = IM_COL32(218, 178, 75, 255);
			drawList->AddRectFilled(
				ImVec2(position.x + 1.0f, position.y + 5.0f),
				ImVec2(position.x + size - 1.0f, position.y + size - 2.0f),
				folderColor,
				2.0f
			);
			drawList->AddRectFilled(
				ImVec2(position.x + 4.0f, position.y + 1.0f),
				ImVec2(position.x + size * 0.56f, position.y + 8.0f),
				folderColor,
				2.0f
			);
			return;
		}

		const ImU32 fileColor =
			kind == 1 ? IM_COL32(72, 130, 204, 255) :
			kind == 2 ? IM_COL32(184, 116, 61, 255) :
			kind == 3 ? IM_COL32(70, 151, 103, 255) :
			IM_COL32(139, 101, 182, 255);
		const char* label = kind == 1 ? "S" : kind == 2 ? "M" : kind == 3 ? "T" : "O";
		drawList->AddRectFilled(position, ImVec2(position.x + size, position.y + size), fileColor, 3.0f);
		const ImVec2 labelSize = ImGui::CalcTextSize(label);
		drawList->AddText(
			ImVec2(
				position.x + size * 0.5f - labelSize.x * 0.5f,
				position.y + size * 0.5f - labelSize.y * 0.5f
			),
			IM_COL32(245, 248, 252, 255),
			label
		);
	}

	int IconKind(AssetCategory category)
	{
		switch (category)
		{
		case AssetCategory::Scenes:
			return 1;
		case AssetCategory::Materials:
			return 2;
		case AssetCategory::Textures:
			return 3;
		case AssetCategory::Models:
			return 4;
		}
		return 4;
	}
}

void InitializeContentBrowser()
{
	RefreshAssetLists();
}

void DrawContentBrowser(const EditorLayout& layout)
{
	ImGui::SetNextWindowPos(layout.contentPos);
	ImGui::SetNextWindowSize(layout.contentSize);
	ImGui::Begin("##ContentBrowser", nullptr,
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoBringToFrontOnFocus
	);

	ImGui::BeginChild("##ProjectSidebar", ImVec2(280.0f, 0.0f), true);
	auto sidebarFolder = [](const char* label, AssetFolder folder, AssetCategory category, size_t count)
	{
		char text[128];
		std::snprintf(text, sizeof(text), "%s  (%zu)", label, count);
		if (ImGui::Selectable(text, currentAssetFolder == folder))
		{
			currentAssetFolder = folder;
			selectedCategory = category;
			selectedAsset.clear();
			assetSearch[0] = '\0';
		}
	};

	ImGui::Text("%s", Tr("项目", "Project"));
	sidebarFolder("Assets", AssetFolder::Root, AssetCategory::Models,
		assetNames[0].size() + assetNames[1].size() + assetNames[2].size() + assetNames[3].size());
	ImGui::Indent();
	sidebarFolder("Scenes", AssetFolder::Scenes, AssetCategory::Scenes, assetNames[0].size());
	sidebarFolder("Materials", AssetFolder::Materials, AssetCategory::Materials, assetNames[1].size());
	sidebarFolder("Models", AssetFolder::Models, AssetCategory::Models, assetNames[2].size());
	sidebarFolder("Textures", AssetFolder::Textures, AssetCategory::Textures, assetNames[3].size());
	ImGui::Unindent();
	ImGui::Spacing();
	ImGui::TextDisabled("Packages");
	ImGui::EndChild();

	ImGui::SameLine();
	ImGui::BeginChild("##ProjectMain", ImVec2(0.0f, 0.0f), false);
	std::string searchTerm(assetSearch);
	std::transform(
		searchTerm.begin(),
		searchTerm.end(),
		searchTerm.begin(),
		[](unsigned char character) { return static_cast<char>(std::tolower(character)); }
	);

	ImGui::Text("ASSETS");
	ImGui::SameLine();
	ImGui::SetCursorPosX(std::max(350.0f, ImGui::GetWindowWidth() - 615.0f));
	ImGui::SetNextItemWidth(300.0f);
	ImGui::InputTextWithHint(
		"##AssetSearch",
		Tr("搜索资源...", "Search assets..."),
		assetSearch,
		IM_ARRAYSIZE(assetSearch)
	);
	ImGui::SameLine();
	if (ImGui::SmallButton(Tr("刷新", "Refresh")))
		RefreshAssetLists();

	ImGui::SameLine();
	const bool gridWasActive = !assetListView;
	if (gridWasActive)
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.18f, 0.31f, 0.48f, 1.0f));
	const bool gridClicked = ImGui::SmallButton(Tr("网格", "Grid"));
	if (gridWasActive)
		ImGui::PopStyleColor();
	if (gridClicked)
		assetListView = false;

	ImGui::SameLine();
	const bool listWasActive = assetListView;
	if (listWasActive)
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.18f, 0.31f, 0.48f, 1.0f));
	const bool listClicked = ImGui::SmallButton(Tr("列表", "List"));
	if (listWasActive)
		ImGui::PopStyleColor();
	if (listClicked)
		assetListView = true;
	ImGui::Separator();

	if (ImGui::SmallButton("Assets##BreadcrumbRoot"))
	{
		currentAssetFolder = AssetFolder::Root;
		selectedAsset.clear();
	}
	if (currentAssetFolder != AssetFolder::Root)
	{
		ImGui::SameLine();
		ImGui::TextDisabled(">");
		ImGui::SameLine();
		const char* folderName =
			currentAssetFolder == AssetFolder::Scenes ? "Scenes" :
			currentAssetFolder == AssetFolder::Materials ? "Materials" :
			currentAssetFolder == AssetFolder::Models ? "Models" : "Textures";
		ImGui::TextUnformatted(folderName);
	}
	ImGui::Separator();

	ImGui::BeginChild("##AssetContent", ImVec2(0.0f, -32.0f), false);
	const bool searching = !searchTerm.empty();
	const float rowHeight = std::max(24.0f, 25.0f * assetZoom);

	auto drawFolderEntry = [&](const char* label, AssetFolder folder)
	{
		if (!ContainsSearch(label, searchTerm))
			return;
		ImGui::PushID(label);
		const ImVec2 rowMin = ImGui::GetCursorScreenPos();
		const float width = ImGui::GetContentRegionAvail().x;
		ImGui::InvisibleButton("##Folder", ImVec2(width, rowHeight));
		if (ImGui::IsItemHovered())
			ImGui::GetWindowDrawList()->AddRectFilled(
				rowMin,
				ImVec2(rowMin.x + width, rowMin.y + rowHeight),
				IM_COL32(45, 52, 64, 255)
			);
		DrawAssetIcon(ImVec2(rowMin.x + 7.0f, rowMin.y + 4.0f), rowHeight - 8.0f, 0);
		ImGui::GetWindowDrawList()->AddText(
			ImVec2(rowMin.x + 34.0f, rowMin.y + (rowHeight - ImGui::GetFontSize()) * 0.5f),
			IM_COL32(220, 224, 230, 255),
			label
		);
		if (ImGui::IsItemClicked())
		{
			currentAssetFolder = folder;
			selectedAsset.clear();
			if (folder == AssetFolder::Scenes)
				selectedCategory = AssetCategory::Scenes;
			else if (folder == AssetFolder::Materials)
				selectedCategory = AssetCategory::Materials;
			else if (folder == AssetFolder::Models)
				selectedCategory = AssetCategory::Models;
			else if (folder == AssetFolder::Textures)
				selectedCategory = AssetCategory::Textures;
		}
		ImGui::PopID();
	};

	auto drawFileEntry = [&](const std::string& file, AssetCategory category)
	{
		if (!ContainsSearch(file, searchTerm))
			return;
		const bool selected = selectedAsset == file;
		ImGui::PushID(file.c_str());
		const ImVec2 rowMin = ImGui::GetCursorScreenPos();
		const float width = ImGui::GetContentRegionAvail().x;
		ImGui::InvisibleButton("##File", ImVec2(width, rowHeight));
		const bool hovered = ImGui::IsItemHovered();
		const ImU32 background = selected
			? IM_COL32(38, 72, 111, 255)
			: hovered ? IM_COL32(45, 52, 64, 255) : 0;
		if (background != 0)
			ImGui::GetWindowDrawList()->AddRectFilled(
				rowMin,
				ImVec2(rowMin.x + width, rowMin.y + rowHeight),
				background
			);
		DrawAssetIcon(ImVec2(rowMin.x + 7.0f, rowMin.y + 4.0f), rowHeight - 8.0f, IconKind(category));
		ImGui::GetWindowDrawList()->AddText(
			ImVec2(rowMin.x + 34.0f, rowMin.y + (rowHeight - ImGui::GetFontSize()) * 0.5f),
			selected ? IM_COL32(245, 249, 255, 255) : IM_COL32(215, 220, 228, 255),
			file.c_str()
		);
		if (searching)
		{
			const char* categoryName = GetCategoryName(category);
			const ImVec2 categorySize = ImGui::CalcTextSize(categoryName);
			ImGui::GetWindowDrawList()->AddText(
				ImVec2(rowMin.x + width - categorySize.x - 12.0f, rowMin.y + (rowHeight - ImGui::GetFontSize()) * 0.5f),
				IM_COL32(130, 140, 154, 255),
				categoryName
			);
		}
		if (ImGui::IsItemClicked())
		{
			selectedAsset = file;
			selectedCategory = category;
		}
		if (category == AssetCategory::Scenes &&
			ImGui::IsItemHovered() &&
			ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		{
			LoadScenePreset("assets/Assets/Scenes/" + file);
		}
		ImGui::PopID();
	};

	if (assetListView)
	{
		if (searching)
		{
			for (const auto& file : assetNames[0]) drawFileEntry(file, AssetCategory::Scenes);
			for (const auto& file : assetNames[1]) drawFileEntry(file, AssetCategory::Materials);
			for (const auto& file : assetNames[2]) drawFileEntry(file, AssetCategory::Models);
			for (const auto& file : assetNames[3]) drawFileEntry(file, AssetCategory::Textures);
		}
		else
		{
			switch (currentAssetFolder)
			{
			case AssetFolder::Root:
				drawFolderEntry("Scenes", AssetFolder::Scenes);
				drawFolderEntry("Materials", AssetFolder::Materials);
				drawFolderEntry("Models", AssetFolder::Models);
				drawFolderEntry("Textures", AssetFolder::Textures);
				break;
			case AssetFolder::Scenes:
				for (const auto& file : assetNames[0]) drawFileEntry(file, AssetCategory::Scenes);
				break;
			case AssetFolder::Materials:
				for (const auto& file : assetNames[1]) drawFileEntry(file, AssetCategory::Materials);
				break;
			case AssetFolder::Models:
				for (const auto& file : assetNames[2]) drawFileEntry(file, AssetCategory::Models);
				break;
			case AssetFolder::Textures:
				for (const auto& file : assetNames[3]) drawFileEntry(file, AssetCategory::Textures);
				break;
			}
		}
	}
	else
	{
		ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8.0f, 8.0f));
		if (ImGui::BeginTable("##AssetGrid", 5, ImGuiTableFlags_SizingStretchSame))
		{
			auto gridFolder = [&](const char* label, AssetFolder folder)
			{
				if (!ContainsSearch(label, searchTerm))
					return;
				ImGui::TableNextColumn();
				const ImVec2 tileMin = ImGui::GetCursorScreenPos();
				const float tileWidth = ImGui::GetContentRegionAvail().x;
				ImGui::PushID(label);
				ImGui::InvisibleButton("##FolderTile", ImVec2(tileWidth, 76.0f * assetZoom));
				if (ImGui::IsItemHovered())
					ImGui::GetWindowDrawList()->AddRectFilled(
						tileMin,
						ImVec2(tileMin.x + tileWidth, tileMin.y + 76.0f * assetZoom),
						IM_COL32(45, 52, 64, 255),
						3.0f
					);
				DrawAssetIcon(ImVec2(tileMin.x + tileWidth * 0.5f - 16.0f, tileMin.y + 8.0f), 32.0f, 0);
				const ImVec2 textSize = ImGui::CalcTextSize(label);
				ImGui::GetWindowDrawList()->AddText(
					ImVec2(tileMin.x + std::max(4.0f, tileWidth * 0.5f - textSize.x * 0.5f), tileMin.y + 50.0f),
					IM_COL32(220, 224, 230, 255),
					label
				);
				if (ImGui::IsItemClicked())
				{
					currentAssetFolder = folder;
					selectedAsset.clear();
				}
				ImGui::PopID();
			};

			auto gridFile = [&](const std::string& file, AssetCategory category)
			{
				if (!ContainsSearch(file, searchTerm))
					return;
				ImGui::TableNextColumn();
				const ImVec2 tileMin = ImGui::GetCursorScreenPos();
				const float tileWidth = ImGui::GetContentRegionAvail().x;
				const bool selected = selectedAsset == file;
				ImGui::PushID(file.c_str());
				ImGui::InvisibleButton("##Tile", ImVec2(tileWidth, 76.0f * assetZoom));
				const bool hovered = ImGui::IsItemHovered();
				if (selected || hovered)
					ImGui::GetWindowDrawList()->AddRectFilled(
						tileMin,
						ImVec2(tileMin.x + tileWidth, tileMin.y + 76.0f * assetZoom),
						selected ? IM_COL32(38, 72, 111, 255) : IM_COL32(45, 52, 64, 255),
						3.0f
					);
				DrawAssetIcon(
					ImVec2(tileMin.x + tileWidth * 0.5f - 15.0f, tileMin.y + 7.0f),
					30.0f,
					IconKind(category)
				);
				const ImVec2 textSize = ImGui::CalcTextSize(file.c_str());
				ImGui::GetWindowDrawList()->AddText(
					ImVec2(tileMin.x + std::max(4.0f, tileWidth * 0.5f - textSize.x * 0.5f), tileMin.y + 48.0f),
					selected ? IM_COL32(245, 249, 255, 255) : IM_COL32(210, 217, 226, 255),
					file.c_str()
				);
				if (ImGui::IsItemClicked())
				{
					selectedAsset = file;
					selectedCategory = category;
				}
				if (category == AssetCategory::Scenes &&
					ImGui::IsItemHovered() &&
					ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				{
					LoadScenePreset("assets/Assets/Scenes/" + file);
				}
				ImGui::PopID();
			};

			if (searching)
			{
				for (const auto& file : assetNames[0]) gridFile(file, AssetCategory::Scenes);
				for (const auto& file : assetNames[1]) gridFile(file, AssetCategory::Materials);
				for (const auto& file : assetNames[2]) gridFile(file, AssetCategory::Models);
				for (const auto& file : assetNames[3]) gridFile(file, AssetCategory::Textures);
			}
			else if (currentAssetFolder == AssetFolder::Root)
			{
				gridFolder("Scenes", AssetFolder::Scenes);
				gridFolder("Materials", AssetFolder::Materials);
				gridFolder("Models", AssetFolder::Models);
				gridFolder("Textures", AssetFolder::Textures);
			}
			else if (currentAssetFolder == AssetFolder::Scenes)
			{
				for (const auto& file : assetNames[0]) gridFile(file, AssetCategory::Scenes);
			}
			else if (currentAssetFolder == AssetFolder::Materials)
			{
				for (const auto& file : assetNames[1]) gridFile(file, AssetCategory::Materials);
			}
			else if (currentAssetFolder == AssetFolder::Models)
			{
				for (const auto& file : assetNames[2]) gridFile(file, AssetCategory::Models);
			}
			else if (currentAssetFolder == AssetFolder::Textures)
			{
				for (const auto& file : assetNames[3]) gridFile(file, AssetCategory::Textures);
			}
			ImGui::EndTable();
		}
		ImGui::PopStyleVar();
	}
	ImGui::EndChild();

	ImGui::Separator();
	ImGui::TextDisabled(
		uiLanguage == UiLanguage::Chinese ? "%zu 个资源" : "%zu assets",
		assetNames[0].size() + assetNames[1].size() + assetNames[2].size() + assetNames[3].size()
	);
	if (!selectedAsset.empty())
	{
		ImGui::SameLine();
		ImGui::TextDisabled("| %s / %s", GetCategoryName(selectedCategory), selectedAsset.c_str());
	}
	ImGui::SameLine();
	ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 220.0f);
	ImGui::TextDisabled("%s", Tr("缩放", "Zoom"));
	ImGui::SameLine();
	ImGui::SetNextItemWidth(125.0f);
	ImGui::SliderFloat("##AssetZoom", &assetZoom, 0.75f, 1.5f, "%.2f");

	ImGui::EndChild();
	ImGui::End();
}

