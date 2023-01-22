#include "helpers.h"

std::string uint16ToHexStr(uint16_t num)
{
	std::stringstream stream;
	stream << std::hex << num;
	std::string hexStr = stream.str();
	if (hexStr.size() % 4 != 0)
		hexStr = std::string(4 - (hexStr.size() % 4), '0').append(hexStr);
	return hexStr;
}

std::string uint32ToHexStr(uint32_t num)
{
	std::stringstream stream;
	stream << std::hex << swapUInt32Endianness(num);
	std::string hexStr = stream.str();
	if (hexStr.size() % 8 != 0)
		hexStr = std::string(8 - (hexStr.size() % 8), '0').append(hexStr);
	return hexStr;
}

std::string getReferenceFromHash(std::string hash, std::string pkgsPath)
{
	Package pkg(getPkgID(hash), pkgsPath);
	return pkg.getEntryReference(hash);
}

std::string getPkgID(std::string hash)
{
	return uint16ToHexStr(getPkgID(hexStrToUint32(hash)));
}

// updated to just be the code from charm here: https://github.com/MontagueM/Charm/blob/main/Field/General/General.cs

uint16_t getPkgID(uint32_t hash)
{
	if ((hash & 0x01000000) != 0)
	{
		return (int)((hash >> 0xD) & 0x3FF) | 0x400;
	}
	return (int)((hash >> 0xD) & 0x3FF);
}

uint32_t hexStrToUint16(std::string hash)
{
	return swapUInt16Endianness(std::stoul(hash, nullptr, 16));
}

uint32_t hexStrToUint32(std::string hash)
{
	return swapUInt32Endianness(std::stoul(hash, nullptr, 16));
}

uint64_t hexStrToUint64(std::string hash)
{
	return swapUInt64Endianness(std::stoull(hash, nullptr, 16));
}

uint16_t swapUInt16Endianness(uint16_t x)
{
	return (x << 8) + (x >> 8);
}

uint32_t swapUInt32Endianness(uint32_t x)
{
	return (x >> 24) |
		((x << 8) & 0x00FF0000) |
		((x >> 8) & 0x0000FF00) |
		(x << 24);
}

uint64_t swapUInt64Endianness(uint64_t k)
{
	return ((k << 56) |
		((k & 0x000000000000FF00) << 40) |
		((k & 0x0000000000FF0000) << 24) |
		((k & 0x00000000FF000000) << 8) |
		((k & 0x000000FF00000000) >> 8) |
		((k & 0x0000FF0000000000) >> 24) |
		((k & 0x00FF000000000000) >> 40) |
		(k >> 56)
		);
}

std::string getFileFromHash(std::string hsh)
{
	uint32_t first_int = hexStrToUint32(hsh);
	uint32_t one = first_int - 2155872256;
	std::string first_hex = uint16ToHexStr(floor(one / 8192));
	std::string second_hex = uint16ToHexStr(first_int % 8192);
	return(first_hex + "-" + second_hex);
}

std::string getHash64(uint64_t hash64, std::unordered_map<uint64_t, uint32_t> hash64Table)
{
	std::string h64 = "";
	try
	{
		h64 = uint32ToHexStr(hash64Table[hash64]);
		if (h64 == "00000000")
			throw h64;
	}
	catch (std::string err)
	{
		std::cerr << "H64 file is out-of-date. Please delete and retry.\n";
		exit(1);
	}

	return h64;
}

std::string to_str(double a_value, int precision)
{
	double a = a_value;
	std::stringstream out;
	out << std::fixed << std::setprecision(precision) << a;
	return out.str();
}

File::File(std::string x, std::string pkgsPath)
{
	hash = x;
	packagesPath = pkgsPath;
}

int File::getData()
{
	if (!isHashValid(hash) || hash.substr(hash.length() - 4) == "8080") return 0;

	pkgID = getPkgID(hash);
	Package pkg(pkgID, packagesPath);
	int fileSize;
	data = pkg.getEntryData(hash, fileSize);
	if (data == nullptr || sizeof(data) == 0) return 0;
	return fileSize;
}

bool isHashValid(uint32_t hash) {
	return (hash >= 0x80a00000 && hash <= 0x81ffffff);
}

bool isHashValid(std::string hash) {
	return (hexStrToUint32(hash) >= 0x80a00000 && hexStrToUint32(hash) <= 0x81ffffff);
}