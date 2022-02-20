#pragma once
#include "helpers.h"
#include "fbxmodel.h"
#include <sarge.cpp>

unsigned char* data = nullptr;
std::string hash = "";
std::string pkgID = "";
std::string packagesPath = "";
bool lodCulling = true;
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
void addVertColSlots(Submesh* submesh);
