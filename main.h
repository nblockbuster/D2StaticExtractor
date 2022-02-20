#pragma once
#include "helpers.h"
#include "fbxmodel.h"
#include "texture.h"
#include <sarge.cpp>

unsigned char* data = nullptr;
std::string hash = "";
std::string pkgID = "";
std::string packagesPath = "";
bool lodCulling = true;
Submesh* submesh = new Submesh();
FbxModel* fbxModel = new FbxModel();
int stride;
int getFile();
std::vector<FbxNode*> nodes;
//void addVertColSlots(Submesh* submesh);