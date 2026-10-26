#include "simpleObjLoader.h"
#include "../material/phongMaterial.h"

#include <fstream>
#include <sstream>

namespace
{
	struct ObjIndex
	{
		int position = 0;
		int texCoord = 0;
		int normal = 0;
	};

	ObjIndex ParseIndex(const string& token)
	{
		ObjIndex result;
		string first;
		string second;
		string third;
		std::stringstream stream(token);
		std::getline(stream, first, '/');
		std::getline(stream, second, '/');
		std::getline(stream, third, '/');

		if (!first.empty())
			result.position = std::stoi(first);
		if (!second.empty())
			result.texCoord = std::stoi(second);
		if (!third.empty())
			result.normal = std::stoi(third);
		return result;
	}

	void AppendVertex(
		const ObjIndex& index,
		const vector<vec3>& positions,
		const vector<vec3>& colors,
		const vector<vec2>& uvs,
		const vector<vec3>& normals,
		vector<float>& outPositions,
		vector<float>& outColors,
		vector<float>& outUvs,
		vector<float>& outNormals,
		vector<unsigned int>& outIndices
	)
	{
		const int positionIndex = index.position - 1;
		if (positionIndex < 0 || positionIndex >= static_cast<int>(positions.size()))
			return;

		outIndices.push_back(static_cast<unsigned int>(outPositions.size() / 3));

		const vec3& position = positions[positionIndex];
		outPositions.insert(outPositions.end(), { position.x, position.y, position.z });

		const vec3 color = positionIndex < static_cast<int>(colors.size())
			? colors[positionIndex]
			: vec3(1.0f);
		outColors.insert(outColors.end(), { color.r, color.g, color.b });

		const vec2 uv = index.texCoord > 0 && index.texCoord <= static_cast<int>(uvs.size())
			? uvs[index.texCoord - 1]
			: vec2(0.0f);
		outUvs.insert(outUvs.end(), { uv.x, uv.y });

		const vec3 normal = index.normal > 0 && index.normal <= static_cast<int>(normals.size())
			? normals[index.normal - 1]
			: vec3(0.0f, 1.0f, 0.0f);
		outNormals.insert(outNormals.end(), { normal.x, normal.y, normal.z });
	}
}

InstancedMesh* SimpleObjLoader::loadInstanced(const string& path, unsigned int instanceCount)
{
	std::ifstream file(path);
	if (!file)
	{
		cerr << "OBJ file not found: " << path << endl;
		return nullptr;
	}

	vector<vec3> positions;
	vector<vec3> colors;
	vector<vec2> uvs;
	vector<vec3> normals;
	vector<float> outPositions;
	vector<float> outColors;
	vector<float> outUvs;
	vector<float> outNormals;
	vector<unsigned int> indices;

	string line;
	while (std::getline(file, line))
	{
		if (line.empty() || line[0] == '#')
			continue;

		std::stringstream stream(line);
		string type;
		stream >> type;

		if (type == "v")
		{
			vec3 position;
			vec3 color(1.0f);
			stream >> position.x >> position.y >> position.z;
			if (!(stream >> color.r >> color.g >> color.b))
				color = vec3(1.0f);
			positions.push_back(position);
			colors.push_back(color);
		}
		else if (type == "vt")
		{
			vec2 uv;
			stream >> uv.x >> uv.y;
			uvs.push_back(uv);
		}
		else if (type == "vn")
		{
			vec3 normal;
			stream >> normal.x >> normal.y >> normal.z;
			normals.push_back(normal);
		}
		else if (type == "f")
		{
			vector<ObjIndex> face;
			string token;
			while (stream >> token)
				face.push_back(ParseIndex(token));

			for (size_t i = 1; i + 1 < face.size(); ++i)
			{
				AppendVertex(face[0], positions, colors, uvs, normals,
					outPositions, outColors, outUvs, outNormals, indices);
				AppendVertex(face[i], positions, colors, uvs, normals,
					outPositions, outColors, outUvs, outNormals, indices);
				AppendVertex(face[i + 1], positions, colors, uvs, normals,
					outPositions, outColors, outUvs, outNormals, indices);
			}
		}
	}

	if (outPositions.empty() || outNormals.empty() || outUvs.empty())
	{
		cerr << "OBJ contains no drawable geometry: " << path << endl;
		return nullptr;
	}

	auto* geometry = new Geometry(outPositions, outNormals, outUvs, outColors, indices);
	auto* material = new PhongMaterial();
	return new InstancedMesh(geometry, material, instanceCount);
}

