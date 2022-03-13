#include "staticmodel.h"

bool StaticModel::get()
{
	fbxModel = new FbxModel();
	if (getReferenceFromHash(hash, packagesPath) != "446d8080")
	{
		std::cerr << "Given hash is not a valid static model.";
		return false;
	}
	getData();
	if (!getBuffers)
	{
		std::cerr << "Static has no model data attached.";
		return false;
	}
	parseBuffers();
	return meshes.size();
}

bool StaticModel::getBuffers()
{
	int fileSize;
	uint32_t off, subfileID;

	float scale;

	memcpy((char*)&subfileID, data + 8, 4);
	memcpy((void*)&scale, data + 0x3C, 4);
	File* subfile = new File(uint32ToHexStr(subfileID), packagesPath);
	fileSize = subfile->getData();
	if (!fileSize) return false;

}

void StaticModel::parseBuffers()
{

}