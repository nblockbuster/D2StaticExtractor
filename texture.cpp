#include "texture.h"

void Texture::getHeader(std::string x)
{
    memcpy((char*)&textureFormat, data + 4, 2);
    memcpy((char*)&width, data + 0x22, 2);
    memcpy((char*)&height, data + 0x24, 2);
    memcpy((char*)&arraySize, data + 0x28, 2);
    uint32_t val;
    memcpy((char*)&val, data + 0x3C, 4);
    largeHash = uint32ToHexStr(val);
}

void Texture::tex2DDS(std::string fullSavePath)
{
    if (largeHash != "ffffffff" && largeHash != "")
        dataFile = new File(largeHash, packagesPath);
    else
        dataFile = new File(getReferenceFromHash(hash, packagesPath), packagesPath);
    writeTexture(fullSavePath);
}

void Texture::tex2Other(std::string fullSavePath, std::string saveFormat)
{
    tex2DDS(fullSavePath);
    std::string dxgiFormat;
    dxgiFormat = DXGI_FORMAT[textureFormat];
    std::string str = "texconv.exe \"" + fullSavePath + "\" -y -ft " + saveFormat + " -f " + dxgiFormat;
    printf(str.c_str());
    system(str.c_str());
    std::string newPath = fullSavePath.substr(0, fullSavePath.size() - 3) + saveFormat;
    std::ifstream f(newPath);
    if (f) std::remove(fullSavePath.c_str());
}

void Texture::writeTexture(std::string fullSavePath)
{
    bool bCompressed = false;
    if (70 < textureFormat < 99) bCompressed = true;

    DDSHeader dds;
    DXT10Header dxt;
    dds.MagicNumber = 542327876;
    dds.dwSize = 124;
    dds.dwFlags = (0x1 + 0x2 + 0x4 + 0x1000) + 0x8;
    dds.dwHeight = height;
    dds.dwWidth = width;
    dds.dwPitchOrLinearSize = 0;
    dds.dwDepth = 0;
    dds.dwMipMapCount = 0;
    dds.dwReserved1 = std::array<uint32_t, 11>();
    dds.dwPFSize = 32;
    dds.dwPFRGBBitCount = 0;
    dds.dwPFRGBBitCount = 32;
    dds.dwPFRBitMask = 0xFF;
    dds.dwPFGBitMask = 0xFF00;
    dds.dwPFBBitMask = 0xFF0000;
    dds.dwPFABitMask = 0xFF000000;
    dds.dwCaps = 0x1000;
    dds.dwCaps2 = 0;
    dds.dwCaps3 = 0;
    dds.dwCaps4 = 0;
    dds.dwReserved2 = 0;
    if (bCompressed)
    {
        dds.dwPFFlags = 0x1 + 0x4;  // contains alpha data + contains compressed RGB data
        dds.dwPFFourCC = 808540228;
        dxt.dxgiFormat = textureFormat;
        dxt.resourceDimension = 3;  // DDS_DIMENSION_TEXTURE2D
        if (arraySize % 6 == 0)
        {
            // Compressed cubemap
            dxt.miscFlag = 4;
            dxt.arraySize = arraySize / 6;
        }
        else
        {
            // Compressed BCn
            dxt.miscFlag = 0;
            dxt.arraySize = 1;
        }
    }
    else
    {
        // Uncompressed
        dds.dwPFFlags = 0x1 + 0x40;  // contains alpha data + contains uncompressed RGB data
        dds.dwPFFourCC = 0;
        dxt.miscFlag = 0;
        dxt.arraySize = 1;
        dxt.miscFlags2 = 0x1;
    }

    writeFile(dds, dxt, fullSavePath);
}


void Texture::writeFile(DDSHeader dds, DXT10Header dxt, std::string fullSavePath)
{
    FILE* outputFile;

    fopen_s(&outputFile, fullSavePath.c_str(), "wb");
    if (outputFile != NULL) {
        fwrite(&dds, sizeof(struct DDSHeader), 1, outputFile);
        fwrite(&dxt, sizeof(struct DXT10Header), 1, outputFile);
        int fileSize = dataFile->getData();
        fwrite(dataFile->data, fileSize, 1, outputFile);
        fclose(outputFile);
    }
}

void Material::parseMaterial(std::unordered_map<uint64_t, uint32_t> hash64Table)
{
    uint32_t fileSize;
    fileSize = getData();
    uint32_t textureCount;
    uint32_t textureOffset;
    memcpy((char*)&textureCount, data + 0x2B8, 4);
    if (textureCount == 0)
        return;
    uint32_t off = 0;
    bool bFound = false;
    uint32_t val;
    off = fileSize - 32;
    while (true)
    {
        if (off == 0)
            break;
        memcpy((char*)&val, data + off, 4);
        if (val == 0x80806DCF)
        {
            bFound = true;
            off += 8;
            textureOffset = off;
            break;
        }
        off -= 4;
    }
    if (!bFound) {
        return;
    }
    uint64_t h64Val;
    for (int i = textureOffset; i < textureOffset + textureCount * 0x18; i += 0x18)
    {
        uint8_t textureIndex;
        memcpy((char*)&textureIndex, data + i, 1);
        uint32_t val;
        memcpy((char*)&val, data + i + 8, 4);
        std::string h64Check = uint32ToHexStr(val);
        if (h64Check == "ffffffff")
        {
            memcpy((char*)&h64Val, data + i + 0x10, 8);
            if (h64Val == 0) continue;
            std::string textureHash = getHash64(h64Val, hash64Table);
            if (textureHash != "ffffffff")
            {
                Texture* texture = new Texture(textureHash, packagesPath);
                textures[textureIndex] = texture;
            }
        }
        else if (h64Check.substr(h64Check.length() - 2) == "80" && h64Check.substr(h64Check.length() - 4) != "8080")
        {
            std::string textureHash = h64Check;// getReferenceFromHash(h64Check, packagesPath);
            //std::cout << textureHash + "\n"; //debugging nonsense
            Texture* texture = new Texture(textureHash, packagesPath);
            textures[textureIndex] = texture;
        }
        else
        {
            printf("Support some texture format?");
            return;
        }
    }
}

void Material::exportTextures(std::string fullSavePath, std::string saveFormat)
{
    std::string actualSavePath;
    std::string newPath;
    for (auto& element : textures)
    {
        uint8_t texID = element.first;
        Texture* tex = element.second;
        actualSavePath = fullSavePath + "/" + tex->hash + ".dds";
        newPath = fullSavePath + "/" + tex->hash + "." + saveFormat;
        std::ifstream f(newPath);
        std::ifstream q(actualSavePath);
        if (f || q)
        {
            free(tex);
            continue;
        }
        if (saveFormat == "dds") tex->tex2DDS(actualSavePath);
        else tex->tex2Other(actualSavePath, saveFormat);
        free(tex);
    }
}