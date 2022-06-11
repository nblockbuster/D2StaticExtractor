#pragma once
#include "helpers.h"
#include "fbxmodel.h"
#include "texture.h"
#include "parsers.h"
#include "dxgiformat.h"
#include <sarge.cpp>
//#include <thread>

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

void transformUV();
void transformPos(float scale);