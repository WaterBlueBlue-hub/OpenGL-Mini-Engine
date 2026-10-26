#pragma once 
#include "mesh.h"

class InstancedMesh : public Mesh 
{
public:
	InstancedMesh(Geometry* geometry, Material* material, unsigned int instanceCount);
	~InstancedMesh();

	void updateMatrices();
	void sortMatrices(mat4 viewMatrix);
public:
	unsigned int mInstanceCount{ 0 };
	vector<mat4> mInstanceMatrices{};
	unsigned int mMatrixVbo{ 0 };
};