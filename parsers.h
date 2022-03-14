#pragma once
#include "helpers.h"

class VertexBuffer : public File
{
private:
	Submesh* submesh;
public:

	VertexBuffer(std::string x, std::string pkgsPath, Submesh* sub) : File(x, pkgsPath)
	{
		submesh = sub;
	};

	void parseVertPos();
	void parseVertUV();
};

