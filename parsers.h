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
	void parseVertNorm();
	void parseVertUV();
	void parseVertexColor();
};

class IndexBuffer : public File
{
public:
	int stride = -1;

	IndexBuffer(std::string x, std::string pkgsPath, int s) : File(x, pkgsPath)
	{
		stride = s;
	}
	void getFaces(Submesh* submesh);
};

class IndexBufferHeader : public Header
{
private:
	void getHeader(std::string x);
public:
	int stride = 2;
	IndexBuffer* indexBuffer = nullptr;
	IndexBufferHeader(std::string x, std::string pkgsPath) : Header(x, pkgsPath)
	{
		if (x != "")
		{
			getData();
			getHeader(x);
			indexBuffer = new IndexBuffer(getReferenceFromHash(x, pkgsPath), pkgsPath, stride);
		}
	}
};