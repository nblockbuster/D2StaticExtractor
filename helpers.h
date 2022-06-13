#pragma once
#include <iostream>
#include <string>
#include <filesystem>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <stdint.h>
#include <filesystem>
#include <stdio.h>
#include <string>
#include <functional>
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include "package.h"
#include <cmath>
#include "dxgiformat.h"

std::string getReferenceFromHash(std::string hash, std::string pkgsPath);
std::string getHash64(uint64_t hash64, std::unordered_map<uint64_t, uint32_t> hash64Table);
std::string getPkgID(std::string hash);
uint16_t getPkgID(uint32_t hash);

std::string uint16ToHexStr(uint16_t num);
std::string uint32ToHexStr(uint32_t num);
uint16_t swapUInt16Endianness(uint16_t x);
uint32_t swapUInt32Endianness(uint32_t x);
uint64_t swapUInt64Endianness(uint64_t x);
uint32_t hexStrToUint16(std::string hash);
uint32_t hexStrToUint32(std::string hash);
uint64_t hexStrToUint64(std::string hash); 
//std::string getReferenceFromHash(std::string hash);
std::string getPkgID(std::string hash);
uint16_t getPkgID(uint32_t hash);

std::string getFileFromHash(std::string hsh);
std::string load3(const std::string& path);

void filePutContents(const std::string& name, const std::string& content);

std::string to_str(double a_value);

struct LODSplit
{
	uint32_t IndexOffset;
	uint32_t IndexCount;
	uint8_t DetailLevel;
};

class Submesh
{
private:
public:
	std::vector<std::vector<float>> vertPos;
	std::vector<std::vector<float>> vertNorm;
	std::vector<std::vector<float>> vertUV;
	std::vector<std::vector<float>> vertCol;
	std::vector<std::vector<uint32_t>> faces;
	std::vector<int16_t> vertNormW;
	std::vector<int16_t> vertPosW;
	std::vector<std::vector<float>> vertColSlots;
	int lodLevel;
	std::string name;
	bool isU32;
	int type;
	std::vector<LODSplit> lodsplit;
	std::vector<float> scales;
	std::vector<float> offset;
	std::unordered_map<int, int> faceMap;
	bool lodCulling;
	void clear();
};

class File
{
private:

public:
	File(std::string x, std::string pkgsPath);

	std::string hash = "";
	unsigned char* data = nullptr;
	std::string pkgID = "";
	std::string packagesPath;
	int getData();
};

class Header : public File
{
private:

public:
	Header(std::string x, std::string pkgsPath) : File(x, pkgsPath) {}
};

