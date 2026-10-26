#pragma once

#include "instancedMesh.h"

class SimpleObjLoader
{
public:
	static InstancedMesh* loadInstanced(const string& path, unsigned int instanceCount);
};

