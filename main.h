#pragma once
#include "helpers.h"
#include "fbxmodel.h"
#include "texture.h"
#include "parsers.h"
#include "dxgiformat.h"
#include <sarge.cpp>
//#include <thread>

Logger logger;

unsigned char* data = nullptr;
std::string hash = "";
std::string pkgID = "";
std::string packagesPath = "";
bool lodCulling = true;
std::string largeHash;
Submesh* submesh = new Submesh();
FbxModel* fbxModel = new FbxModel();
int getFile();
std::vector<FbxNode*> nodes;
void addVertColSlots(Submesh* submesh);

std::vector<LODSplit> IndexLODSplits;
std::vector<Submesh*> submeshes;

struct LookupTable
{
	uint16_t EntryA;
	uint16_t EntryB;
	uint16_t EntryC;
	uint16_t EntryD;
};

void transformUV(Submesh* sub);
void transformPos(Submesh* sub, Vector4 pos_off);

std::vector<std::vector<float_t>> trimVertsData(std::vector<std::vector<float_t>> verts, std::set<int> dsort, bool bVertCol);
bool ExportSingleLoadZone(std::string lzHash, std::string outputPath, bool bl, bool bTextures, std::string texTypeIn);
