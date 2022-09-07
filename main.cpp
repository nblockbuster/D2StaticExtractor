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
	sarge.setArgument("d", "bubhash", "bubble pointer hash", true);
	sarge.setDescription("Destiny 2 static model extractor by nblock.");
	sarge.setUsage("D2StaicExtractor");
	if (!sarge.parseArguments(argc, argv))
	{
		std::cerr << "Couldn't parse arguments..." << std::endl;
		show_usage();
		return 1;
	}
	std::string pkgsPath, outputPath, batchPkg;
	std::string BubbleHash = "";
	std::string modelHash = "";
	std::string texTypeIn = "";
	uint32_t sfhash32;
	float scale = 1;
	sarge.getFlag("pkgspath", pkgsPath);
	sarge.getFlag("outputpath", outputPath);
	sarge.getFlag("filetype", texTypeIn);
	sarge.getFlag("bubhash", BubbleHash);
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

	if (BubbleHash != "") 
	{
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
		/*
		* Input hashes must be of class 07878080 — i call them "Bubble Pointers".
		* These Bubble Pointer files point to ones of class 83988080. - my name for these personally are "DynStaMapPointers" (Dynamic/Static Map Pointers)
		* 
		* If the file has a reference to a static loadzone file, the data would just be 0-0—0—1 for rotation (quaternion) and 0-0-0-1 for pos/scale (XZY-Scale)
		* These files also have references to Dyn1 Header files as hash64 that can be put into MDE to get the object, for example a static reference will most likely be a plane
		* If the file has references to dynamics, the data layout would be the same as static ones, just with much more data.
		* 
		* For each 144 byte block in the 85988080 table, the layout would be like this:
		* 
		* 0x00 - Quaternionic rotation values (XYZW)
		* 0x10 - Position values (XYZ, Scale)
		* 0x20 - Hash64 Check (Should always be 0xFFFFFFFF)
		* 0x24 - Unk24
		* 0x28 - Unk28
		* 0x2C - Unk2C
		* 0x30 - Hash64 Value
		* 
		* And the rest is unknown to me!
		*/
		if (getReferenceFromHash(BubbleHash, packagesPath) != "07878080") { std::cout << "Not a valid bubble hash.\n"; exit(56); }
		hash = BubbleHash;
		int fileSize;
		fileSize = getFile();
		unsigned char* bubdata = data;
		uint32_t hash_count, val;
		std::vector<std::string> AllPointerHashes;
		std::set<std::string> ExistingPointerHashes;
		memcpy((char*)&hash_count, bubdata + 0x40, 4);
		for (int i = 0x50; i < 0x50 + hash_count * 4; i += 4)
		{
			memcpy((char*)&val, bubdata + i, 4);
			if (ExistingPointerHashes.find(uint32ToHexStr(val)) == ExistingPointerHashes.end())
			{
				AllPointerHashes.push_back(uint32ToHexStr(val));
				ExistingPointerHashes.insert(uint32ToHexStr(val));
			}
		}
		delete[] bubdata;
		std::vector<Vector4> Rotation;
		std::vector<Vector4> Translation;
		//std::set<std::string> ExistingDynamicHashes;
		std::vector<std::string> DynamicHashes;
		std::vector<std::string> StaticLZs;
		std::vector<FbxNode*> DynPointNodes;
		FbxModel* dynamic_point_model = new FbxModel();
		for (const auto& pbhash : AllPointerHashes)
		{
			if (getReferenceFromHash(pbhash, packagesPath) != "83988080") continue;
			std::cout << pbhash << '\n';
			uint32_t pos_or_empty_count;
			hash = pbhash;
			int fileSize;
			fileSize = getFile();
			if (fileSize < 144) continue;
			unsigned char* pbdata = data;
			memcpy((char*)&pos_or_empty_count, pbdata + 0x20, 4);
			uint32_t end = 0x30 + pos_or_empty_count * 0x90;
			for (int p = 0x30; p < end; p += 0x90)
			{
				float quatx, quaty, quatz, quatw, fx, fy, fz, lzscale;
				memcpy((char*)&quatx, pbdata + p, 4);
				memcpy((char*)&quaty, pbdata + p + 4, 4);
				memcpy((char*)&quatz, pbdata + p + 8, 4);
				memcpy((char*)&quatw, pbdata + p + 0xC, 4);
				memcpy((void*)&fx, pbdata + p + 0x10, 4);
				memcpy((void*)&fy, pbdata + p + 0x14, 4);
				memcpy((void*)&fz, pbdata + p + 0x18, 4);
				memcpy((void*)&lzscale, pbdata + p + 0x1C, 4);
				//These might not be right, i havent tested them yet.
				quaty *= -1;
				quatz *= -1;
				quatw *= -1;
				fx *= -1;
				Rotation.push_back({ quatx, quaty, quatz, quatw });
				Translation.push_back({ fx, fy, fz, lzscale });
				uint32_t h64Check;
				uint64_t hVal;
				std::string h64_hash;
				memcpy((char*)&h64Check, pbdata + p + 0x20, 4);
				if (uint32ToHexStr(h64Check) != "ffffffff") {
					std::cout << uint32ToHexStr(h64Check) << " Not a h64 value.\n";
					continue;
				}
				memcpy((char*)&hVal, pbdata + p + 0x30, 8);
				h64_hash = getHash64(hVal, hash64Table);
				std::string name = uint32ToHexStr(h64Check);
				DynamicHashes.push_back(h64_hash);
				//ExistingDynamicHashes.insert(h64_hash)
			}
			memcpy((char*)&val, pbdata + end + 0x4, 4);
			if (val != 0x80806CC9)
			{
				std::cout << "I dont know what to do here.\n";
				continue;
			}
			else
			{
				memcpy((char*)&val, pbdata + end + 0x10, 4);
				if (uint32ToHexStr(val) != "ffffffff") std::cout << "I also dont know what to do here.\n";
				else
				{
					memcpy((char*)&val, pbdata + end + 0x18, 4);
					std::string StaticLZPointerHash = uint32ToHexStr(val);
					std::cout << "---- " + StaticLZPointerHash << "\n";
					hash = StaticLZPointerHash;
					int fileSize;
					fileSize = getFile();
					memcpy((char*)&val, data + 0x8, 4);
					if (uint32ToHexStr(val).substr(uint32ToHexStr(val).length() - 2, 2) != "80") break;
					std::cout << "-------- " + uint32ToHexStr(val) << "\n";
					StaticLZs.push_back(uint32ToHexStr(val));
				}
			}

			for (int i = 0; i < DynamicHashes.size(); i++)
			{
				std::string name = DynamicHashes[i];
				FbxNode* node = FbxNode::Create(dynamic_point_model->manager, name.c_str());
				node->SetName(name.c_str());
				//double loadscale = lzscale * 100;
				double loadscale = Translation.at(i).w * 100;
				node->LclScaling.Set(FbxDouble3(loadscale));
				//node->LclTranslation.Set(FbxDouble3(fx * 100, fz * 100, fy * 100));
				node->LclTranslation.Set(FbxDouble3(Translation.at(i).x * 100, Translation.at(i).z * 100, Translation.at(i).y * 100));
				FbxQuaternion fq = FbxQuaternion(Rotation.at(i).x, Rotation.at(i).z, Rotation.at(i).y, Rotation.at(i).w);
				FbxVector4 fe2;
				fe2.SetXYZ(fq);
				node->LclRotation.Set(fe2);
				DynPointNodes.push_back(node);
			}
			for (const auto& dyn_node : DynPointNodes)
			{
				dynamic_point_model->scene->GetRootNode()->AddChild(dyn_node);
				std::string full_dyn_path = outputPath + "/" + BubbleHash + "/DynPoints/" + pbhash + "_Dyn_" + dyn_node->GetName();
				fs::create_directories(outputPath + "/" + BubbleHash + "/DynPoints/");
				dynamic_point_model->save(full_dyn_path, false);
			}
			DynPointNodes.clear();

		}
		
		dynamic_point_model->scene->Clear();
		dynamic_point_model->manager->Destroy();

		if (!StaticLZs.size()) { std::cout << "Static Loadzones Empty.\n"; exit(22); }
		for (const auto& lz_hash : StaticLZs)
		{
			bool result = ExportSingleLoadZone(lz_hash, outputPath + "/" + BubbleHash, false, sarge.exists("texex"), texTypeIn, hash64Table);
			if (!result) exit(25);
		}
	}
}

bool ExportSingleLoadZone(std::string lzHash, std::string outputPath, bool bl, bool bTextures, std::string texTypeIn, std::unordered_map<uint64_t, uint32_t> hash64Table)
{
	//outputPath += "/" + lzHash;

	hash = lzHash;
	int fileSize;
	fileSize = getFile();
	unsigned char* lzdata = data;


	std::vector<LookupTable> LUTS;
	std::vector<Vector4> Translation;
	std::vector<Vector4> Rotation;
	std::vector<std::string> TextureFileLines;
	uint32_t modelCount, val, tableOffset, posCount, posTableOff, posLookupTable, posLookupCount;
	uint16_t lookupVal;
	//WQ Values

	memcpy((char*)&posTableOff, lzdata + 0x48, 4);
	memcpy((char*)&modelCount, lzdata + 0x78, 4);
	memcpy((char*)&tableOffset, lzdata + 0x80, 4);
	memcpy((char*)&posLookupTable, lzdata + 0x90, 4);
	tableOffset += 0x90;
	posLookupTable += 0x90;
	posTableOff += 0x48;
	//BL Values
	if (bl)
	{
		memcpy((char*)&posTableOff, lzdata + 0x30, 4);
		memcpy((char*)&modelCount, lzdata + 0x58, 4);
		memcpy((char*)&tableOffset, lzdata + 0x60, 4);
		memcpy((char*)&posCount, lzdata + 0x38, 4);
		memcpy((char*)&posLookupTable, lzdata + 0x70, 4);
		posTableOff += 0x40;
		tableOffset += 0x70;
		posLookupTable += 0x70;
	}

	memcpy((char*)&posLookupCount, lzdata + posLookupTable, 4);
	posLookupTable += 0x10;

	memcpy((char*)&val, lzdata + posTableOff + 8, 4);
	if (val != 0x80806D40)
	{
		posTableOff += 0x20;
	}
	memcpy((char*)&posCount, lzdata + posTableOff, 4);
	posTableOff += 0x10;

	float quatx, quaty, quatz, quatw, fx, fy, fz, lzscale;
	int l = 0;
	for (int p = posTableOff; p < posTableOff + (posCount * 48); p += 0x20) {
		memcpy((char*)&quatx, lzdata + p, 4);
		memcpy((char*)&quaty, lzdata + p + 4, 4);
		memcpy((char*)&quatz, lzdata + p + 8, 4);
		memcpy((char*)&quatw, lzdata + p + 0xC, 4);
		p += 0x10;
		memcpy((void*)&fx, lzdata + p, 4);
		memcpy((void*)&fy, lzdata + p + 4, 4);
		memcpy((void*)&fz, lzdata + p + 8, 4);
		memcpy((void*)&lzscale, lzdata + p + 0xC, 4);
		quatx *= -1;
		quatz *= -1;
		quatw *= -1;
		fy *= -1;
		Rotation.push_back({ quatx, quaty, quatz, quatw });
		Translation.push_back({ fx, fz, fy, lzscale });
		l += 1;
	}
	for (int o = posLookupTable; o < posLookupTable + (posLookupCount * 8); o += 0x8)
	{
		LookupTable LUT;
		memcpy((char*)&LUT.EntryA, lzdata + o, 2);
		memcpy((char*)&LUT.EntryB, lzdata + o + 2, 2);
		memcpy((char*)&LUT.EntryC, lzdata + o + 4, 2);
		memcpy((char*)&LUT.EntryD, lzdata + o + 6, 2);
		LUTS.push_back(LUT);
	}
	int ab = 0;
	fs::create_directories(outputPath + "/" + lzHash);
	std::vector<std::string> modelHashes;
	for (int j = tableOffset; j < tableOffset + (modelCount * 4); j += 4)
	{
		memcpy((char*)&val, lzdata + j, 4);
		modelHashes.push_back(uint32ToHexStr(val));
	}
	delete[] lzdata;
	std::string mainModelHash;
	int m;
	for (int a = 0; a < modelHashes.size(); a++)
	{
		mainModelHash = modelHashes.at(a);
		std::cout << mainModelHash << "\n";
		m = LUTS[LUTS[a].EntryC].EntryB;
		std::cout << std::to_string(a) << "\n";
		int aaablr = Translation.size() - (m - 1);
		FbxNode* orignode = FbxNode::Create(fbxModel->manager, "");
		while (m < (LUTS[LUTS[a].EntryC].EntryB + LUTS[LUTS[a].EntryC].EntryA)) {
			if (orignode->GetMesh()) {
				std::string name = orignode->GetName();
				name = name.substr(0, name.find_last_of('_') + 1);
				name += std::to_string(ab);
				FbxNode* node = FbxNode::Create(fbxModel->manager, name.c_str());
				node->SetNodeAttribute(orignode->GetMesh());
				node->SetName(name.c_str());
				double loadscale = Translation.at(m).w * 100;
				node->LclScaling.Set(FbxDouble3(loadscale));
				node->LclTranslation.Set(FbxDouble3(Translation.at(m).x * 100, Translation.at(m).y * 100, Translation.at(m).z * 100));
				FbxQuaternion fq = FbxQuaternion(Rotation.at(m).x, Rotation.at(m).y, Rotation.at(m).z, Rotation.at(m).w);
				FbxVector4 fe2;
				fe2.SetXYZ(fq);
				node->LclRotation.Set(fe2);
				std::cout << "(Instanced) " + name << "\n";
				std::cout << "(Instanced) X: " + to_str(Translation.at(m).x * 100) + " Y: " + to_str(Translation.at(m).y * 100) + " Z: " + to_str(Translation.at(m).z * 100) + " Scale: " + to_str(Translation.at(m).w * 100) << "\n";
				std::cout << "(Instanced) X Rot: " + to_str(Rotation.at(m).x) + " Y Rot: " + to_str(Rotation.at(m).y) + " Z Rot: " + to_str(Rotation.at(m).z) + " W Rot: " + to_str(Rotation.at(m).w) << "\n";
				nodes.push_back(node);
				ab++;
				m++;
				submeshes.clear();
				continue;
			}
			Vector4 pos_off;
			int fileSize;
			hash = mainModelHash;
			if (getReferenceFromHash(hash, packagesPath) != "446d8080") break;
			fileSize = getFile();
			
			uint32_t sfhash32;
			memcpy((char*)&sfhash32, data + 0x8, 4);

			float x_off, y_off, z_off, scale;
			memcpy((char*)&x_off, data + 0x50, 4);
			memcpy((char*)&y_off, data + 0x54, 4);
			memcpy((char*)&z_off, data + 0x58, 4);
			memcpy((char*)&scale, data + 0x5C, 4);
			//needs adjustment!!
			pos_off.x = x_off;
			pos_off.y = y_off;
			pos_off.z = z_off;
			pos_off.w = scale;

			uint32_t extOff = 0;
			delete[] data;
			hash = uint32ToHexStr(sfhash32);
			if (getReferenceFromHash(hash, packagesPath) != "306d8080") break;
			fileSize = getFile();
			uint32_t val;
			memcpy((char*)&val, data + fileSize - 0x14, 4);
			if (uint32ToHexStr(val).substr(uint32ToHexStr(val).length() - 2, 2) != "80") break;

			IndexBufferHeader* indexBufHeader = new IndexBufferHeader(uint32ToHexStr(val), packagesPath);

			memcpy((char*)&val, data + fileSize - 0x10, 4);
			if (uint32ToHexStr(val).substr(uint32ToHexStr(val).length() - 2, 2) != "80") break;

			VertexBuffer* vertBuf = new VertexBuffer(getReferenceFromHash(uint32ToHexStr(val), packagesPath), packagesPath, submesh);

			memcpy((char*)&val, data + fileSize - 0xC, 4);

			if (val != 0xFFFFFFFF)
			{
				if (uint32ToHexStr(val).substr(uint32ToHexStr(val).length() - 2, 2) != "80") break;
				VertexBuffer* vertUVBuf = new VertexBuffer(getReferenceFromHash(uint32ToHexStr(val), packagesPath), packagesPath, submesh);
				vertUVBuf->parseVertUV();
			}

			memcpy((char*)&val, data + fileSize - 0x8, 4);

			if (val != 0xFFFFFFFF)
			{
				if (uint32ToHexStr(val).substr(uint32ToHexStr(val).length() - 2, 2) != "80") break;
				VertexBuffer* vertVCBuf = new VertexBuffer(getReferenceFromHash(uint32ToHexStr(val), packagesPath), packagesPath, submesh);
				vertVCBuf->parseVertexColor();
				addVertColSlots(submesh);
			}
			float uoff, voff, uscale, vscale;
			memcpy((char*)&uscale, data + 0x44, 4);
			memcpy((char*)&vscale, data + 0x48, 4);
			memcpy((char*)&uoff, data + 0x4c, 4);
			memcpy((char*)&voff, data + 0x50, 4);
			submesh->scales.push_back(uscale);
			submesh->scales.push_back(vscale);
			submesh->offset.push_back(uoff);
			submesh->offset.push_back(voff);

			vertBuf->parseVertPos();
			indexBufHeader->indexBuffer->getFaces(submesh);
			vertBuf->parseVertNorm();
			transformPos(scale, pos_off);
			transformUV();

			if (!submesh->faces.size() || !submesh->vertPos.size())
				break;

			if (lodCulling)
			{
				uint32_t amountLOD, lodTableOffset;
				memcpy((char*)&lodTableOffset, data + 0x20, 4);
				lodTableOffset += 0x20;
				memcpy((char*)&amountLOD, data + lodTableOffset, 4);
				lodTableOffset += 0x10;
				for (int o = lodTableOffset; o < lodTableOffset + (amountLOD * 12); o += 12) {
					LODSplit split;
					memcpy((char*)&split.IndexOffset, data + o, 4);
					memcpy((char*)&split.IndexCount, data + o + 4, 4);
					memcpy((char*)&split.DetailLevel, data + o + 0xA, 1);
					if (split.DetailLevel == 1)
						submesh->lodsplit.push_back(split);
					else
						continue;
				}

				delete[] data;


				for (const auto& split : submesh->lodsplit) {
					if (!submesh->faces.size() || !submesh->vertPos.size())
						break;
					Submesh* newsub = new Submesh();
					for (int i = split.IndexOffset; i < split.IndexOffset + split.IndexCount; i++) {
						newsub->faces.push_back(submesh->faces[i / 3]);
					}
					std::set<int> dsort;
					for (auto& face : newsub->faces)
					{
						for (auto& f : face)
						{
							dsort.insert(f);
						}
					}
					if (!dsort.size()) {
						dsort.clear();
						continue;
					}
					std::unordered_map<int, int> d;
					int i = 0;
					for (auto& val : dsort)
					{
						d[val] = i;
						i++;
					}
					for (auto& face : newsub->faces)
					{
						for (auto& f : face)
						{
							f = d[f];
						}
					}
					newsub->vertPos = trimVertsData(submesh->vertPos, dsort, false);
					if (submesh->vertUV.size()) newsub->vertUV = trimVertsData(submesh->vertUV, dsort, false);
					if (submesh->vertNorm.size()) newsub->vertNorm = trimVertsData(submesh->vertNorm, dsort, false);
					if (submesh->vertCol.size()) newsub->vertCol = trimVertsData(submesh->vertCol, dsort, true);
					if (submesh->vertColSlots.size()) newsub->vertColSlots = submesh->vertColSlots;
					submeshes.push_back(newsub);
					dsort.clear();
					d.clear();
				}
			}
			else {
				delete[] data;
				submeshes.push_back(submesh);
			}

			for (int p = 0; p < submeshes.size(); p++)
			{
				submeshes[p]->name = mainModelHash + "_" + std::to_string(p) + "_" + std::to_string(ab);
				orignode = fbxModel->addSubmeshToFbx(submeshes[p]);
				orignode->SetName(submeshes[p]->name.c_str());
				double loadscale = Translation.at(m).w * 100;
				orignode->LclScaling.Set(FbxDouble3(loadscale));
				orignode->LclTranslation.Set(FbxDouble3(Translation.at(m).x * 100, Translation.at(m).y * 100, Translation.at(m).z * 100));
				FbxQuaternion fq = FbxQuaternion(Rotation.at(m).x, Rotation.at(m).y, Rotation.at(m).z, Rotation.at(m).w);
				FbxVector4 fe2;
				fe2.SetXYZ(fq);
				orignode->LclRotation.Set(fe2);
				std::cout << "X: " + to_str(Translation.at(m).x * 100) + " Y: " + to_str(Translation.at(m).y * 100) + " Z: " + to_str(Translation.at(m).z * 100) + " Scale: " + to_str(Translation.at(m).w * 100) << "\n";
				std::cout << "X Rot: " + to_str(Rotation.at(m).x) + " Y Rot: " + to_str(Rotation.at(m).y) + " Z Rot: " + to_str(Rotation.at(m).z) + " W Rot: " + to_str(Rotation.at(m).w) << "\n";
				nodes.push_back(orignode);
				/*
				if (bTextures)
				{
					submesh->exportTextures = true;
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

					submesh->exportFormat = texType;
					hash = mainModelHash;
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
							submesh->materials.push_back(mat);
						}
					}


					//for (auto mat : externalMaterials)
					//{
						MapExportTextures(submesh, outputPath, hash64Table);
						//mat->parseMaterial(hash64Table);
						//mat->exportTextures(fullSavePath, texType);
						//txtline = mainModelHash + "[" + mat->hash + "]: ";
						//for (auto t : mat->textures) {
							//if (t.second->hash == "") continue;
							//txtline += t.second->hash + ", ";
						//}
					//}
					//TextureFileLines.push_back(txtline);
				}
				*/
				submeshes[p]->clear();
				free(submeshes[p]);
			}
			submesh->clear();
			ab++;
			m++;
		}
		m = 0;
		ab = 0;
		submeshes.clear();
		submesh->clear();
	}


	std::string fbxpath = outputPath + "/" + lzHash + "/" + lzHash + ".fbx";
	std::string bubbleName = "STA_" + lzHash; //replace with bubble name!!
	FbxNode* master_map_empty = fbxModel->scene->GetRootNode()->Create(fbxModel->manager, bubbleName.c_str());
	master_map_empty->SetNodeAttribute(nullptr);
	//for (auto& node : nodes) { master_map_empty->AddChild(node); }// applyMaterial(fbxModel, submesh, node); }
	for (auto& node : nodes) { fbxModel->scene->GetRootNode()->AddChild(node); }
	//fbxModel->scene->GetRootNode()->AddChild(master_map_empty);
	fbxModel->save(fbxpath, false);
	nodes.clear();
	fbxModel->scene->Clear();
	return true;
}

std::vector<std::vector<float_t>> trimVertsData(std::vector<std::vector<float_t>> verts, std::set<int> dsort, bool bVertCol)
{
	std::vector<std::vector<float_t>> newVec;
	std::vector<float_t> zeroVec = { 0, 0, 0, 0 };
	for (auto& val : dsort)
	{
		if (bVertCol)
		{
			if (val >= verts.size()) newVec.push_back(zeroVec);
			else newVec.push_back(verts[val]);
		}
		else
		{
			newVec.push_back(verts[val]);
		}
	}
	return newVec;
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

void transformPos(float scale, Vector4 pos_off)
{
	for (auto& vert : submesh->vertPos)
	{
		vert[0] *= (pos_off.w + pos_off.x);
		vert[1] *= (pos_off.w + pos_off.z);
		vert[2] *= (pos_off.w + pos_off.y);
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
