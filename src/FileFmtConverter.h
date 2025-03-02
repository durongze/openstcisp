#ifndef FILE_FMT_CONVERTER_H
#define FILE_FMT_CONVERTER_H

#include "HexFile.h"
#include "BinFile.h"

class FileFmtConverter
{
	public :
	FileFmtConverter();
	~FileFmtConverter();
	int Hex2Bin(const HexFile &hex, BinFile &bin);
	private:
};

#endif