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
#include "package.h"
#include <cmath>
#include "dxgiformat.h"
#include <time.h>

std::string getReferenceFromHash(std::string hash, std::string pkgsPath);
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
// std::string getReferenceFromHash(std::string hash);
std::string getPkgID(std::string hash);
uint16_t getPkgID(uint32_t hash);

std::string getFileFromHash(std::string hsh);
std::string to_str(double a_value);

struct Vector2
{
	double x;
	double y;
};

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

struct LODSplit
{
	uint32_t IndexOffset;
	uint32_t IndexCount;
	uint8_t DetailLevel;
	uint8_t submeshIndex;
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
	Vector2 scales;
	Vector2 offset;
	std::unordered_map<int, int> faceMap;
	bool lodCulling;
	void clear();
	uint32_t someValue;
};

class File
{
private:
public:
	File(std::string x, std::string pkgsPath);

	std::string hash = "";
	unsigned char *data = nullptr;
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

enum ELoggerLevels
{
	Off = 0,
	Error = 1,
	Warn = 2,
	Info = 3,
	Debug = 4
};

class Logger
{
public:
	Logger(ELoggerLevels logLevel) { LoggerLevel = logLevel; }
	Logger() {}

	ELoggerLevels LoggerLevel = ELoggerLevels::Info;

	void Debug(std::string message);
	void Info(std::string message);
	void Warning(std::string message);
	void Error(std::string message);
	std::string currentDateTime();
};
