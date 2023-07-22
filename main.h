#pragma once
#include "helpers.h"
#include "fbxmodel.h"
#include "texture.h"
#include "parsers.h"
#include "dxgiformat.h"
#include "Sarge/src/sarge.cpp"

#define _USE_MATH_DEFINES
#include <math.h>
//#include <thread>

unsigned char* data = nullptr;
std::string hash = "";
std::string pkgID = "";
std::string packagesPath = "";
bool lodCulling = true;
std::string largeHash = "";
Submesh* submesh = new Submesh();
FbxModel* fbxModel = new FbxModel();
std::vector<FbxNode*> nodes;

std::vector<LODSplit> IndexLODSplits;
std::vector<Submesh*> submeshes;

struct Vector3
{
	double x;
	double y;
	double z;
};

struct Vector4
{
	double x;
	double y;
	double z;
	double w;
};

struct LookupTable
{
	uint16_t EntryA;
	uint16_t EntryB;
	uint16_t EntryC;
	uint16_t EntryD;
};

int getFile();

void transformUV();
void transformPos(Vector4 pos_off, float unk44);

std::vector<std::vector<float_t>> trimVertsData(std::vector<std::vector<float_t>> verts, std::set<int> dsort, bool bVertCol);
void addVertColSlots(Submesh* submesh);

bool ExportSingleLoadZone(std::string lzHash, std::string outputPath, bool bl, bool bTextures, std::string texTypeIn, std::unordered_map<uint64_t, uint32_t> hash64Table);

Vector3 QuatToEulerAngles(Vector4 q);
