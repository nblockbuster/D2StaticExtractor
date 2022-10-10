#pragma once
#pragma comment(lib, "libfbxsdk.lib")
#include "helpers.h"
#include <fbxsdk.h>

class FbxModel
{
private:
	FbxMesh *createMesh(Submesh *submesh);
	void addNorm(FbxMesh *mesh, Submesh *submesh, FbxLayer *layer);
	void addUV(FbxMesh *mesh, Submesh *submesh, FbxLayer *layer);
	void addVC(FbxMesh *mesh, Submesh *submesh, FbxLayer *layer);
	void addVCSlots(FbxMesh *mesh, Submesh *submesh, FbxLayer *layer);
	// void addWeights(FbxMesh* mesh, DynamicSubmesh* submesh, std::vector <Node*> bones);

public:
	FbxNode *addSubmeshToFbx(Submesh *submesh);
	void save(std::string savePath, bool ascii = false);

	FbxManager *manager = FbxManager::Create();
	FbxScene *scene = FbxScene::Create(manager, "");
};