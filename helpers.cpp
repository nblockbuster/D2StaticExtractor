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
	if (hash.substr(hash.length() - 2) != "80" || hash.substr(hash.length() - 4) == "8080")
		if (hash.substr(hash.length() - 2) != "81")
			return 0;

	pkgID = getPkgID(hash);
	Package pkg(pkgID, packagesPath);
	int fileSize;
	data = pkg.getEntryData(hash, fileSize);
	if (data == nullptr || sizeof(data) == 0) return 0;
	return fileSize;
}

std::string Logger::currentDateTime() {
	time_t now = time(0);
	struct tm tstruct;
	char buf[80];
	localtime_s(&tstruct, &now);

	strftime(buf, sizeof(buf), "%H:%M:%S", &tstruct);

	return buf;
}
void Logger::Debug(std::string message)
{
	//if (LoggerLevel < 4)
		//return;
#if !defined(NDEBUG)
	std::string curdatetime = currentDateTime();
	std::cout << curdatetime << " [DEBUG] " + message << "\n";
#endif
}
void Logger::Info(std::string message)
{
	if (LoggerLevel < 3)
		return;
	std::string curdatetime = currentDateTime();
	std::cout << curdatetime << " [INFO] " + message << "\n";
}
void Logger::Warning(std::string message)
{
	if (LoggerLevel < 2)
		return;
	std::string curdatetime = currentDateTime();
	std::cout << curdatetime << " [WARNING] " + message << "\n";
}
void Logger::Error(std::string message)
{
	if (LoggerLevel < 1)
		return;
	std::string curdatetime = currentDateTime();
	std::cout << curdatetime << " [ERROR] " + message << "\n";
}
