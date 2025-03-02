#include "FileFmtConverter.h"

#include <iostream>

FileFmtConverter::FileFmtConverter() {}
FileFmtConverter::~FileFmtConverter() {}

int FileFmtConverter::Hex2Bin(const HexFile &hex, BinFile &bin)
{
    int lineNo = 2;
    int lineMax = 0;
    unsigned char dataLen;
    unsigned short recordAddr;
    unsigned char recordType = -1;
    std::string lineData;
    unsigned char checkCode;
    lineMax = hex.Exist();

    unsigned short nextAddr;
    std::vector<unsigned char> lineVal;
    bin.Exist(true);

    std::cout << "hex [" << lineMax << "] lines" << std::endl;

    for (lineNo = 1; lineNo <= lineMax; lineNo++)
    {
        hex.GetSingleLineData(lineNo, dataLen, recordAddr, recordType, lineData, checkCode);
        if (0 == recordType)
        {
            hex.TryNextLineData(lineNo + 1, nextAddr);
            for (recordAddr += dataLen; recordAddr < nextAddr; recordAddr++)
            {
                lineData.append("ff");
            }
            HexStrToHexVal(lineData, lineVal);
            bin.WriteData(lineVal);
        }
    }

    return 0;
}