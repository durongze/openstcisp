#include "HexFile.h"
#include <fstream>
#include <strstream>
#include <iostream>
#include <iomanip>
#include <string.h>

unsigned char HexToAscii(const char ch)
{
	if (ch <= 9 && ch >= 0)
	{
		return (ch + '0');
	}
	if (ch <= 15 && ch >= 10)
	{
		return (ch + 'a' - 10);
	}
	return ch;
}

unsigned char AsciiToHex(const char ch)
{
	if (ch <= '9' && ch >= '0')
	{
		return (ch - '0');
	}
	if (ch <= 'f' && ch >= 'a')
	{
		return ((ch - 'a') + 10);
	}
	if (ch <= 'F' && ch >= 'A')
	{
		return ((ch - 'A') + 10);
	}
	return ch;
}

unsigned char ByteStrToHex(const char *str_byte)
{
	return AsciiToHex(str_byte[1]) | (AsciiToHex(str_byte[0]) << 4);
}

short ShortStrToHex(const char *str_byte)
{
	return ByteStrToHex(str_byte + 2) | (ByteStrToHex(str_byte) << 8);
}

HexFile::HexFile(const std::string &fileName) : m_fileName(fileName) {}

HexFile::~HexFile() {}

int HexFile::GetSingleLineData(int lineNo, unsigned char &dataLen, unsigned short &recordAddr, unsigned char &recordType, std::string &lineData, unsigned char &checkCode) const
{
	std::ifstream fs;
	std::string lineRaw;
	int lineIdx;
	dataLen = 0;
	recordAddr = 0;
	recordType = 0;
	checkCode = 0;
	fs.open(m_fileName, std::ios::in);
	for (lineIdx = 0; lineIdx < lineNo; lineIdx++)
	{
		std::getline(fs, lineRaw);
	}
	ParseLineData(lineRaw, dataLen, recordAddr, recordType, lineData, checkCode);
	fs.close();
	return 0;
}

int HexFile::GetMultiLineData(int lineStart, int lineEnd, std::string &data)
{
	std::ifstream fs;
	std::string lineRaw;
	int lineIdx;

	unsigned char dataLen;
	unsigned short recordAddr;
	unsigned char recordType;
	std::string lineData;
	unsigned char checkCode;

	std::vector<unsigned char> lineVal;

	unsigned short nextAddr = 0;

	fs.open(m_fileName, std::ios::in);
	for (lineIdx = 0; lineIdx <= lineEnd; lineIdx++)
	{
		std::getline(fs, lineRaw);
		if (lineIdx < lineStart)
		{
			continue;
		}
		ParseLineData(lineRaw, dataLen, recordAddr, recordType, lineData, checkCode);
		if (0 == recordType)
		{
			TryNextLineData(fs, nextAddr);
			for (recordAddr += dataLen; recordAddr < nextAddr; recordAddr++)
			{
				lineData.append("ff");
			}
			UpperHexStr2LowerHexStr(lineData);
			// std::cout << "[" << lineIdx << "]    " << lineData << std::endl;
			HexStrToHexVal(lineData, lineVal);
			data += lineData;
		}
	}

	fs.close();
	return 0;
}

int HexFile::TryNextLineData(std::ifstream &fs, unsigned short &Addr)
{
	std::string lineRaw;

	auto last_pos = fs.tellg();
	std::getline(fs, lineRaw);
	fs.seekg(last_pos);

	unsigned char dataLen;
	unsigned short recordAddr;
	unsigned char recordType;
	std::string lineData;
	unsigned char checkCode;

	ParseLineData(lineRaw, dataLen, recordAddr, recordType, lineData, checkCode);

	Addr = recordAddr;
	return 0;
}

int HexFile::TryNextLineData(int lineNo, unsigned short &recordAddr) const
{
	std::ifstream fs;
	std::string lineRaw;
	int lineIdx;

	fs.open(m_fileName, std::ios::in);
	unsigned char recordType = 0;

	for (lineIdx = 1; lineIdx < lineNo; lineIdx++)
	{
		std::getline(fs, lineRaw);
	}

	do
	{
		unsigned char dataLen;
		std::string lineData;
		unsigned char checkCode;
		recordAddr = 0;
		auto &ret = std::getline(fs, lineRaw);
		if (!ret)
		{
			break;
		}
		ParseLineData(lineRaw, dataLen, recordAddr, recordType, lineData, checkCode);

	} while (0 != recordType);
	fs.close();
	return 0;
}

int HexStrToHexVal(const std::string &lineData, std::vector<unsigned char> &lineVal)
{
	int byteIdx = 0;
	int byteLen = lineData.length();
	lineVal.resize(byteLen >> 1);
	for (byteIdx = 0; byteIdx < byteLen; byteIdx += 2)
	{
		lineVal[byteIdx >> 1] = ByteStrToHex(lineData.c_str() + byteIdx);
		// std::cout << (byteIdx >> 1) << ' ';
		// std::cout << std::hex << std::setiosflags(std::ios::uppercase) << std::setfill('0') << std::setw(2);
		// std::cout << (int)lineVal[byteIdx >> 1] << std::endl;
	}
	return 0;
}

int HexValToHexStr(const std::vector<unsigned char> &lineVal, std::string &lineData)
{
	int byteIdx = 0;
	int byteLen = lineVal.size();
	lineData = "";
	for (byteIdx = 0; byteIdx < byteLen; byteIdx += 1)
	{
		lineData += HexToAscii((lineVal[byteIdx] >> 4) & 0xF);
		lineData += HexToAscii((lineVal[byteIdx] >> 0) & 0xF);
	}
	return 0;
}

int HexVal2HexStr(const std::vector<char> &lineVal, std::string &lineData)
{
	int byteIdx = 0;
	int byteLen = lineVal.size();
	lineData = std::string();
	for (byteIdx = 0; byteIdx < byteLen; byteIdx += 1)
	{
		lineData += HexToAscii((lineVal[byteIdx] >> 4) & 0xF);
		lineData += HexToAscii((lineVal[byteIdx] >> 0) & 0xF);
	}
	return 0;
}

int HexFile::Exist() const
{
	int lineIdx = 0;
	std::ifstream fs;
	std::string lineRaw;

	fs.open(m_fileName, std::ios::in);
	if (fs.is_open())
	{
		for (;;)
		{
			auto &ret = std::getline(fs, lineRaw);
			if (!ret)
			{
				std::cout << "hex line[" << lineIdx << "]:" << lineRaw << std::endl; // 输出当前行
				break;
			}
			lineIdx++;
		}
		fs.close();
		return lineIdx;
	}
	else
	{
		std::cout << "Hex file '" << m_fileName << "' does not exist!" << std::endl;
	}
	return 0;
}

int HexFile::ParseLineData(const std::string &lineRaw, unsigned char &dataLen, unsigned short &recordAddr, unsigned char &recordType, std::string &lineData, unsigned char &checkCode) const
{
	dataLen = 0;
	recordAddr = 0;
	recordType = 0;
	checkCode = 0;

	char str_byte[64];
	std::istrstream iss(lineRaw.c_str());
	iss.read(str_byte, 1);
	memset(str_byte, 0, sizeof(str_byte));
	iss.read(str_byte, HEX_LINE_LEN_STR_LEN);
	// dataLen = std::atoi(str_byte);
	dataLen = ByteStrToHex(str_byte);
	memset(str_byte, 0, sizeof(str_byte));
	iss.read(str_byte, HEX_REC_ADDR_STR_LEN);
	// recordAddr = std::atoi(str_byte);
	recordAddr = ShortStrToHex(str_byte);
	memset(str_byte, 0, sizeof(str_byte));
	iss.read(str_byte, HEX_REC_TYPE_STR_LEN);
	// recordType = std::atoi(str_byte);
	recordType = ByteStrToHex(str_byte);
	memset(str_byte, 0, sizeof(str_byte));
	iss.read(str_byte, dataLen * 2);
	lineData = str_byte;
	memset(str_byte, 0, sizeof(str_byte));
	iss.read(str_byte, HEX_CHK_CODE_STR_LEN);
	// sscanf(str_byte,"%02X", checkCode);
	checkCode = ByteStrToHex(str_byte);
	return 0;
}

int HexFile::UpperHexStr2LowerHexStr(std::string &lineRaw)
{
	int idx = 0;
	int len = lineRaw.length();
	for (idx = 0; idx < len; ++idx)
	{
		lineRaw.at(idx) = (lineRaw.c_str()[idx] >= 'A' && lineRaw.c_str()[idx] <= 'Z') ? lineRaw.c_str()[idx] + ('a' - 'A') : lineRaw.c_str()[idx];
	}
	return 0;
}
