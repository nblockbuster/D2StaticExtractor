#pragma once
#include "helpers.h"
#include "fbxmodel.h"
#include "vertex.h"
#include "index.h"
#include "texture.h"

class StaticModel : public Model
{
private:
	bool bTextures = false;
	FbxModel* fbxModel = nullptr;
	std::vector<Mesh*> meshes;
	std::vector<FbxNode*> nodes;
	std::unordered_map<uint64_t, uint32_t> h64Table;
	std::vector<Material*> externalMaterials;

	bool getBuffers();
	void parseBuffers();

	std::vector<std::vector<float_t>> trimVertsData(std::vector<std::vector<float_t>> verts, std::set<int> dsort, bool bVertCol);
	std::vector<std::vector<uint8_t>> trimVertsData(std::vector<std::vector<uint8_t>> verts, std::set<int> dsort);
public:
	std::string packagesPath;
	StaticModel(std::string x, std::unordered_map<uint64_t, uint32_t> hash64Table, std::string pkgsPath, bool btex) : Model(x, pkgsPath)
	{
		packagesPath = pkgsPath;
		h64Table = hash64Table;
		bTextures = btex;
	}
	StaticModel(std::string x, std::string pkgsPath) : Model(x, pkgsPath)
	{
		packagesPath = pkgsPath;
	}
	bool get();
	void pack(std::string saveDirectory);
	void save(std::string saveDirectory, std::string saveName);
};