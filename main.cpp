#include "main.h"
//this is horrible, messy code.
//read https://github.com/nblockbuster/D2StaticDocs on the basics of statics

namespace fs = std::filesystem;

#pragma region Parsers

bool parseIndexBufferInMemory(int vertexCount, int fileSize)
{
	//Submesh* sub;
	//std::vector<std::vector<int16_t>> facesi16;
	//std::vector<std::vector<uint32_t>> facesu32;
	int e = 0;
	int b = 0;
	int l = 0;
	int16_t face;
	uint32_t u32face;
	int n = vertexCount;
	for (int i = 0; i < fileSize; i += 0x6) {
		if (submesh->isU32) {
			std::vector<uint32_t> uintFace;
			uintFace.reserve(3);
			for (int j = 0; j < 3; j++) {
				memcpy((char*)&u32face, data + i + j * 4, 4);
				uintFace.push_back(u32face);
			}
			i += 6;
			if (u32face > vertexCount)
				break;
			//facesu32.push_back(uintFace);
			submesh->facesu32.push_back(uintFace);
		}
		else {
			std::vector<int16_t> intFace;
			intFace.reserve(3);
			for (int j = 0; j < 3; j++) {
				memcpy((char*)&face, data + i + j * 2, 2);
				intFace.push_back(face);
			}
			if (face > vertexCount)
				break;
			//facesi16.push_back(intFace);
			submesh->faces.push_back(intFace);
		}
		if (lodCulling) {
			if (b > fileSize / 3) {
				if (fileSize > 12500) {
					if (submesh->faces[l][0] + 8000 < submesh->faces[l - 2][0] && submesh->faces[l][1] + 8000 < submesh->faces[l - 2][1] && submesh->faces[l][2] + 8000 < submesh->faces[l - 2][2]) {
						submesh->faces.resize(submesh->faces.size() - 1);
						return false;
					}
				}
				else if (fileSize > 5500) {
					if (submesh->faces[l][0] + 3500 < submesh->faces[l - 2][0] && submesh->faces[l][1] + 3500 < submesh->faces[l - 2][1] && submesh->faces[l][2] + 3500 < submesh->faces[l - 2][2]) {
						submesh->faces.resize(submesh->faces.size() - 1);
						return false;
					}
				}
				else if ((submesh->faces[l][0] + 200 < submesh->faces[l - 2][0] && submesh->faces[l][1] + 200 < submesh->faces[l - 2][1] && submesh->faces[l][2] + 200 < submesh->faces[l - 2][2])) {
					submesh->faces.resize(submesh->faces.size() - 1);
					return false;
				}
			}
			l += 1;
			b += 3;
			/*
			int c = 0;
			if (submesh->lodsplit[c].off != (submesh->lodsplit[c-1].off + submesh->lodsplit[c-1].count)) {
				c += 1;
				std::cout << std::to_string(c) << "\n";
			}
			if (i == (submesh->lodsplit[c].off + submesh->lodsplit[c].count)) {
				if (facesu32.size()) {
					submesh->facesu32 = facesu32;
					submeshes.push_back(submesh);
					facesu32.clear();
					c += 1;
				}
				else {
					submesh->faces = facesi16;
					submeshes.push_back(submesh);
					facesi16.clear();
					c+=1;
				}
			}
			l += 1;
			b += 3;
			*/
		}
	}
	//Submesh* submesh2 = new Submesh();
	//submesh2->vertCol = submesh->vertCol;
	//submesh2->vertColSlots = submesh->vertColSlots;
	//submesh2->vertNorm = submesh->vertNorm;
	//submesh2->vertPos = submesh->vertPos;
	//submesh2->vertUV = submesh->vertUV;
	//submesh2->faces = facesi16;
	//submesh2->facesu32 = facesu32;
	//submeshes.push_back(submesh2);
	return true;
}

void parseVertexBufferInMemory(int fileSize, float scale)
{
	int16_t x,y,z;
	int16_t num;
	for (int i = 0; i < fileSize; i += 0x10) {
		std::vector<float> vertexpos;
		vertexpos.reserve(3);
		for (int j = 0; j < 3; j++)
		{
			memcpy((char*)&num, data + i + j * 2, 2);
			vertexpos.push_back(((float)num / 32767) * scale);
		}
		submesh->vertPos.push_back(vertexpos);
	}
}

void parseVertexNormalsInMemory(int fileSize) {
	int16_t num;
	uint16_t numW;
	for (int i = 0x8; i < fileSize; i += 0x10) {
		std::vector<float> norm;
		norm.reserve(3);
		for (int j = 0; j < 3; j++)
		{
			memcpy((char*)&num, data + i + j * 2, 2);
			norm.push_back((float)num / 32767);
		}
		submesh->vertNorm.push_back(norm);

		memcpy((char*)&numW, data + i + 6, 2);
		submesh->vertNormW.push_back(numW);
	}
}

void parseUVBufferInMemory(int fileSize, float uoff, float voff, float uscale, float vscale)
{
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
		float fixu = ((float)u / 32767) * uoff + uscale;// *0.08;
		float fixv = ((float)v / 32767) * -voff + vscale;// *0.08;
		//float fixu = ((float)u / 32767) * uscale + uoff;// *0.08;
		//float fixv = ((float)v / 32767) * -vscale + voff;// *0.08;
		uvert.push_back(fixu);
		uvert.push_back(fixv);
		submesh->vertUV.push_back(uvert);
	}
}

void parseVCBufferInMemory(int fileSize) {
	//literally just mont's vc code from MDE
	bool bAnyValidVC = false;
	for (int i = 0; i < fileSize; i += 4) {
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
}

#pragma endregion

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

	sarge.setDescription("Destiny 2 static model extractor by nblock.");
	sarge.setUsage("D2StaicExtractor");

	if (!sarge.parseArguments(argc, argv))
	{
		std::cerr << "Couldn't parse arguments..." << std::endl;
		show_usage();
		return 1;
	}

	std::string pkgsPath, outputPath, batchPkg;
	std::string modelHash = "";
	std::string texTypeIn = "";

	uint32_t sfhash32;
	float scale = 1;
	sarge.getFlag("pkgspath", pkgsPath);
	sarge.getFlag("outputpath", outputPath);
	sarge.getFlag("inputhash", modelHash);
	sarge.getFlag("batch", batchPkg);
	sarge.getFlag("filetype", texTypeIn);

	lodCulling = true;
	if (sarge.exists("lodcull"))
		lodCulling = false;

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
			std::cout<<"Not a valid static model.\n";
			exit(112);
		}
		outputPath += "/" + modelHash;
		fs::create_directories(outputPath);
		int fileSize;
		hash = modelHash;
		fileSize = getFile();
		memcpy((char*)&sfhash32, data + 0x8, 4);
		memcpy((void*)&scale, data + 0x3C, 4);
		uint32_t extOff = 0;
		hash = uint32ToHexStr(sfhash32);
		fileSize = getFile();
		uint32_t indexBuffer, vertexBuffer, uvBuffer, vcBuffer;
		memcpy((char*)&indexBuffer, data + fileSize - 0x14, 4);
		memcpy((char*)&vertexBuffer, data + fileSize - 0x10, 4);
		memcpy((char*)&uvBuffer, data + fileSize - 0xC, 4);
		memcpy((char*)&vcBuffer, data + fileSize - 0x8, 4);
		float uoff, voff, uscale, vscale;
		memcpy((void*)&uoff, data + 0x48, 4);
		memcpy((void*)&voff, data + 0x4C, 4);
		memcpy((void*)&uscale, data + 0x54, 4);
		memcpy((void*)&vscale, data + 0x58, 4);

		//This is very experimental and doesn't work yet.
		/*
		uint32_t val, amountLOD;
		bool bFound = false;
		extOff = fileSize -= 4;
		while (true)
		{
			memcpy((char*)&val, data + extOff, 4);
			if (val == 0x80806D37)
			{
				bFound = true;
				extOff -= 8;
				break;
			}
			extOff -= 4;
		}
		memcpy((char*)&amountLOD, data + extOff, 4);
		extOff += 0x10;
		int j = 0;
		uint32_t splitnext_off;
		uint32_t splitnext_count;
		for (int o = extOff; o < extOff + (amountLOD * 12); o += 12) {
			LODSplit split;
			memcpy((char*)&split.off, data + o, 4);
			memcpy((char*)&split.count, data + o + 4, 4);
			memcpy((char*)&splitnext_off, data + o + 12, 4);
			memcpy((char*)&splitnext_count, data + o + 16, 4);
			std::cout << "CO: " + std::to_string(split.off) + " CC: " + std::to_string(split.count)
				<< "\nNO: " + std::to_string(splitnext_off) + " NC: " + std::to_string(splitnext_count) << "\n";
			if (j > 0 && submesh->lodsplit.size()) {
				//if (split.off != (submesh->lodsplit[j - 1].off + submesh->lodsplit[j - 1].count))
				//if (split.off == submesh->lodsplit[j - 1].off && split.count >)
				//	std::cout << "invalid LOD. SKIIIIIP\n";
				//else
				//	submesh->lodsplit.push_back(split);
				if (split.off == splitnext_off && split.count < splitnext_count) {
					std::cout << "\ninvalid LOD or not LOD0. SKIIIIIP\n\n";
					//o += 12;
				}
				else
					submesh->lodsplit.push_back(split);
			}
			else if (split.off == splitnext_off && split.count < splitnext_count) {
				std::cout << "\ninvalid LOD or not LOD0. SKIIIIIP\n\n";
				//o += 12;
			}
			else
				submesh->lodsplit.push_back(split);
			j += 1;	
		}
		*/

		delete[] data;
		hash = getReferenceFromHash(uint32ToHexStr(vertexBuffer), packagesPath);
		fileSize = getFile();
		if (scale == 0.000000)
			scale = 1;
		parseVertexBufferInMemory(fileSize, scale);
		parseVertexNormalsInMemory(fileSize);
		delete[] data;
		if (uvBuffer != 0xFFFFFFFF) {
			hash = getReferenceFromHash(uint32ToHexStr(uvBuffer), packagesPath);
			fileSize = getFile();
			parseUVBufferInMemory(fileSize, uoff, voff, uscale, vscale);
			delete[] data;
		}
		if (vcBuffer != 0xFFFFFFFF){
			hash = getReferenceFromHash(uint32ToHexStr(vcBuffer), packagesPath);
			fileSize = getFile();
			parseVCBufferInMemory(fileSize);
			delete[] data;
			addVertColSlots(submesh);
		}

		//uhhh this might work idk
		hash = uint32ToHexStr(indexBuffer);
		bool isu32 = false;
		getFile();
		memcpy((char*)&isu32, data + 1, 1);
		submesh->isU32 = isu32;

		delete[] data;
		hash = getReferenceFromHash(uint32ToHexStr(indexBuffer), packagesPath);
		fileSize = getFile();
		parseIndexBufferInMemory(submesh->vertPos.size(), fileSize);
		delete[] data;
		
		
		std::string fbxpath = outputPath + "/" + modelHash + ".fbx";
		submesh->name = modelHash;
		//submeshes.push_back(submesh);

		//for (int i = 0; i < submeshes.size(); i++) {
		//	Submesh* sub = submeshes[i];
		//	sub->name = modelHash + "_" + std::to_string(i);
		//	FbxNode* node = fbxModel->addSubmeshToFbx(sub);
		//	nodes.push_back(node);
		//}


		FbxNode* node = fbxModel->addSubmeshToFbx(submesh);
		nodes.push_back(node);

		if (nodes.size()) {
			for (auto& node : nodes) fbxModel->scene->GetRootNode()->AddChild(node);
			fbxModel->save(fbxpath, false);
		}


		submesh->faces.clear();
		submesh->facesu32.clear();
		submesh->vertPos.clear();
		submesh->vertUV.clear();
		submesh->vertNorm.clear();
		submesh->vertNormW.clear();

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
			uint32_t val;
			bool bFound = false;
			extOff = fileSize -= 4;
			while (true)
			{
				memcpy((char*)&val, data + extOff, 4);
				if (val == 0x80800014)
				{
					bFound = true;
					extOff -= 8;
					break;
				}
				extOff -= 4;
			}
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
				break;
			int fileSize = 0;
			hash.clear();
			pkgID.clear();
			hash = modelhash;
			getFile();
			memcpy((char*)&sfhash32, data + 0x8, 4);
			memcpy((void*)&scale, data + 0x3C, 4);
			hash.clear();
			pkgID.clear();
			hash = uint32ToHexStr(sfhash32);
			fileSize = getFile();
			uint32_t indexBuffer, vertexBuffer, uvBuffer, vcBuffer;
			memcpy((char*)&indexBuffer, data + fileSize - 0x14, 4);
			memcpy((char*)&vertexBuffer, data + fileSize - 0x10, 4);
			memcpy((char*)&uvBuffer, data + fileSize - 0xC, 4);
			memcpy((char*)&vcBuffer, data + fileSize - 0x8, 4);
			if (indexBuffer < 0x80800000 || vertexBuffer < 0x80800000 || uvBuffer < 0x80800000 || vcBuffer < 0x80800000)
				continue;
			float uoff, voff, uscale, vscale;
			memcpy((void*)&uoff, data + 0x48, 4);
			memcpy((void*)&voff, data + 0x4C, 4);
			memcpy((void*)&uscale, data + 0x54, 4);
			memcpy((void*)&vscale, data + 0x58, 4);
			//This is very experimental and doesn't work yet.
		/*
		uint32_t val, amountLOD;
		bool bFound = false;
		extOff = fileSize -= 4;
		while (true)
		{
			memcpy((char*)&val, data + extOff, 4);
			if (val == 0x80806D37)
			{
				bFound = true;
				extOff -= 8;
				break;
			}
			extOff -= 4;
		}
		memcpy((char*)&amountLOD, data + extOff, 4);
		extOff += 0x10;
		int j = 0;
		uint32_t splitnext_off;
		uint32_t splitnext_count;
		for (int o = extOff; o < extOff + (amountLOD * 12); o += 12) {
			LODSplit split;
			memcpy((char*)&split.off, data + o, 4);
			memcpy((char*)&split.count, data + o + 4, 4);
			memcpy((char*)&splitnext_off, data + o + 12, 4);
			memcpy((char*)&splitnext_count, data + o + 16, 4);
			std::cout << "CO: " + std::to_string(split.off) + " CC: " + std::to_string(split.count)
				<< "\nNO: " + std::to_string(splitnext_off) + " NC: " + std::to_string(splitnext_count) << "\n";
			if (j > 0 && submesh->lodsplit.size()) {
				//if (split.off != (submesh->lodsplit[j - 1].off + submesh->lodsplit[j - 1].count))
				//if (split.off == submesh->lodsplit[j - 1].off && split.count >)
				//	std::cout << "invalid LOD. SKIIIIIP\n";
				//else
				//	submesh->lodsplit.push_back(split);
				if (split.off == splitnext_off && split.count < splitnext_count) {
					std::cout << "\ninvalid LOD or not LOD0. SKIIIIIP\n\n";
					//o += 12;
				}
				else
					submesh->lodsplit.push_back(split);
			}
			else if (split.off == splitnext_off && split.count < splitnext_count) {
				std::cout << "\ninvalid LOD or not LOD0. SKIIIIIP\n\n";
				//o += 12;
			}
			else
				submesh->lodsplit.push_back(split);
			j += 1;
		}
		*/

			delete[] data;
			hash = getReferenceFromHash(uint32ToHexStr(vertexBuffer), packagesPath);
			fileSize = getFile();
			if (scale == 0.000000)
				scale = 1;
			parseVertexBufferInMemory(fileSize, scale);
			//parseVertexNormalsInMemory(fileSize);
			delete[] data;
			if (getReferenceFromHash(uint32ToHexStr(uvBuffer), packagesPath) != "ffffffff") {
				hash = getReferenceFromHash(uint32ToHexStr(uvBuffer), packagesPath);
				fileSize = getFile();
				parseUVBufferInMemory(fileSize, uoff, voff, uscale, vscale);
				delete[] data;
			}

			//uhhh this might work idk
			hash = uint32ToHexStr(indexBuffer);
			bool isu32 = false;
			getFile();
			memcpy((char*)&isu32, data + 1, 1);
			submesh->isU32 = isu32;

			delete[] data;
			hash = getReferenceFromHash(uint32ToHexStr(indexBuffer), packagesPath);
			fileSize = getFile();
			parseIndexBufferInMemory(submesh->vertPos.size(), fileSize);
			delete[] data;
			//for later
			/*
			hash = uint32ToHexStr(vcBuffer);
			if (hash != "FFFFFFFF"){
				fileSize = getFile();
				parseVCBufferInMemory(fileSize);
				delete[] data;
				addVertColSlots(submesh);
			}
			*/
			std::string fbxpath = outputPath + "/" + modelhash + ".fbx";

			submeshes.push_back(submesh);

			FbxNode* node = fbxModel->addSubmeshToFbx(submesh);
			nodes.push_back(node);

			if (nodes.size()) {
				for (auto& node : nodes) fbxModel->scene->GetRootNode()->AddChild(node);
				fbxModel->save(fbxpath, false);
			}


			submesh->faces.clear();
			submesh->facesu32.clear();
			submesh->vertPos.clear();
			submesh->vertUV.clear();

			std::cout << modelhash + ".fbx extracted.\n";
			submesh->name.clear();
			nodes.clear();
			fbxModel->scene->Clear();
			submeshes.clear();
		}
		std::cout << "Finished extracting " + hashes.size() << " models.\n";
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