#pragma once
#include "helpers.h"
#include "fbxmodel.h"
#include <sarge.cpp>

unsigned char* data = nullptr;
unsigned char* lzdata = nullptr;
std::string hash = "";
std::string pkgID = "";
std::string packagesPath = "";
bool lodCulling = true;
//float lzscale;

struct Vector3
{
	float x;
	float y;
	float z;
};

struct Vector4
{
	float x;
	float y;
	float z;
	float w;
};

struct Vector2
{
	float u;
	float v;
};

struct Faces
{
	int16_t x;
	int16_t y;
	int16_t z;
};

std::vector<Vector4> Translation;
std::vector<uint16_t> PosLookup;

int textureFormat;
uint16_t width;
uint16_t height;
uint16_t arraySize;
std::string largeHash;

Submesh* submesh = new Submesh();
FbxModel* fbxModel = new FbxModel();
int stride;

int getFile();

std::vector<FbxNode*> nodes;
//std::vector<StaticMesh*> meshes;
void addVertColSlots(Submesh* submesh);