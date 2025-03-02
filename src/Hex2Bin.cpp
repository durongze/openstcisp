#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "Hex2Bin.h"

const char* RetString[] =
{
    "bin file to hex file success!",
    "line data of hex file is too large",
    "line data of hex file is too short",
    "line data of hex file is no colon",
    "line data of hex file type is error",
    "line data of hex file length is error",
    "line data of hex file check error",
    "hex file is not exist",
    "bin file path is error",
    "hex file is no end"
};
 
/********************************************************************************
input:
c:单个字符('0'~'9' 'a'~'f', 'A'~'F')
output:
单个字符转化为单个字符
********************************************************************************/
static uint8_t HexCharToBinBinChar(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    else if (c >= 'a' && c <= 'z')
        return c - 'a' + 10;
    else if (c >= 'A' && c <= 'Z')
        return c - 'A' + 10;
    return 0xff;
}
 
/********************************************************************************
input:
p: 两个文本字符
output:
转化为1个字节
********************************************************************************/
static uint8_t Hex2Bin(const char *p)
{
    uint8_t tmp = 0;
    tmp = HexCharToBinBinChar(p[0]);
    tmp <<= 4;
    tmp |= HexCharToBinBinChar(p[1]);
    return tmp;
}
/********************************************************************************
input:
src: hex单行字符串
p->type: 如果函数返回结果正确，这里就存着转化后的类型
p->len: 如果函数运行正确，这里就存着转化后的bin数据长度
p->data: 如果函数运行正确，长度并且不为0，该指针就只想转化后的数据
p->addr[0]: 如果函数返回结果正确，这里就存着地址的低字节
p->addr[1]: 如果函数返回结果正确，这里就存着地址的低字节
output:
返回hex格式流分析的结果
********************************************************************************/
 
static RESULT_STATUS parserLine(char *src, BinFormat *p)
{
    static int cnt = 0; //count
    char buf[4];
    static int flag = 0;
    uint8_t check = 0, tmp[4], binLen;
    uint16_t hexLen = strlen(src);
    uint16_t num = 0, offset = 0;
    if (hexLen > HEX_MAX_LENGTH)//数据内容过长
        return RES_DATA_TOO_LONG;
    if (hexLen < HEX_MIN_LEN)
        return RES_DATA_TOO_SHORT;//数据内容过短
    if (src[0] != ':')
        return RES_NO_COLON;//没有冒号
    if ((hexLen - 1) % 2 != 0)
        return RES_LENGTH_ERROR;//hexLen的长度应该为奇数
    binLen = (hexLen - 1) / 2;//bin总数据的长度，包括长度，地址，类型校验等内容
    while (num < 4)
    {
        offset = (num << 1) + 1;
        tmp[num] = Hex2Bin(src + offset);
        check += tmp[num];
        num++;
    }
    //把解析的这些数据保存到结构体中
    p->len = tmp[0];
    p->addr = tmp[1];
    p->addr <<= 8;
    p->addr += tmp[2];
    p->type = tmp[3];
    while (num < binLen)
    {
        offset = (num << 1) + 1;//保存真正的bin格式数据流
        p->data[num - 4] = Hex2Bin(src + offset);
        check += p->data[num - 4];
        num++;
    }
    //获取扩展地址
    if (p->type == 4)
    {
        strncpy(buf, src + 9, 4);
        p->base_addr = p->data[0];
        p->base_addr <<= 8;
        p->base_addr += p->data[1];
        cnt++;
    }
 
    //检查hex格式流的长度和数据的长度是否一致 
    if (p->len != binLen - 5)
        return RES_LENGTH_ERROR;
    //检查校验是否通过
    if (check != 0)
        return RES_CHECK_ERROR;
    return RES_OK;
}

RESULT_STATUS getData(const char *src, char *out, unsigned int start_addr, unsigned int *datalen)
{
 
    FILE *src_file;
    uint16_t addr_low = 0;
    uint32_t addr_hign = 0;
    char buffer_hex[1024];
    uint8_t buffer_bin[1024];
    BinFormat p = {0};
    SaveInfo sp = {0};
    RESULT_STATUS res;
    char read_flag = 0;
    unsigned int file_pos=0;
    int filesize = 0;
    p.data = buffer_bin;
    src_file = fopen(src, "r");//以文本的形式打开一个hex文件
    if (!src_file)
        return RES_HEX_FILE_NOT_EXIST;
 
    fseek(src_file, 0, SEEK_SET); //定位到开头，准备开始读取数据  
    while (!feof(src_file))
    {
        fscanf(src_file, "%s\r\n", buffer_hex);
        res = parserLine(buffer_hex, &p);
        if (res != RES_OK)
        {
            fclose(src_file);
            return res;
        }
        switch (p.type)
        {
        case 0: //数据记录
            addr_low = p.addr;
            sp.base_addr = addr_low + addr_hign; //地址偏移
            sp.len = p.len;
            sp.data = p.data;
            if ((start_addr <= sp.base_addr)&& ((start_addr+ *datalen - 0x20)>= sp.base_addr))//偏移到指定位置
            {
                read_flag = 1;//开始读标志
            }
            else {
                read_flag = 0;
            }
            if (read_flag==1)
            {
                memcpy(out + sp.base_addr, (void*)sp.data, sp.len);//拷贝数据
            }
            break;
        case 1: //数据结束
            return RES_OK;
            break;
        case 2:
            addr_hign = ((uint32_t)p.base_addr) << 2;
            break;
        case 3:
            continue;
        case 4:
            //用来标识扩展线性地址的记录
            addr_hign = ((uint32_t)p.base_addr) << 16;
            break;
        case 5:
            continue;
        default:
            fclose(src_file);
            return RES_TYPE_ERROR;
        }
    }
    
    fclose(src_file);
    return RES_LENGTH_ERROR;
}

char *loadHexFile(const char* hexfile)
{
    int i;
    unsigned int hex_len = 0;
    char* hex_mem = NULL;
    FILE* fp = fopen(hexfile, "r");
    if (!fp) {
        return NULL;
    }
    fseek(fp, 0, SEEK_END);
    hex_len = ftell(fp);
    hex_mem = (char*)malloc(hex_len);
    if (NULL != hex_mem) {
        for (i = 0; i < hex_len; ++i) {
            if (0 >= fread((char*)hex_mem + i, 1, 1, fp)) {
                break;
            }
        }
        hex_len = i;
    }
    fclose(fp);
    return hex_mem;
}