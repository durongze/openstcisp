#ifndef HEXFILE_H
#define HEXFILE_H

#include <string>
#include <vector>

#define HEX_LINE_MAX          256

#define HEX_LINE_LEN_STR_LEN  2
#define HEX_REC_ADDR_STR_LEN  4
#define HEX_REC_TYPE_STR_LEN  2

#define HEX_CHK_CODE_STR_LEN  2

int HexStrToHexVal(const std::string& lineData, std::vector<unsigned char>& lineVal);
int HexValToHexStr(const std::vector<unsigned char>& lineVal, std::string& lineData);
int HexVal2HexStr(const std::vector<char>& lineVal, std::string& lineData);

class HexFile
{
public:
    HexFile(const std::string& fileName);
    ~HexFile();
    int GetSingleLineData(int lineNo, unsigned char& dataLen, unsigned short& recordAddr, unsigned char& recordType, std::string& lineData, unsigned char& checkCode) const;
    int GetMultiLineData(int lineStart, int lineEnd, std::string& data);
    int TryNextLineData(std::ifstream& fs, unsigned short &Addr);
    int TryNextLineData(int lineNo,  unsigned short& recordAddr) const;
    int Exist()const;
private:
    int ParseLineData(const std::string& lineRaw, unsigned char& dataLen, unsigned short& recordAddr, unsigned char& recordType, std::string& lineData, unsigned char& checkCode)const;
    int UpperHexStr2LowerHexStr(std::string& lineRaw);
private:
    std::string   m_fileName;
};


#endif