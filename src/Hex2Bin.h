#ifndef HEX2BIN_H
#define HEX2BIN_H

#define HEX_MAX_LENGTH  128
#define HEX_MIN_LEN     1

enum RESULT_STATUS{
	RES_OK,
	RES_LENGTH_ERROR,
	RES_DATA_TOO_LONG,
	RES_DATA_TOO_SHORT,
	RES_CHECK_ERROR,
	RES_TYPE_ERROR,
	RES_HEX_FILE_NOT_EXIST,
	RES_NO_COLON
};

struct BinFormat{
	uint8_t len;
	uint16_t addr;
	uint8_t type;
	uint8_t *data;
	uint16_t base_addr;
};

struct SaveInfo {
	uint8_t len;
	uint8_t *data;
	uint16_t base_addr;
};

RESULT_STATUS getData(const char* src, char* out, unsigned int start_addr, unsigned int* datalen);

#endif