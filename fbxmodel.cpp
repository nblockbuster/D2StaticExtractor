#include "fbxmodel.h"

void FbxModel::addUV(FbxMesh* mesh, Submesh* submesh, FbxLayer* layer)
{
	FbxLayerElementUV* uvLayerElement = FbxLayerElementUV::Create(mesh, "uv");
	uvLayerElement->SetMappingMode(FbxLayerElement::eByControlPoint);
	uvLayerElement->SetReferenceMode(FbxLayerElement::eDirect);
	for (auto& vert : submesh->vertUV)
	{
		uvLayerElement->GetDirectArray().Add(FbxVector2(vert[0], vert[1]));
	}
	layer->SetUVs(uvLayerElement, FbxLayerElement::eTextureDiffuse);
}

FbxMesh* FbxModel::createMesh(Submesh* submesh) {
	FbxMesh* mesh = FbxMesh::Create(manager, submesh->name.c_str());
	for (int i = 0; i < submesh->vertPos.size(); i++)
	{
		std::vector<float_t> v = submesh->vertPos[i];
		mesh->SetControlPointAt(FbxVector4(-v[0], v[2], v[1]), i);
	}
	if (submesh->isU32) {
		for (auto& uface : submesh->facesu32)
		{
			mesh->BeginPolygon();
			mesh->AddPolygon(uface[0]);
			mesh->AddPolygon(uface[1]);
			mesh->AddPolygon(uface[2]);
			mesh->EndPolygon();
		}
	}
	else {
		for (auto& face : submesh->faces)
		{
			mesh->BeginPolygon();
			mesh->AddPolygon(face[0]);
			mesh->AddPolygon(face[1]);
			mesh->AddPolygon(face[2]);
			mesh->EndPolygon();
		}
	}
	return mesh;
}

FbxNode* FbxModel::addSubmeshToFbx(Submesh* submesh)
{
	FbxMesh* mesh = createMesh(submesh);
	if (!mesh->GetLayer(0)) mesh->CreateLayer();
	FbxLayer* layer = mesh->GetLayer(0);
	FbxNode* node = FbxNode::Create(manager, submesh->name.c_str());
	node->SetNodeAttribute(mesh);
	node->LclScaling.Set(FbxDouble3(100, 100, 100));
	//if (submesh->vertNorm.size()) addNorm(mesh, submesh, layer);
	if (submesh->vertUV.size()) addUV(mesh, submesh, layer);
	if (submesh->vertCol.size()) addVC(mesh, submesh, layer);
	if (submesh->vertColSlots.size())
	{
		mesh->CreateLayer();
		FbxLayer* layerVC = mesh->GetLayer(1);
		addVCSlots(mesh, submesh, layerVC);
	}
	return node;
}

void FbxModel::addNorm(FbxMesh* mesh, Submesh* submesh, FbxLayer* layer)
{
	FbxLayerElementNormal* normLayerElement = FbxLayerElementNormal::Create(mesh, "norm");
	normLayerElement->SetMappingMode(FbxLayerElement::eByControlPoint);
	normLayerElement->SetReferenceMode(FbxLayerElement::eDirect);
	for (auto& vert : submesh->vertNorm)
	{
		normLayerElement->GetDirectArray().Add(FbxVector4(-vert[0], vert[2], vert[1]));
	}
	layer->SetNormals(normLayerElement);
}

void FbxModel::save(std::string savePath, bool ascii)
{
	if (not manager->GetIOSettings())
	{
		FbxIOSettings* ios = FbxIOSettings::Create(manager, IOSROOT);
		manager->SetIOSettings(ios);
	}

	manager->GetIOSettings()->SetBoolProp(EXP_FBX_MATERIAL, true);
	manager->GetIOSettings()->SetBoolProp(EXP_FBX_TEXTURE, true);
	manager->GetIOSettings()->SetBoolProp(EXP_FBX_EMBEDDED, true);
	manager->GetIOSettings()->SetBoolProp(EXP_FBX_SHAPE, true);
	manager->GetIOSettings()->SetBoolProp(EXP_FBX_GOBO, true);
	manager->GetIOSettings()->SetBoolProp(EXP_FBX_ANIMATION, true);
	manager->GetIOSettings()->SetBoolProp(EXP_FBX_GLOBAL_SETTINGS, true);
	int intAscii = -1;
	if (ascii) intAscii = 1;

	FbxExporter* exporter = FbxExporter::Create(manager, "");
	exporter->Initialize(savePath.c_str(), intAscii, manager->GetIOSettings());
	exporter->Export(scene);
	exporter->Destroy();
}

void FbxModel::addVC(FbxMesh* mesh, Submesh* submesh, FbxLayer* layer)
{
	FbxLayerElementVertexColor* vcLayerElement = FbxLayerElementVertexColor::Create(mesh, "vc");
	vcLayerElement->SetMappingMode(FbxLayerElement::eByControlPoint);
	vcLayerElement->SetReferenceMode(FbxLayerElement::eDirect);
	for (auto& vert : submesh->vertCol)
	{
		vcLayerElement->GetDirectArray().Add(FbxColor(vert[0], vert[1], vert[2], vert[3]));
	}
	layer->SetVertexColors(vcLayerElement);
}

void FbxModel::addVCSlots(FbxMesh* mesh, Submesh* submesh, FbxLayer* layer)
{
	FbxLayerElementVertexColor* vcLayerElement = FbxLayerElementVertexColor::Create(mesh, "slots");
	vcLayerElement->SetMappingMode(FbxLayerElement::eByControlPoint);
	vcLayerElement->SetReferenceMode(FbxLayerElement::eDirect);
	for (auto& vert : submesh->vertColSlots)
	{
		vcLayerElement->GetDirectArray().Add(FbxColor(vert[0], vert[1], vert[2], vert[3]));
	}
	layer->SetVertexColors(vcLayerElement);
}