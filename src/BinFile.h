#ifndef BINFILE_H
#define BINFILE_H

#include <string>
#include <vector>

#define HEX_LINE_MAX          256

#define HEX_LINE_LEN_STR_LEN  2
#define HEX_REC_ADDR_STR_LEN  4
#define HEX_REC_TYPE_STR_LEN  2

#define HEX_CHK_CODE_STR_LEN  2

class BinFile
{
public:
    BinFile(const std::string& fileName);
    ~BinFile();
    int GetSingleLineData(int lineNo, std::vector<unsigned char>& lineVal);
    int GetMultiLineData(int lineStart, int lineEnd, std::string& data);
    int Exist(bool isCreate = false);
    int WriteData(const std::vector<unsigned char> &lineVal);
private:
    int LowerHexStr2UpperHexStr(std::string& lineRaw);
private:
    std::string   m_fileName;
};


#endif