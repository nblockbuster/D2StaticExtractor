#include "parsers.h"

void VertexBuffer::parseVertPos()
{
	int fileSize = getData();
	int16_t num;
	for (int i = 0; i < fileSize; i += 0x10) {
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

void VertexBuffer::parseVertUV()
{
	int fileSize = getData();
	int16_t u, v;
	for (int i = 0; i < fileSize; i += 0x4) {
		std::vector<float> uvert;
		uvert.reserve(2);
		memcpy((char*)&u, data + i, 2);
		memcpy((char*)&v, data + i + 2, 2);
		//i need to fix this because it isnt right still
		//how did i have this backwards??
		//eh who cares
		//OFF IS SCALE AND SCALE IS OFF NOW
		//float fixu = ((float)u / 32767); //*uoff + uscale;
		//float fixv = ((float)v / 32767); //*-voff + vscale;
		//float fixu = ((float)u / 32767) * uscale + uoff;// *0.08;
		//float fixv = ((float)v / 32767) * -vscale + voff;// *0.08;
		uvert.push_back((float)u / 32767);
		uvert.push_back((float)v / 32767);
		submesh->vertUV.push_back(uvert);
	}
	delete[] data;
}