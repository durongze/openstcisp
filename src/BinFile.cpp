#include "BinFile.h"
#include <fstream>
#include <strstream>
#include <iostream>
#include <iomanip>
#include "HexFile.h"

BinFile::BinFile(const std::string& fileName) :m_fileName(fileName) {}

BinFile::~BinFile() {}

int BinFile::GetSingleLineData(int lineNo, std::vector<unsigned char>& lineVal) {
	std::ifstream fs;
	int lineIdx;
	int lineCnt = 0;
	fs.open(m_fileName, std::ios::in | std::ios::binary);
	for (lineIdx = 1; lineIdx <= lineNo; lineIdx++) {
		fs.read((char*)lineVal.data(), lineVal.size());
	}
	lineCnt = lineVal.size() - fs.gcount();
	while (lineCnt--) {
		lineVal.pop_back();
	}
	fs.close();
	return 0;
}

int BinFile::GetMultiLineData(int lineStart, int lineEnd, std::string& data) {
	std::ifstream fs;
	int lineIdx;
	std::string lineStr;
	int lineSize = 16;
	int lineCnt = 0;
	std::vector<unsigned char> lineVal;
	lineVal.resize(lineSize);
	data = lineStr;
	// std::cout << "[" << lineStart << ":" << lineEnd << "]" << std::endl;
	fs.open(m_fileName, std::ios::in | std::ios::binary);
	for (lineIdx = 1; lineIdx <= lineEnd; lineIdx++) {
		fs.read((char*)lineVal.data(), lineVal.size());
		if (lineIdx < lineStart) { 
			continue; 
		}
		lineCnt = lineVal.size() - fs.gcount();
		for (; lineCnt > 0; lineCnt--) {
			lineVal.pop_back();
		}
		while (lineVal.size() < lineSize) {
			lineVal.push_back(0xff);
		}
		HexValToHexStr(lineVal, lineStr);
		LowerHexStr2UpperHexStr(lineStr);
		// std::cout << "[" << lineIdx << "]:" << lineStr << std::endl;
		data += std::string((char*)lineVal.data(), lineVal.size());
	}

	fs.close();
	return 0;
}

int BinFile::LowerHexStr2UpperHexStr(std::string& lineRaw)
{
	int idx = 0;
	int len = lineRaw.length();
	for (idx = 0; idx < len; ++idx) {
		lineRaw.at(idx) = (lineRaw.c_str()[idx] >= 'a' && lineRaw.c_str()[idx] <= 'z') ? lineRaw.c_str()[idx] - ('a' - 'A') : lineRaw.c_str()[idx];
	}
	return 0;
}

int BinFile::Exist(bool isCreate)
{
	int lineIdx = 0;
	std::ifstream fs;
	std::string lineStr;
	int lineSize = 16;
	std::vector<unsigned char> lineVal;
	lineVal.resize(lineSize);
	if (isCreate) {
		fs.open(m_fileName, std::ios::out | std::ios::trunc);
		fs.close();
		return 0;
	}

    fs.open(m_fileName, std::ios::in|std::ios::binary);

	if (fs.is_open()) {
		for (;; ) { // 循环直到getline返回false
			auto &ret = fs.read((char*)lineVal.data(), lineVal.size());
			if (!ret) {
				lineSize = lineVal.size() - fs.gcount();
				while (lineSize--) {
					lineIdx++;
					lineVal.pop_back();
				}
				HexValToHexStr(lineVal, lineStr);
				LowerHexStr2UpperHexStr(lineStr);
				std::cout << std::dec << "bin line[" << lineIdx << "]:" << lineStr << std::endl;
				break;
			}
			HexValToHexStr(lineVal, lineStr);
			LowerHexStr2UpperHexStr(lineStr);
			// std::cout << std::dec << "[" << lineIdx << "]:" << lineStr << std::endl;
			lineIdx++;
		}
		fs.close();
		return lineIdx;
	}
	return 0;
}

int BinFile::WriteData(const std::vector<unsigned char>& lineVal)
{
	int charIdx = 0;
	std::fstream fs;
	std::string lineRaw;

	fs.open(m_fileName, std::ios::out | std::ios::binary | std::ios::app);
	if (fs.is_open()) {
		fs.write((char*)lineVal.data(), lineVal.size());
		fs.close();
		return charIdx;
	}

	return 0;
}
