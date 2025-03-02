#ifndef CFG_INI_H
#define CFG_INI_H

#include <stdint.h>

#include <string>
#include <vector>

#define INI_LINE_MAX          256

#define IniKeyHexPath   "HexPath"
#define IniKeyAssign   "="

class IniFile
{
public:
    IniFile(const std::string& fileName);
    ~IniFile();
    int GetHexPath(const std::string& line_key, std::string& line_val);
    int Exist();
private:
    int ParseLineData(const std::string& lineRaw, const std::string& line_key, std::string& line_val);
private:
    std::string   m_fileName;
};

#endif