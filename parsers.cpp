#include "parsers.h"

void VertexBufferHeader::getHeader(std::string x)
{
	memcpy((char*)&stride, data + 4, 2);
}

void VertexBuffer::parseVertPos()
{
	int fileSize = getData();
	int16_t num;
	for (int i = 0; i < fileSize; i += stride) {
		std::vector<float> vertexpos;
		vertexpos.reserve(3);
		for (int j = 0; j < 3; j++)
		{
			memcpy((char*)&num, data + i + j * 2, 2);
			vertexpos.push_back((float)num / 32767);
		}
		submesh->vertPos.push_back(vertexpos);
	}
	delete[] data;
}

void VertexBuffer::parseVertNorm()
{
	int fileSize = getData();
	int16_t num;
	for (int i = 0; i < fileSize; i += stride) {
		std::vector<float> norm;
		norm.reserve(3);
		for (int j = 0; j < 3; j++)
		{
			memcpy((char*)&num, data + i + 8 + j * 2, 2);
			norm.push_back((float)num / 32767);
		}
		submesh->vertNorm.push_back(norm);
		memcpy((char*)&num, data + i + 6, 2);
		submesh->vertPosW.push_back(num);
		memcpy((char*)&num, data + i + 0xE, 2);
		submesh->vertNormW.push_back(num);
	}
	delete[] data;
}

void VertexBuffer::parseVertUV()
{
	int fileSize = getData();
	int16_t u, v;
	for (int i = 0; i < fileSize; i += stride) {
		std::vector<float> uvert;
		uvert.reserve(2);
		memcpy((char*)&u, data + i, 2);
		memcpy((char*)&v, data + i + 2, 2);
		uvert.push_back((float)u / 32767);
		uvert.push_back((float)v / 32767);
		submesh->vertUV.push_back(uvert);
	}
	delete[] data;
}

void VertexBuffer::parseVertexColor()
{
	int fileSize = getData();
	bool bAnyValidVC = false;
	for (int i = 0; i < fileSize; i += stride) {
		int8_t val;
		std::vector<float> vc;
		for (int j = i; j < i + 4; j++) {
			memcpy((char*)&val, data + j, 1);
			if (!bAnyValidVC) {
				if (val != 0) bAnyValidVC = true;
			}
			vc.push_back((float)val / 255);
		}
		submesh->vertCol.push_back(vc);
	}
	if (!bAnyValidVC) submesh->vertCol.clear();
	delete[] data;
}

void IndexBufferHeader::getHeader(std::string x)
{
	bool bIsStride4;
	memcpy((char*)&bIsStride4, data + 0x1, 1);
	if (bIsStride4) stride = 4;
}

void IndexBuffer::getFaces(Submesh* submesh)
{
	int fileSize = getData();
	int increment = 3;
	std::vector<uint32_t> intFacesData;
	intFacesData.reserve(floor(fileSize / stride));
	for (int i = 0; i < fileSize; i += stride)
	{
		uint32_t face = 0;
		memcpy((char*)&face, data + i, stride);
		intFacesData.push_back(face);
	}
	int j = 0;
	int faceIndex = 0;
	while (true)
	{
		if (faceIndex == intFacesData.size())
		{
			submesh->faceMap[faceIndex] = submesh->faces.size() - 1;
			break;
		}
		// Check vector break
		bool bEnd = false;
		for (int i = 0; i < 3; i++)
		{
			if (intFacesData[faceIndex + i] == 65535 or intFacesData[faceIndex + i] == 4294967295)
			{
				bEnd = true;
				break;
			}
		}
		if (bEnd)
		{
			j = 0;
			submesh->faceMap[faceIndex] = submesh->faces.size() - 1;
			faceIndex += increment;
			continue;
		}
		std::vector<uint32_t> face;
		face = std::vector<uint32_t>(intFacesData.begin() + faceIndex, intFacesData.begin() + faceIndex + 3);
		submesh->faces.push_back(face);
		submesh->faceMap[faceIndex] = submesh->faces.size() - 1;
		faceIndex += increment;
		j++;
	}
	delete[] data;
}