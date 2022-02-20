#include "main.h"
//this is horrible, messy code.
//read https://github.com/nblockbuster/D2StaticDocs on the basics of statics

namespace fs = std::filesystem;

#pragma region Parsers

bool parseIndexBufferInMemory(int vertexCount, bool isU32Indexes, int fileSize)
{
	int e = 0;
	int b = 0;
	int l = 0;
	int16_t face;
	int n = vertexCount;
	for (int i = 0; i < fileSize; i += 0x6) {
		std::vector<int16_t> intFace;
		intFace.reserve(3);
		for (int j = 0; j < 3; j++) {
			memcpy((char*)&face, data + i + j * 2, 2);
			intFace.push_back(face);
		}
		if (face > vertexCount)
			break;
		submesh->faces.push_back(intFace);
		//im not proud of this...
		//it also doesnt work half the time :]
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
		}
	}

	//leftover obj exporting code for confirming identical between obj -> fbx
	
	//std::string fstr;
	//std::vector<std::string> fvstr;
	//for (int k = 0; k < submesh->faces.size(); k++) {
	//	std::vector<int16_t> face = submesh->faces[k];
	//	fstr = "f " + std::to_string(face[0] + 1) + "/" + std::to_string(face[0] + 1) + " " + std::to_string(face[1] + 1) + "/" + std::to_string(face[1] + 1) + " " + std::to_string(face[2] + 1) + "/" + std::to_string(face[2] + 1);
	//	fvstr.push_back(fstr);
	//}
	//std::ofstream idx;
	//idx.open("test.obj", std::ios_base::app);
	//for (const auto& f : fvstr) idx << f << "\n";
	//idx.close();
	//fvstr.clear();
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

	//leftover obj exporting code for confirming identical between obj -> fbx

	//std::string xyzw;
	//std::vector<std::string> vstr;
	//for (int i = 0; i < submesh->vertPos.size(); i++) {
	//	std::vector<float_t> v = submesh->vertPos[i];
	//	xyzw = "v " + to_str(-v[0]) + " " + to_str(v[2]) + " " + to_str(v[1]);
	//	vstr.push_back(xyzw);
	//}
	//std::ofstream vx;
	//vx.open("test.obj", std::ios_base::app);
	//for (const auto& v : vstr) vx << v << "\n";
	//vx.close();
	//vstr.clear();
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

void parseUVBufferInMemory(int fileSize, float utrans, float vtrans, float uoff, float voff)
{
	int16_t u, v;
	for (int i = 0; i < fileSize; i += 0x4) {
		std::vector<float> uvert;
		uvert.reserve(2);
		memcpy((char*)&u, data + i, 2);
		memcpy((char*)&v, data + i + 2, 2);
		//i need to fix this because it isnt right
		float fixu = ((float)u / 32767) * uoff + utrans;
		float fixv = ((float)v / 32767) * -voff + vtrans;
		uvert.push_back(fixu);
		uvert.push_back(fixv);
		submesh->vertUV.push_back(uvert);
	}

	//leftover obj exporting code for confirming identical between obj -> fbx

	//std::string vt;
	//std::vector<std::string> vtstr;
	//for (int k = 0; k < submesh->vertUV.size(); k++) {
	//	std::vector<float_t> uv = submesh->vertUV[k];
	//	vt = "vt " + to_str(uv[0]) + " " + to_str(uv[1]);
	//	vtstr.push_back(vt);
	//}
	//std::ofstream vtx;
	//vtx.open("test.obj", std::ios_base::app);
	//for (const auto& v : vtstr) vtx << v << "\n";
	//vtx.close();
	//vtstr.clear();
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
	std::cerr << "Usage: D2StaticExtractor -p [packages path] -o [output path] -i [input hash] -b [package ID] -l -t"
		<< std::endl;
	std::cerr << "-i extracts a single main model hash\n";
	std::cerr << "-b extracts all the static models available for that package ID. (-t is ignored)\n";
	std::cerr << "-l enables hacky lod culling\n";
	std::cerr << "-t extracts textures for given model\n";
}

int main(int argc, char* argv[])
{
	#pragma region Sarge stuff

	Sarge sarge;
	sarge.setArgument("p", "pkgspath", "pkgs path", true);
	sarge.setArgument("o", "outputpath", "output path", true);
	sarge.setArgument("i", "inputhash", "hash of mainmodel file", true);
	sarge.setArgument("b", "batch", "batch with pkg ID", true);
	sarge.setArgument("l", "lodcull", "disable lod culling", false);
	sarge.setArgument("t", "texex", "texture extraction", false);

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

	uint32_t sfhash32;
	float scale = 1;
	sarge.getFlag("pkgspath", pkgsPath);
	sarge.getFlag("outputpath", outputPath);
	sarge.getFlag("inputhash", modelHash);
	sarge.getFlag("batch", batchPkg);

	lodCulling = sarge.exists("lodcull");

	if (pkgsPath == "" && outputPath == "")
		show_usage();

	packagesPath = pkgsPath;
	#pragma endregion

	#pragma region H64 generation for textures

	std::unordered_map<uint64_t, uint32_t> hash64Table;
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
	#pragma endregion

	if (sarge.exists("lodcull"))
		lodCulling = false;

	if (modelHash != "") {
		std::string outputName;
		outputPath += "/" + modelHash;
		outputName = outputPath + "/" + modelHash + ".obj";
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
		float utrans, vtrans, uoff, voff;
		memcpy((void*)&utrans, data + 0x58, 4);
		memcpy((void*)&vtrans, data + 0x5C, 4);
		memcpy((void*)&uoff, data + 0x60, 4);
		memcpy((void*)&voff, data + 0x64, 4);
		delete[] data;
		hash = uint32ToHexStr(vertexBuffer);
		fileSize = getFile();
		if (scale == 0.000000)
			scale = 1;
		parseVertexBufferInMemory(fileSize, scale);
		//parseVertexNormalsInMemory(fileSize);
		delete[] data;
		hash = getReferenceFromHash(uint32ToHexStr(uvBuffer), packagesPath);
		fileSize = getFile();
		parseUVBufferInMemory(fileSize, utrans, vtrans, uoff, voff);
		delete[] data;
		hash = uint32ToHexStr(indexBuffer);
		fileSize = getFile();
		parseIndexBufferInMemory(submesh->vertPos.size(), false, fileSize);
		delete[] data;
		/*
		hash = uint32ToHexStr(vcBuffer);
		if (hash != "FFFFFFFF"){
			fileSize = getFile();
			parseVCBufferInMemory(fileSize);
			delete[] data;
			addVertColSlots(submesh);
		}
		*/
		std::string fbxpath = outputPath + "/" + modelHash + ".fbx";

		submesh->name = modelHash.c_str();
		FbxNode* node = fbxModel->addSubmeshToFbx(submesh);
		nodes.push_back(node);
		for (auto& node : nodes) fbxModel->scene->GetRootNode()->AddChild(node);

		fbxModel->save(fbxpath, false);
		fbxModel->manager->Destroy();

		submesh->faces.clear();
		submesh->vertPos.clear();
		submesh->vertUV.clear();
		
		//parse MATERIALS ?
		//(ill probably fold this into a smaller function that doesnt require rawtex and a temp bin, like MDE)
		if (sarge.exists("texex")) {
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
			std::vector<std::string> externalMaterials;
			uint32_t extCount;
			memcpy((char*)&extCount, data + extOff, 4);
			extOff += 0x10;
			std::set<std::uint32_t> existingMats;
			for (int j = extOff; j < extOff + extCount * 4; j += 4)
			{
				memcpy((char*)&val, data + j, 4);
				if (existingMats.find(val) == existingMats.end())
				{
					externalMaterials.push_back(uint32ToHexStr(val));
					existingMats.insert(val);
				}
			}
			for (auto hash : externalMaterials) {
				unsigned char* matdata = nullptr;
				int matFileSize;
				Package matpkg(getPkgID(hash), packagesPath);
				matdata = matpkg.getEntryData(hash, matFileSize);
				uint32_t textureCount;
				uint32_t textureOffset;
				memcpy((char*)&textureCount, matdata + 0x2A0, 4);
				if (textureCount == 0) continue;
				memcpy((char*)&textureOffset, matdata + 0x2A8, 4);
				textureOffset += 0x2A8 + 0x10;
				uint64_t h64Val;
				for (int v = textureOffset; v < textureOffset + textureCount * 0x18; v += 0x18) {
					uint8_t textureIndex;
					memcpy((char*)&textureIndex, matdata + v, 1);
					memcpy((char*)&val, matdata + v + 8, 4);
					std::string h64Check = uint32ToHexStr(val);
					if (h64Check == "ffffffff") {
						memcpy((char*)&h64Val, matdata + v + 0x10, 8);
						if (h64Val == 0) continue;
						std::string textureHash = getHash64(h64Val, hash64Table);
						if (textureHash != "ffffffff") {
							unsigned char* headdat = nullptr;
							Package headpkg(getPkgID(textureHash), packagesPath);
							int headsize;
							headdat = headpkg.getEntryData(textureHash, headsize);
							memcpy((char*)&textureFormat, headdat + 0x4, 4);
							memcpy((char*)&width, headdat + 0x22, 2);
							memcpy((char*)&height, headdat + 0x24, 2);
							memcpy((char*)&val, headdat + 0x3C, 4);
							largeHash = uint32ToHexStr(val);
							delete[] headdat;
							std::string finalHash = "";
							if (largeHash != "ffffffff" && largeHash != "")
								finalHash = largeHash;
							else
								finalHash = headpkg.getEntryReference(textureHash);

							Package datpkg(getPkgID(finalHash), packagesPath);
							unsigned char* datdat = nullptr;
							int datsize;
							datdat = datpkg.getEntryData(finalHash, datsize);
							FILE* dootfile;
							fs::create_directory("expath_temp");
							std::string tempo = "expath_temp\\" + textureHash + ".bin";
							fopen_s(&dootfile, tempo.c_str(), "wb");
							fwrite(datdat, datsize, 1, dootfile);
							fclose(dootfile);
							std::string rawtex = "rawtexcmd.exe \"" + tempo + "\" " + std::to_string(textureFormat) + " 0 " + std::to_string(width) + " " + std::to_string(height);
							system(rawtex.c_str());
							fs::create_directories(outputPath + "/textures");

							//TGA support
							
							//std::string dxgiFormat = DXGI_FORMAT[textureFormat];
							//std::string texconv = "texconv.exe \"expath_temp\\" + textureHash + ".dds\" -nologo -o \"" + outputPath + "\\textures" + "\" -y -ft TGA -f " + dxgiFormat;
							//system(texconv.c_str());

							fs::rename("expath_temp/" + textureHash + ".dds", outputPath + "/textures/" + textureHash + ".dds");
							fs::remove_all("expath_temp");
						}
					}
					else if (val > 0x80000000) {
						std::string textureHash = h64Check;
						unsigned char* headdat = nullptr;
						Package headpkg(getPkgID(textureHash), packagesPath);
						int headsize;
						headdat = headpkg.getEntryData(textureHash, headsize);
						memcpy((char*)&textureFormat, headdat + 0x4, 4);
						memcpy((char*)&width, headdat + 0x22, 2);
						memcpy((char*)&height, headdat + 0x24, 2);
						memcpy((char*)&val, headdat + 0x3C, 4);
						largeHash = uint32ToHexStr(val);
						delete[] headdat;
						std::string finalHash = "";

						if (largeHash != "ffffffff" && largeHash != "")
							finalHash = largeHash;
						else
							finalHash = headpkg.getEntryReference(textureHash);

						Package datpkg(getPkgID(finalHash), packagesPath);
						unsigned char* datdat = nullptr;
						int datsize;
						datdat = datpkg.getEntryData(finalHash, datsize);
						FILE* dootfile;
						fs::create_directory("expath_temp");
						std::string tempo = "expath_temp\\" + textureHash + ".bin";
						fopen_s(&dootfile, tempo.c_str(), "wb");
						fwrite(datdat, datsize, 1, dootfile);
						fclose(dootfile);
						std::string rawtex = "rawtexcmd.exe \"" + tempo + "\" " + std::to_string(textureFormat) + " 0 " + std::to_string(width) + " " + std::to_string(height);
						system(rawtex.c_str());
						fs::create_directories(outputPath + "/textures");

						//TGA support

						//std::string dxgiFormat = DXGI_FORMAT[textureFormat];
						//std::string texconv = "texconv.exe \"expath_temp\\" + textureHash + ".dds\" -nologo -o \"" + outputPath + "\\textures" + "\" -y -ft TGA -f " + dxgiFormat;
						//system(texconv.c_str());

						fs::rename("expath_temp/" + textureHash + ".dds", outputPath + "/textures/" + textureHash + ".dds");
						fs::remove_all("expath_temp");
					}
				}
			}
			fs::remove("expath_temp");
		}

		std::cout << modelHash + ".fbx extracted.\n";
	}

	else if (batchPkg != "") {
		std::string outputName;
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
			outputName = outputPath + modelhash + ".obj";
			if (fs::exists(outputName) || fs::exists(outputPath + modelhash + ".fbx")) {
				printf("Output file already exists.\n");
				continue;
			}
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
			if (indexBuffer < 0x80800000 || vertexBuffer < 0x80800000 || uvBuffer < 0x80800000 || vcBuffer < 0x80800000) {
				std::cout << std::to_string(indexBuffer) + " " + std::to_string(vertexBuffer) + " " + std::to_string(uvBuffer) + " " + std::to_string(vcBuffer) << std::endl;
				continue;
			}
			float utrans, vtrans, uoff, voff;
			memcpy((void*)&utrans, data + 0x58, 4);
			memcpy((void*)&vtrans, data + 0x5C, 4);
			memcpy((void*)&uoff, data + 0x60, 4);
			memcpy((void*)&voff, data + 0x64, 4);
			delete[] data;
			hash.clear();
			pkgID.clear();
			hash = uint32ToHexStr(vertexBuffer);
			fileSize = getFile();
			if (scale == 0.000000)
				scale = 1;
			parseVertexBufferInMemory(fileSize, scale);
			delete[] data;
			hash.clear();
			pkgID.clear();
			if (uvBuffer == 0xFFFFFFFF)
				continue;
			else {
				hash = getReferenceFromHash(uint32ToHexStr(uvBuffer), packagesPath);
				fileSize = getFile();
				parseUVBufferInMemory(fileSize, utrans, vtrans, uoff, voff);
				delete[] data;
				hash.clear();
			}
			hash = uint32ToHexStr(indexBuffer);
			fileSize = getFile();
			parseIndexBufferInMemory(submesh->vertPos.size(), false, fileSize);
			delete[] data;

			std::string fbxpath = outputPath + "/" + modelhash + ".fbx";

			submesh->name = modelhash.c_str();
			FbxNode* node = fbxModel->addSubmeshToFbx(submesh);
			nodes.push_back(node);
			for (auto& node : nodes) fbxModel->scene->GetRootNode()->AddChild(node);

			fbxModel->save(fbxpath, false);

			submesh->faces.clear();
			submesh->vertPos.clear();
			submesh->vertUV.clear();
			submesh->vertNorm.clear();
			submesh->vertNormW.clear();
			submesh->vertCol.clear();
			submesh->vertColSlots.clear();

			std::cout << modelhash + ".fbx extracted.\n";
			submesh->name.clear();
			nodes.clear();
			fbxModel->scene->Clear();
		}
		std::cout << "Finished extracting " + hashes.size() << " models.\n";
		fbxModel->manager->Destroy();
	}

}

int getFile()
{
	if (pkgID == "")
		pkgID = getPkgID(hash);
	Package pkg(pkgID, packagesPath);
	int fileSize;
	data = pkg.getEntryData(hash, fileSize);
	if (data == nullptr || sizeof(data) == 0) return 0;
	return fileSize;
}

void addVertColSlots(Submesh* submesh) {
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