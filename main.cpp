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
	logger = Logger(ELoggerLevels::Debug);

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
		logger.Error("Couldn't parse arguments...");
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
	sarge.getFlag("inputhash", modelHash);
	lodCulling = false;
	if (sarge.exists("lodcull"))
		lodCulling = true;
	submesh->lodCulling = lodCulling;
	if (pkgsPath == "" && outputPath == "")
		show_usage();
	packagesPath = pkgsPath;
#pragma endregion

	if (BubbleHash != "")
	{
		if (getReferenceFromHash(BubbleHash, packagesPath) != "548a8080")
		{
			logger.Error("Not a valid bubble hash.\n");
			exit(160);
		}

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
		std::vector<std::string> DynamicHashes;
		std::vector<std::string> StaticLZs;
		std::vector<FbxNode*> DynPointNodes;
		FbxModel* dynamic_point_model = new FbxModel();
		for (const auto& pbhash : AllPointerHashes)
		{
			if (getReferenceFromHash(pbhash, packagesPath) != "d6998080") continue;
			logger.Debug(pbhash);
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
				memcpy((char*)&quatx, pbdata + p + 0x10, 4);
				memcpy((char*)&quaty, pbdata + p + 0x14, 4);
				memcpy((char*)&quatz, pbdata + p + 0x18, 4);
				memcpy((char*)&quatw, pbdata + p + 0x1C, 4);
				memcpy((void*)&fx, pbdata + p + 0x20, 4);
				memcpy((void*)&fy, pbdata + p + 0x24, 4);
				memcpy((void*)&fz, pbdata + p + 0x28, 4);
				memcpy((void*)&lzscale, pbdata + p + 0x2C, 4);
				//--These might not be right, i havent tested them yet.--
				//These seem right.
				quaty *= -1;
				quatz *= -1;
				quatw *= -1;
				fx *= -1;
				Rotation.push_back({ quatx, quaty, quatz, quatw });
				Translation.push_back({ fx, fy, fz, lzscale });
				uint32_t dyn_hash;
				memcpy((char*)&dyn_hash, pbdata + p, 4);
				DynamicHashes.push_back(uint32ToHexStr(dyn_hash));
			}
			memcpy((char*)&val, pbdata + end + 0x4, 4);
			if (val != 0x808071B3)
			{
				logger.Debug("I dont know what to do here.");
				continue;
			}
			else
			{
				memcpy((char*)&val, pbdata + end + 0x18, 4);
				if (val < 0x80800000)
					logger.Debug("I also dont know what to do here."); //just incase the hash just doesnt
				else
				{
					memcpy((char*)&val, pbdata + end + 0x18, 4);
					std::string StaticLZPointerHash = uint32ToHexStr(val);
					logger.Debug("---- " + StaticLZPointerHash);
					hash = StaticLZPointerHash;
					int fileSize;
					fileSize = getFile();
					memcpy((char*)&val, data + 0x8, 4);
					if (uint32ToHexStr(val).substr(uint32ToHexStr(val).length() - 2, 2) != "80") break;
					logger.Debug("-------- " + uint32ToHexStr(val));
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

		if (!StaticLZs.size()) { logger.Error("Static Loadzones Empty."); exit(22); }
		for (const auto& lz_hash : StaticLZs)
		{
			bool result = ExportSingleLoadZone(lz_hash, outputPath + "/" + BubbleHash, false, sarge.exists("texex"), texTypeIn);
			if (!result) exit(25);
		}
	}
}

//ignore this, only valid for postbl

bool ExportSingleLoadZone(std::string lzHash, std::string outputPath, bool bl, bool bTextures, std::string texTypeIn)
{
	hash = lzHash;
	int fileSize;
	fileSize = getFile();
	unsigned char* lzdata = data;

	uint32_t posTableOff, modelCount, tableOffset, posCount, posLookupTable, posLookupCount, val;

	memcpy((char*)&posTableOff, lzdata + 0x30, 4);
	memcpy((char*)&modelCount, lzdata + 0x58, 4);
	memcpy((char*)&tableOffset, lzdata + 0x60, 4);
	memcpy((char*)&posCount, lzdata + 0x38, 4);
	memcpy((char*)&posLookupTable, lzdata + 0x70, 4);
	posTableOff += 0x50;
	tableOffset += 0x70;
	posLookupTable += 0x70;
	memcpy((char*)&posLookupCount, lzdata + posLookupTable, 4);
	posLookupTable += 0x10;

	memcpy((char*)&val, lzdata + posTableOff + 8, 4);
	/*if (val != 0x80806D40)
	{
		posTableOff += 0x20;
	}
	memcpy((char*)&posCount, lzdata + posTableOff, 4);
	posTableOff += 0x10;*/

	std::vector<LookupTable> LUTS;
	std::vector<Vector4> Rotation;
	std::vector<Vector4> Translation;

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
		quaty *= -1;
		quatz *= -1;
		quatw *= -1;
		fx *= -1;
		Rotation.push_back({ quatx, quaty, quatz, quatw });
		Translation.push_back({ fx, fy, fz, lzscale });
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
	//fs::create_directories(outputPath);
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
		logger.Info("Currently Extracting Model " + mainModelHash);
		m = LUTS[LUTS[a].EntryC].EntryB;
		logger.Debug(std::to_string(a));
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
				node->LclScaling.Set(FbxDouble3(Translation.at(m).w * 100));
				node->LclTranslation.Set(FbxDouble3(Translation.at(m).x * 100, Translation.at(m).z * 100, Translation.at(m).y * 100));
				FbxQuaternion fq = FbxQuaternion(Rotation.at(m).x, Rotation.at(m).z, Rotation.at(m).y, Rotation.at(m).w);
				FbxVector4 fe2;
				fe2.SetXYZ(fq);
				node->LclRotation.Set(fe2);
				logger.Debug("(Instanced) " + name);
				logger.Debug("(Instanced) X: " + to_str(Translation.at(m).x * 100) + " Y: " + to_str(Translation.at(m).z * 100) + " Z: " + to_str(Translation.at(m).y * 100) + " Scale: " + to_str(Translation.at(m).w * 100));
				logger.Debug("(Instanced) RAW X: " + to_str(Translation.at(m).x) + " RAW Y: " + to_str(Translation.at(m).z) + " RAW Z: " + to_str(Translation.at(m).y) + " RAW Scale: " + to_str(Translation.at(m).w));
				logger.Debug("(Instanced) X Rot: " + to_str(Rotation.at(m).x) + " Y Rot: " + to_str(Rotation.at(m).z) + " Z Rot: " + to_str(Rotation.at(m).y) + " W Rot: " + to_str(Rotation.at(m).w));
				nodes.push_back(node);
				ab++;
				m++;
				submeshes.clear();
				continue;
			}
			Vector4 pos_off;
			int fileSize;
			hash = mainModelHash;
			fileSize = getFile();
			unsigned char* mmdata = data;

			uint32_t sfHash;
			memcpy((char*)&sfHash, mmdata + 0x8, 4);

			//float ;
			//memcpy((void*)&scale, mmdata + 0x3C, 4);

			float x_off, y_off, z_off, scale;
			memcpy((char*)&x_off, data + 0x50, 4);
			memcpy((char*)&y_off, data + 0x54, 4);
			memcpy((char*)&z_off, data + 0x58, 4);
			memcpy((char*)&scale, data + 0x5C, 4);
			pos_off.x = x_off;
			pos_off.y = y_off;
			pos_off.z = z_off;
			pos_off.w = scale;
			//std::cout << to_str(x_off*scale) << " | " << to_str(y_off* scale) << " | " << to_str(z_off*scale) << " | " << to_str(scale) << "\n";

			float TXCoord_ScaleX, TXCoord_ScaleY, TXCoord_OffsetX, TXCoord_OffsetY;
			memcpy((char*)&TXCoord_OffsetX, data + 0x60, 4);
			memcpy((char*)&TXCoord_OffsetY, data + 0x64, 4);
			memcpy((char*)&TXCoord_ScaleX, data + 0x68, 4);
			memcpy((char*)&TXCoord_ScaleY, data + 0x6C, 4);
			submesh->offset.x = TXCoord_OffsetX;
			submesh->offset.y = TXCoord_OffsetY;
			submesh->scales.x = TXCoord_ScaleX;
			submesh->scales.y = TXCoord_ScaleY;

			delete[] data;

			hash = uint32ToHexStr(sfHash);
			fileSize = getFile();
			unsigned char* sbdata = data;

			uint32_t bufferOff;
			memcpy((char*)&bufferOff, data + 0x30, 4);
			bufferOff += 0x30;

			uint32_t bufferCount = 0;
			memcpy((char*)&bufferCount, sbdata + bufferOff, 4);
			bufferOff += 0x10;
			for (int i = bufferOff; i < bufferOff + bufferCount * 0x10; i += 0x10)
			{
				Submesh* sub = new Submesh;

				uint32_t val;
				memcpy((char*)&val, sbdata + i, 4);
				//if (uint32ToHexStr(val).substr(uint32ToHexStr(val).length() - 2, 2) != "80" || "81") break;

				IndexBufferHeader* ibHeader = new IndexBufferHeader(uint32ToHexStr(val), packagesPath);

				memcpy((char*)&val, sbdata + i + 4, 4);
				//if (uint32ToHexStr(val).substr(uint32ToHexStr(val).length() - 2, 2) != "80" || "81") break;

				VertexBufferHeader* vbHeader = new VertexBufferHeader(uint32ToHexStr(val), packagesPath, sub);

				memcpy((char*)&val, sbdata + i + 12, 4);
				sub->someValue = val;


				vbHeader->vertexBuffer->parseVertPos();
				ibHeader->indexBuffer->getFaces(sub);

				transformPos(sub, pos_off);

				if (!sub->vertPos.size() || !sub->faces.size())
					continue;

				submeshes.push_back(sub);
			}

			if (lodCulling)
			{
				if (!submeshes.size())
					break;
				uint32_t amountLOD, lodTableOffset;
				memcpy((char*)&lodTableOffset, sbdata + 0x20, 4);
				lodTableOffset += 0x20;
				memcpy((char*)&amountLOD, sbdata + lodTableOffset, 4);
				lodTableOffset += 0x10;
				submesh->lodsplit.clear();
				for (int o = lodTableOffset; o < lodTableOffset + (amountLOD * 12); o += 12) {
					LODSplit split;
					memcpy((char*)&split.IndexOffset, sbdata + o, 4);
					memcpy((char*)&split.IndexCount, sbdata + o + 4, 4);
					memcpy((char*)&split.submeshIndex, sbdata + o + 0x8, 1);
					memcpy((char*)&split.DetailLevel, sbdata + o + 0xA, 1);
					if (split.DetailLevel == 1 || split.DetailLevel == 0xA)
						submesh->lodsplit.push_back(split);
					else
						continue;
				}
				std::vector<Submesh*> temp_submeshes;
				for (const auto& split : submesh->lodsplit) {
					Submesh* cursub = submeshes[split.submeshIndex];
					if (!cursub->faces.size() || !cursub->vertPos.size())
						continue;
					Submesh* newsub = new Submesh;
					//if (split.IndexOffset + split.IndexCount > cursub->faces.size())
						//continue;
					for (int i = split.IndexOffset; i < split.IndexOffset + split.IndexCount; i++) {
						newsub->faces.push_back(cursub->faces[i / 3]);
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
					newsub->vertPos = trimVertsData(cursub->vertPos, dsort, false);
					if (cursub->vertUV.size()) newsub->vertUV = trimVertsData(cursub->vertUV, dsort, false);
					if (cursub->vertNorm.size()) newsub->vertNorm = trimVertsData(cursub->vertNorm, dsort, false);
					if (cursub->vertCol.size()) newsub->vertCol = trimVertsData(cursub->vertCol, dsort, true);
					if (cursub->vertColSlots.size()) newsub->vertColSlots = cursub->vertColSlots;
					temp_submeshes.push_back(newsub);
					dsort.clear();
					d.clear();
				}
				submeshes.clear();
				submeshes = temp_submeshes;
			}

			for (int p = 0; p < submeshes.size(); p++)
			{
				submeshes[p]->name = mainModelHash + "_" + std::to_string(p);
				orignode = fbxModel->addSubmeshToFbx(submeshes[p]);
				orignode->SetName(submeshes[p]->name.c_str());
				orignode->LclScaling.Set(FbxDouble3(Translation.at(m).w * 100));
				orignode->LclTranslation.Set(FbxDouble3(Translation.at(m).x * 100, Translation.at(m).z * 100, Translation.at(m).y * 100));
				FbxQuaternion fq = FbxQuaternion(Rotation.at(m).x, Rotation.at(m).z, Rotation.at(m).y, Rotation.at(m).w);
				FbxVector4 fe2;
				fe2.SetXYZ(fq);
				orignode->LclRotation.Set(fe2);
				logger.Debug("TRANSLATION TABLE LOOKUP: " + std::to_string(m));
				logger.Debug("X: " + to_str(Translation.at(m).x * 100) + " Y: " + to_str(Translation.at(m).z * 100) + " Z: " + to_str(Translation.at(m).y * 100) + " Scale: " + to_str(Translation.at(m).w * 100));
				logger.Debug("X Rot: " + to_str(Rotation.at(m).x) + " Y Rot: " + to_str(Rotation.at(m).z) + " Z Rot: " + to_str(Rotation.at(m).y) + " W Rot: " + to_str(Rotation.at(m).w));
				nodes.push_back(orignode);
				submeshes[p]->clear();
				free(submeshes[p]);
			}
			ab++;
			m++;
		}
		submeshes.clear();
		submesh->clear();
		m = 0;
		ab = 0;
	}
	std::string fbxpath = outputPath + "/" + lzHash + ".fbx";
	std::string bubbleName = "STA_" + lzHash;
	FbxNode* master_map_empty = fbxModel->scene->GetRootNode()->Create(fbxModel->manager, bubbleName.c_str());
	master_map_empty->SetNodeAttribute(nullptr);
	for (auto& node : nodes) { master_map_empty->AddChild(node); }// applyMaterial(fbxModel, submesh, node); }
	fbxModel->scene->GetRootNode()->AddChild(master_map_empty);
	fbxModel->save(fbxpath, false);
	nodes.clear();
	fbxModel->scene->Clear();
	logger.Info("Extracted loadzone " + lzHash + " from bubble " + outputPath.substr(outputPath.rfind('/')+1));
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


void addVertColSlots(Submesh* sub){
	for (auto& w : sub->vertNormW)
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
		sub->vertColSlots.push_back(vc);
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

void transformUV(Submesh* sub)
{
	for (auto& vert : sub->vertUV)
	{
		vert[0] = vert[0] * sub->scales.x + sub->offset.x;
		vert[1] = vert[1] * -sub->scales.y + (1 - sub->offset.y);
	}
}

void transformPos(Submesh* sub, Vector4 pos_off)
{
	for (auto& vert : sub->vertPos)
	{
		vert[0] *= pos_off.w + pos_off.x;
		vert[1] *= pos_off.w + pos_off.z;
		vert[2] *= pos_off.w + pos_off.y;

		//for (int i = 0; i < 3; i++)
		//{
		//	vert[i] = vert[i] * scale;
		//}
		//modify vert position with pos_off values
		//vert[0] = vert[0] + pos_off.x;
		//vert[1] = vert[1] + pos_off.y;
		//vert[2] = vert[2] + pos_off.z;
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
	submesh->offset = { 0.0, 0.0 };
	submesh->scales = { 0.0, 0.0 };
}
