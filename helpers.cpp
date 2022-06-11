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
	std::string reference = pkg.getEntryReference(hash);
	return reference;
}

std::string getPkgID(std::string hash)
{
	std::string pkgID = uint16ToHexStr(floor((hexStrToUint32(hash) - 0x80800000) / 8192));
	return pkgID;
}

uint16_t getPkgID(uint32_t hash)
{
	uint16_t pkgID = floor((hash - 0x80800000) / 8192);
	return pkgID;
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
	x = (x << 8) + (x >> 8);
	return x;
}

uint32_t swapUInt32Endianness(uint32_t x)
{
	x = (x >> 24) |
		((x << 8) & 0x00FF0000) |
		((x >> 8) & 0x0000FF00) |
		(x << 24);
	return x;
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

std::string load3(const std::string& path) {

	auto close_file = [](FILE* f) {fclose(f); };

#pragma warning(suppress : 4996)
	auto holder = std::unique_ptr<FILE, decltype(close_file)>(fopen(path.c_str(), "rb"), close_file);
	if (!holder)
		return "";

	FILE* f = holder.get();

	// in C++17 following lines can be folded into std::filesystem::file_size invocation
	int size = std::filesystem::file_size(path);

	std::string res;
	res.resize(size);

	// C++17 defines .data() which returns a non-const pointer
	fread(const_cast<char*>(res.data()), 1, size, f);

	return res;
}

void filePutContents(const std::string& name, const std::string& content) {
	std::ofstream outfile;
	outfile.open(name, std::ios_base::app);
	outfile << content;
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

std::string to_str(double a_value)
{
	double a = a_value;
	std::stringstream out;
	out << std::fixed << std::setprecision(6) << a;
	return out.str();
}

File::File(std::string x, std::string pkgsPath)
{
	hash = x;
	packagesPath = pkgsPath;
}

int File::getData()
{
	if (hash.substr(hash.length() - 2) != "80" || hash.substr(hash.length() - 4) == "8080") return 0;

	pkgID = getPkgID(hash);
	Package pkg(pkgID, packagesPath);
	int fileSize;
	data = pkg.getEntryData(hash, fileSize);
	if (data == nullptr || sizeof(data) == 0) return 0;
	return fileSize;
}