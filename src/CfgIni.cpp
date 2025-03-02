#include "CfgIni.h"
#include <fstream>
#include <strstream>
#include <iostream>
#include <iomanip>
#include <stdint.h>

IniFile::IniFile(const std::string &fileName) : m_fileName(fileName) {}

IniFile::~IniFile() {}

int IniFile::GetHexPath(const std::string &line_key, std::string &line_val)
{
	std::ifstream fs;
	std::string lineRaw;
	int lineIdx = 0;
	fs.open(m_fileName, std::ios::in);

	if (fs.is_open())
	{
		for (;;)
		{
			auto &ret = std::getline(fs, lineRaw);
			if (!ret)
			{
				break;
			}

			ParseLineData(lineRaw, line_key, line_val);
			if (line_val.length() > 0)
			{
				std::cout << lineIdx++ << "    " << line_val << std::endl; // 输出当前行
			}
		}
		fs.close();
		return lineIdx;
	}
	else
	{
		std::cout << "Ini file '" << m_fileName << "' does not exist!" << std::endl;
	}
	return 0;
}

int IniFile::Exist()
{
	return 0;
}

int IniFile::ParseLineData(const std::string &lineRaw, const std::string &line_key, std::string &line_val)
{
	std::string keypath = IniKeyHexPath;
	std::string keysig = IniKeyAssign;
	size_t key_start;
	size_t key_end;
	size_t sig_start;
	size_t sig_end;
	key_start = lineRaw.find(keypath);
	if (key_start >= 0 && key_start < lineRaw.length())
	{
		key_end = key_start + keypath.length();
		sig_start = lineRaw.find(keysig);
		if (sig_start >= key_end && key_start < lineRaw.length())
		{
			sig_end = sig_start + keysig.length();
			line_val = lineRaw.substr(sig_end);
		}
	}

	return 0;
}