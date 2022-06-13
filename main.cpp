#include "main.h"
//this is horrible, messy code.
//read https://github.com/nblockbuster/D2StaticDocs on the basics of statics

namespace fs = std::filesystem;

static void show_usage()
{
	std::cerr << "Usage: D2StaticExtractor -p [packages path] -o [output path] -i [input hash] -b [package ID] -f [filetype] -l -t"
		<< std::endl;
	std::cerr << "-i extracts a single main model hash\n";
	std::cerr << "-b extracts all the static models available for that package ID. (-t is ignored)\n";
	std::cerr << "-l disables hacky lod culling\n";
	std::cerr << "-t extracts textures for given model\n";
	std::cerr << "-f changes the file type that the textures are output to. (default PNG) Valid types:\n";
	std::cerr << "bmp, jpg/jpeg, png, dds, tga, hdr, tif/tiff, wdp/hdp/jxr, ppm/pfm\n";
	//Found here, under the file-type list: https://github.com/Microsoft/DirectXTex/wiki/Texconv#optional-switches-description
}

int main(int argc, char* argv[])
{
#pragma region Sarge
	Sarge sarge;
	sarge.setArgument("p", "pkgspath", "pkgs path", true);
	sarge.setArgument("o", "outputpath", "output path", true);
	sarge.setArgument("i", "inputhash", "hash of mainmodel file", true);
	sarge.setArgument("b", "batch", "batch with pkg ID", true);
	sarge.setArgument("l", "lodcull", "disable lod culling", false);
	sarge.setArgument("t", "texex", "texture extraction", false);
	sarge.setArgument("f", "filetype", "type to convert to", true);
	sarge.setArgument("d", "loadhash", "loadzone hash", true);
	sarge.setDescription("Destiny 2 static model extractor by nblock.");
	sarge.setUsage("D2StaicExtractor");
	if (!sarge.parseArguments(argc, argv))
	{
		std::cerr << "Couldn't parse arguments..." << std::endl;
		show_usage();
		return 1;
	}
	std::string pkgsPath, outputPath, batchPkg;
	std::string lzHash = "";
	std::string modelHash = "";
	std::string texTypeIn = "";
	uint32_t sfhash32;
	float scale = 1;
	sarge.getFlag("pkgspath", pkgsPath);
	sarge.getFlag("outputpath", outputPath);
	sarge.getFlag("inputhash", modelHash);
	sarge.getFlag("batch", batchPkg);
	sarge.getFlag("filetype", texTypeIn);
	sarge.getFlag("loadhash", lzHash);
	lodCulling = true;
	if (sarge.exists("lodcull"))
		lodCulling = false;
	submesh->lodCulling = lodCulling;
	if (pkgsPath == "" && outputPath == "")
		show_usage();
	packagesPath = pkgsPath;
#pragma endregion

#pragma region H64
	std::unordered_map<uint64_t, uint32_t> hash64Table;
	if (sarge.exists("texex")) {
		std::ifstream f("h64");
		if (f) {
			hash64Table = loadH64Table();
			if (hash64Table.size() < 10000) {
				hash64Table = generateH64Table(pkgsPath);
			}
		}
		else {
			hash64Table = generateH64Table(pkgsPath);
			saveH64Table(hash64Table);
		}
	}
#pragma endregion

	if (modelHash != "") {
		if (getReferenceFromHash(modelHash, packagesPath) != "446d8080") {
			std::cout << "Not a valid static model.\n";
			exit(112);
		}
		outputPath += "/" + modelHash;
		fs::create_directories(outputPath);
		int fileSize;
		hash = modelHash;
		fileSize = getFile();
		memcpy((char*)&sfhash32, data + 0x8, 4);
		memcpy((char*)&scale, data + 0x3C, 4);
		if (scale == 0.000000)
			scale = 1;
		uint32_t extOff = 0;
		delete[] data;
		hash = uint32ToHexStr(sfhash32);
		fileSize = getFile();

		float uoff, voff, uscale, vscale;
		memcpy((char*)&uscale, data + 0x48, 4);
		memcpy((char*)&vscale, data + 0x50, 4);
		memcpy((char*)&uoff, data + 0x54, 4);
		memcpy((char*)&voff, data + 0x58, 4);
		submesh->scales.push_back(uscale);
		submesh->scales.push_back(vscale);
		submesh->offset.push_back(uoff);
		submesh->offset.push_back(voff);

		uint32_t texOffset, texCount;
		uint16_t PartIndex, Unk04;
		int8_t Unk02, Unk03;
		memcpy((char*)&texOffset, data + 0x10, 4);
		texOffset += 0x10;
		memcpy((char*)&texCount, data + texOffset, 4);
		texOffset += 0x10;
		for (int b = texOffset; b < texOffset + texCount * 6; b += 0x6)
		{
			memcpy((char*)&PartIndex, data + b, 2);
			memcpy((char*)&Unk02, data + b + 2, 1);
			memcpy((char*)&Unk03, data + b + 3, 1);
			memcpy((char*)&Unk04, data + b + 4, 2);
			std::cout << "Part Index: " << std::to_string(PartIndex) << " Unk02: " <<
				std::to_string(Unk02) << " Unk03: " << std::to_string(Unk03)
				<< " Unk04: " << std::to_string(Unk04) << "\n";
		}


		uint32_t val;
		memcpy((char*)&val, data + fileSize - 0x14, 4);
		if (uint32ToHexStr(val).substr(uint32ToHexStr(val).length() - 2, 2) != "80")
		{
			std::cerr << "Mesh has no valid data attached.\n";
			exit(2820);
		}

		IndexBufferHeader* indexBufHeader = new IndexBufferHeader(uint32ToHexStr(val), packagesPath);

		memcpy((char*)&val, data + fileSize - 0x10, 4);
		if (uint32ToHexStr(val).substr(uint32ToHexStr(val).length() - 2, 2) != "80")
		{
			std::cerr << "Mesh has no valid data attached.\n";
			exit(2820);
		}

		VertexBuffer* vertBuf = new VertexBuffer(getReferenceFromHash(uint32ToHexStr(val), packagesPath), packagesPath, submesh);

		memcpy((char*)&val, data + fileSize - 0xC, 4);
		if (val != 0xFFFFFFFF)
		{
			VertexBuffer* vertUVBuf = new VertexBuffer(getReferenceFromHash(uint32ToHexStr(val), packagesPath), packagesPath, submesh);
			vertUVBuf->parseVertUV();
		}
		memcpy((char*)&val, data + fileSize - 0x8, 4);
		if (val != 0xFFFFFFFF)
		{
			VertexBuffer* vertVCBuf = new VertexBuffer(getReferenceFromHash(uint32ToHexStr(val), packagesPath), packagesPath, submesh);
			vertVCBuf->parseVertexColor();
			addVertColSlots(submesh);
		}

		vertBuf->parseVertPos();
		vertBuf->parseVertNorm();
		transformPos(scale);
		transformUV();
		indexBufHeader->indexBuffer->getFaces(submesh);

		//This is very experimental and might not work well.
		//It could work much better now, but for some reason I stll dont trust it, even though it works.
		if (lodCulling)
		{
			uint32_t amountLOD, lodTableOffset;
			memcpy((char*)&lodTableOffset, data + 0x20, 4);
			lodTableOffset += 0x20;
			memcpy((char*)&amountLOD, data + lodTableOffset, 4);
			lodTableOffset += 0x10;
			int j = 0;
			uint32_t splitnext_off;
			uint32_t splitnext_count;
			for (int o = lodTableOffset; o < lodTableOffset + (amountLOD * 12); o += 12) {
				LODSplit split;
				memcpy((char*)&split.IndexOffset, data + o, 4);
				memcpy((char*)&split.IndexCount, data + o + 4, 4);
				memcpy((char*)&split.DetailLevel, data + o + 0xA, 1);
				memcpy((char*)&splitnext_off, data + o + 12, 4);
				memcpy((char*)&splitnext_count, data + o + 16, 4);
				if (split.DetailLevel != 1)
				{
					continue;
				}
				else
					submesh->lodsplit.push_back(split);
				j += 1;
			}

			delete[] data;

			for (const auto& split : submesh->lodsplit) {
				if (split.DetailLevel != 1) {
					continue;
				}
				Submesh* newsub = new Submesh();
				int o = 0;
				newsub->vertPos = submesh->vertPos;
				newsub->vertUV = submesh->vertUV;
				newsub->vertNorm = submesh->vertNorm;
				newsub->vertCol = submesh->vertCol;
				newsub->vertColSlots = submesh->vertColSlots;
				for (int i = split.IndexOffset; i < split.IndexOffset + split.IndexCount; i++) {
					newsub->faces.push_back(submesh->faces[i / 3]);
				}

				//indexBufHeader->indexBuffer->getFaces(newsub, split.IndexOffset, split.IndexCount);

				submeshes.push_back(newsub);
			}
		}
		else {
			delete[] data;
			submeshes.push_back(submesh);
		}
		
		for (int p = 0; p < submeshes.size(); p++)
		{
			submeshes[p]->name = modelHash + "_" + std::to_string(p);
			FbxNode* node = fbxModel->addSubmeshToFbx(submeshes[p]);
			nodes.push_back(node);
		}
		if (nodes.size()) {
			for (auto& node : nodes) fbxModel->scene->GetRootNode()->AddChild(node);
			std::string fbxpath = outputPath + "/" + modelHash + ".fbx";
			fbxModel->save(fbxpath, false);
		}

		//parse MATERIALS ?
		if (sarge.exists("texex")) {
			std::string fullSavePath = outputPath + "/textures/";
			fs::create_directories(fullSavePath);
			std::string texType;
			bool found = false;
			std::vector<std::string> validTypes = { "png","jpg","jpeg","bmp","dds","tga","hdr","tif","tiff","wdp","hdp","jxr","ppm","pfm" };
			while (true) {
				if (texTypeIn == "") {
					texType = "PNG";
					break;
				}
				for (const auto& type : validTypes)
				{
					if (boost::iequals(texTypeIn, type)) {
						found = true;
						texType = boost::to_upper_copy(type);
						break;
					}
				}
				if (found)
					break;
				else {
					texType = "PNG";
					break;
				}
			}
			hash = modelHash;
			fileSize = getFile();
			uint32_t extOff, val;
			memcpy((char*)&extOff, data + 0x18, 4);
			extOff += 0x18;
			std::vector<Material*> externalMaterials;
			uint32_t extCount;
			memcpy((char*)&extCount, data + extOff, 4);
			extOff += 0x10;
			std::set<std::uint32_t> existingMats;
			for (int j = extOff; j < extOff + extCount * 4; j += 4)
			{
				memcpy((char*)&val, data + j, 4);
				if (existingMats.find(val) == existingMats.end())
				{
					Material* mat = new Material(uint32ToHexStr(val), packagesPath);
					externalMaterials.push_back(mat);
					existingMats.insert(val);
				}
			}
			for (auto mat : externalMaterials)
			{
				mat->parseMaterial(hash64Table);
				mat->exportTextures(fullSavePath, texType);
			}
		}

		free(submesh);
		fbxModel->manager->Destroy();

		std::cout << modelHash + ".fbx extracted.\n";
	}
	else if (batchPkg != "") {
		Package pkg(batchPkg, packagesPath);
		std::vector<std::string> hashes = pkg.getAllFilesGivenRef("446d8080");
		outputPath += "/" + batchPkg + "/";
		if (!fs::exists(outputPath))
			fs::create_directories(outputPath);
		std::cout << "Number of files to batch extract: " << std::to_string(hashes.size()) << "\n";
		for (auto& modelhash : hashes)
		{
			if (modelhash == "")
				continue;
			int fileSize;
			hash = modelhash;
			fileSize = getFile();
			memcpy((char*)&sfhash32, data + 0x8, 4);
			memcpy((char*)&scale, data + 0x3C, 4);
			if (scale == 0.000000)
				scale = 1;
			uint32_t extOff = 0;
			delete[] data;
			hash = uint32ToHexStr(sfhash32);
			fileSize = getFile();

			float uoff, voff, uscale, vscale;
			memcpy((char*)&uscale, data + 0x48, 4);
			memcpy((char*)&vscale, data + 0x50, 4);
			memcpy((char*)&uoff, data + 0x54, 4);
			memcpy((char*)&voff, data + 0x58, 4);
			submesh->scales.push_back(uscale);
			submesh->scales.push_back(vscale);
			submesh->offset.push_back(uoff);
			submesh->offset.push_back(voff);

			uint32_t val;
			memcpy((char*)&val, data + fileSize - 0x14, 4);
			if (uint32ToHexStr(val).substr(uint32ToHexStr(val).length() - 2, 2) != "80") continue;

			IndexBufferHeader* indexBufHeader = new IndexBufferHeader(uint32ToHexStr(val), packagesPath);

			memcpy((char*)&val, data + fileSize - 0x10, 4);
			if (uint32ToHexStr(val).substr(uint32ToHexStr(val).length() - 2, 2) != "80") continue;

			VertexBuffer* vertBuf = new VertexBuffer(getReferenceFromHash(uint32ToHexStr(val), packagesPath), packagesPath, submesh);

			memcpy((char*)&val, data + fileSize - 0xC, 4);
			if (val != 0xFFFFFFFF)
			{
				VertexBuffer* vertUVBuf = new VertexBuffer(getReferenceFromHash(uint32ToHexStr(val), packagesPath), packagesPath, submesh);
				vertUVBuf->parseVertUV();
			}
			memcpy((char*)&val, data + fileSize - 0x8, 4);
			if (val != 0xFFFFFFFF)
			{
				VertexBuffer* vertVCBuf = new VertexBuffer(getReferenceFromHash(uint32ToHexStr(val), packagesPath), packagesPath, submesh);
				vertVCBuf->parseVertexColor();
				addVertColSlots(submesh);
			}
			vertBuf->parseVertPos();
			vertBuf->parseVertNorm();
			transformPos(scale);
			transformUV();
			indexBufHeader->indexBuffer->getFaces(submesh);

			//This is very experimental and might not work well.
			//It could work much better now, but for some reason I stll dont trust it, even though it works.
			if (lodCulling)
			{
				uint32_t amountLOD, lodTableOffset;
				memcpy((char*)&lodTableOffset, data + 0x20, 4);
				lodTableOffset += 0x20;
				memcpy((char*)&amountLOD, data + lodTableOffset, 4);
				lodTableOffset += 0x10;
				int j = 0;
				uint32_t splitnext_off;
				uint32_t splitnext_count;
				for (int o = lodTableOffset; o < lodTableOffset + (amountLOD * 12); o += 12) {
					LODSplit split;
					memcpy((char*)&split.IndexOffset, data + o, 4);
					memcpy((char*)&split.IndexCount, data + o + 4, 4);
					memcpy((char*)&split.DetailLevel, data + o + 0xA, 1);
					memcpy((char*)&splitnext_off, data + o + 12, 4);
					memcpy((char*)&splitnext_count, data + o + 16, 4);
					if (split.DetailLevel != 1)
					{
						continue;
					}
					else
						submesh->lodsplit.push_back(split);
					j += 1;
				}

				delete[] data;

				for (const auto& split : submesh->lodsplit) {
					if (split.DetailLevel != 1) {
						continue;
					}
					Submesh* newsub = new Submesh();
					int o = 0;
					newsub->vertPos = submesh->vertPos;
					newsub->vertUV = submesh->vertUV;
					newsub->vertNorm = submesh->vertNorm;
					newsub->vertCol = submesh->vertCol;
					newsub->vertColSlots = submesh->vertColSlots;
					for (int i = split.IndexOffset; i < split.IndexOffset + split.IndexCount; i++) {
						newsub->faces.push_back(submesh->faces[i / 3]);
					}

					//indexBufHeader->indexBuffer->getFaces(newsub, split.IndexOffset, split.IndexCount);

					submeshes.push_back(newsub);
				}
			}
			else {
				delete[] data;
				submeshes.push_back(submesh);
			}
			for (int p = 0; p < submeshes.size(); p++)
			{
				submeshes[p]->name = modelhash + "_" + std::to_string(p);
				FbxNode* node = fbxModel->addSubmeshToFbx(submeshes[p]);
				nodes.push_back(node);
			}
			if (nodes.size()) {
				for (auto& node : nodes) fbxModel->scene->GetRootNode()->AddChild(node);
				std::string fbxpath = outputPath + "/" + modelhash + ".fbx";
				fbxModel->save(fbxpath, false);
			}

			submesh->faces.clear();
			submesh->vertPos.clear();
			submesh->vertPosW.clear();
			submesh->vertUV.clear();
			submesh->vertCol.clear();
			submesh->vertNorm.clear();
			submesh->vertNormW.clear();
			submesh->vertColSlots.clear();
			submesh->scales.clear();
			submesh->offset.clear();

			std::cout << modelhash + ".fbx extracted.\n";
			submesh->name.clear();
			nodes.clear();
			fbxModel->scene->Clear();
			submeshes.clear();
			//free(submesh);
			//if (lodCulling)
				//free(newsub);
		}
		std::cout << "Finished extracting " + std::to_string(hashes.size()) << " models.\n";
		fbxModel->manager->Destroy();
	}
}

void addVertColSlots(Submesh* submesh){
	for (auto& w : submesh->vertNormW)
	{
		std::vector<float> vc = { 0., 0., 0., 1. };
		switch (w & 0x7)
		{
		case 0:
			vc[0] = 0.333;
			break;
		case 1:
			vc[0] = 0.666;
			break;
		case 2:
			vc[0] = 0.999;
			break;
		case 3:
			vc[1] = 0.333;
			break;
		case 4:
			vc[1] = 0.666;
			break;
		case 5:
			vc[1] = 0.999;
			break;
		}
		submesh->vertColSlots.push_back(vc);
	}
}

int getFile()
{
	pkgID = getPkgID(hash);
	Package pkg(pkgID, packagesPath);
	int fileSize;
	data = pkg.getEntryData(hash, fileSize);
	if (data == nullptr || sizeof(data) == 0) return 0;
	return fileSize;
}

void transformUV()
{
	for (auto& vert : submesh->vertUV)
	{
		vert[0] = vert[0] * submesh->scales[0] + submesh->offset[0];
		vert[1] = vert[1] * -submesh->scales[1] + (1 - submesh->offset[1]);
	}
}

void transformPos(float scale)
{
	for (auto& vert : submesh->vertPos)
	{
		for (int i = 0; i < 3; i++)
		{
			vert[i] = vert[i] * scale;
		}
	}
}

void Submesh::clear()
{
	submesh->vertPos.clear();
	submesh->vertPosW.clear();
	submesh->vertUV.clear();
	submesh->vertNorm.clear();
	submesh->vertNormW.clear();
	submesh->vertCol.clear();
	submesh->vertColSlots.clear();
	submesh->faces.clear();
	submesh->lodsplit.clear();
	submesh->faceMap.clear();
	submesh->offset.clear();
	submesh->scales.clear();
}
