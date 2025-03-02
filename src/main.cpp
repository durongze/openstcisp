/*******************************************************

********************************************************/

#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <stdlib.h>

#include <hidapi.h>

// Headers needed for sleeping.
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

// Fallback/example
#ifndef HID_API_MAKE_VERSION
#define HID_API_MAKE_VERSION(mj, mn, p) (((mj) << 24) | ((mn) << 8) | (p))
#endif
#ifndef HID_API_VERSION
#define HID_API_VERSION HID_API_MAKE_VERSION(HID_API_VERSION_MAJOR, HID_API_VERSION_MINOR, HID_API_VERSION_PATCH)
#endif

//
// Sample using platform-specific headers
#if defined(__APPLE__) && HID_API_VERSION >= HID_API_MAKE_VERSION(0, 12, 0)
#include <hidapi_darwin.h>
#endif

#if defined(_WIN32) && HID_API_VERSION >= HID_API_MAKE_VERSION(0, 12, 0)
#include <hidapi_winapi.h>
#endif

#if defined(USING_HIDAPI_LIBUSB) && HID_API_VERSION >= HID_API_MAKE_VERSION(0, 12, 0)
#include <hidapi_libusb.h>
#endif
//

#include <iostream>
#include <iomanip>
#include <stdint.h>
#include "HexFile.h"
#include "Crc24.h"
#include "parser.h"
#include "CdcMsg.h"
#include "CfgIni.h"
#include "Hex2Bin.h"
#include "FileFmtConverter.h"
#include <filesystem>
#include <fstream>
#include "stcisp.h"

using namespace std;

// Arzawa Instrument's Frequency Meter device commands
const uint8_t CMD_GETTIME    = 0x10;
const uint8_t CMD_SETGTIME   = 0x11;
const uint8_t CMD_BEGINMEAS  = 0x20;
const uint8_t CMD_ABORTMEAS  = 0x21;
const uint8_t CMD_GETFREQ    = 0x22;
const uint8_t CMD_ENTERDFU   = 0x30;
const uint8_t CMD_GETHWINFO  = 0x40;
const uint8_t CMD_GETSWINFO  = 0x41;

// Arzawa Instrument's Frequency Meter device commands responses
const uint8_t RSP_OK         = 0x00;
const uint8_t RSP_BUSY       = 0x01;
const uint8_t RSP_INVALIDCMD = 0x02;
const uint8_t RSP_BADPARAM   = 0x03;
const uint8_t RSP_ERR0       = 0x04;
const uint8_t RSP_ERR2       = 0x05;
const uint8_t RSP_ERR3       = 0x06;
const uint8_t RSP_ERR4       = 0x07;


std::string   g_hexfile;

void set_hex_file_name(const std::string &hexfile)
{
    g_hexfile = hexfile;
}

void stcisp_usage(int argc, char **argv)
{
    std::string exec_dir;
    linked_version(argc, argv, __FUNCTION__);
    std::cout << std::endl << std::endl;
    std::cout << "Note:" << std::endl;
    std::cout << "\tStep 1 : Set hex file path to '"<< exec_dir <<"\\stcisp.ini'." << std::endl;
    std::cout << "\tStep 2 : Enter upgrade mode." << std::endl;
    std::cout << std::endl << std::endl;
}

void GetHwInfo(std::string &HidData)
{
    // Generate command
    HidData.clear();
    HidData.resize(65, 0);
    HidData[0] = 0;
    HidData[1] = static_cast<char>(CMD_GETHWINFO);
}

void GetSwInfo(std::string& HidData)
{
    // Generate command
    HidData.clear();
    HidData.resize(65, 0);
    HidData[0] = 0;
    HidData[1] = static_cast<char>(CMD_GETSWINFO);
}

void GetHwInfo8(std::string &HidData)
{
    int i = 0;
    // Generate command
    HidData.clear();
    HidData.resize(65, 0);
    HidData[0] = 0;
    HidData[1] = static_cast<char>(CMD_GETHWINFO);
}

void dump_read_data(const std::string &str, std::ostream &os = std::cout)
{
    int idx;
    int len;
    len = str.length();
    for (idx = 0; idx < len; idx++)
    {
        if (idx % 4 == 0)
        {
            os << " ";
        }
        os << std::hex << std::setiosflags(std::ios::uppercase) << std::setw(2) << std::setfill('0') << static_cast<int>(str.at(idx) & 0xff);
    }
    os << std::dec << std::endl
       << "read finished" << std::endl;
}

void ReadsFifoBuffer(const std::string &str, int pkgIdx, std::ostream &os = std::cout, hid_device *handle = NULL)
{

    read_requested_state(handle);

    return;
}

void GetVendorName(std::string& HidData)
{
    int i = 0;
    // Generate command
    HidData.clear();
    HidData.resize(65, 0);
    HidData[i++] = 0;
    HidData[i++] = 0x80;
    HidData[i++] = 0x06;
    HidData[i++] = 0x01;
    HidData[i++] = 0x03;
    HidData[i++] = 0x09;
    HidData[i++] = 0x04;
    HidData[i++] = 0x02;
    HidData[i++] = 0x02; 
}

void GetAppName(std::string& HidData)
{
    int i = 0;
    // Generate command
    HidData.clear();
    HidData.resize(65, 0);
    HidData[i++] = 0;
    HidData[i++] = 0x80;
    HidData[i++] = 0x06;
    HidData[i++] = 0x02;
    HidData[i++] = 0x03;
    HidData[i++] = 0x09;
    HidData[i++] = 0x04;
    HidData[i++] = 0x02;
    HidData[i++] = 0x02;
}

void Update001(std::string &HidData, int isTry, hid_device* handle)
{
    int res;
    int i = 1;
    // Generate command
    HidData.clear();
    HidData.resize(64 + isTry, 0);
    // 46 b9 6a 00  07 00 00 71  16
    if (isTry) { HidData[i++] = 0; }
    HidData[i++] = 0x46;
    HidData[i++] = 0xb9;
    HidData[i++] = 0x6a;
    HidData[i++] = 0x00;
    HidData[i++] = 0x07;
    HidData[i++] = 0x00;
    HidData[i++] = 0x00;
    HidData[i++] = 0x71;
    HidData[i++] = 0x16;
    res = hid_write(handle, (unsigned char*)HidData.c_str(), HidData.length());
}

void Update002(std::string &HidData, int isTry, hid_device* handle)
{
    int res;
    int i = 1;
    // Generate command
    HidData.clear();
    HidData.resize(64 + isTry, 0);
    // 46 b9 6a 00  0e 01 00 00  00 00 00 00  80 00 f9 16
    if (isTry) { HidData[i++] = 0; }
    HidData[i++] = 0x46;
    HidData[i++] = 0xb9;
    HidData[i++] = 0x6a;
    HidData[i++] = 0x00;
    HidData[i++] = 0x0e;
    HidData[i++] = 0x01;
    HidData[i++] = 0x00;
    HidData[i++] = 0x00;
    HidData[i++] = 0x00;
    HidData[i++] = 0x00;
    HidData[i++] = 0x00;
    HidData[i++] = 0x00;
    HidData[i++] = 0x80;
    HidData[i++] = 0x00;
    HidData[i++] = 0xf9;
    HidData[i++] = 0x16;
    res = hid_write(handle, (unsigned char*)HidData.c_str(), HidData.length());
}

void Update003(std::string &HidData, int isTry, hid_device* handle)
{
    int res;
    int i = 1;
    // Generate command
    HidData.clear();
    HidData.resize(64 + isTry, 0);
    // 46 b9 6a 00  0b 05 00 00  5a a5 01 79  16
    if (isTry) { HidData[i++] = 0; }
    HidData[i++] = 0x46;
    HidData[i++] = 0xb9;
    HidData[i++] = 0x6a;
    HidData[i++] = 0x00;
    HidData[i++] = 0x0b;
    HidData[i++] = 0x05;
    HidData[i++] = 0x00;
    HidData[i++] = 0x00;
    HidData[i++] = 0x5a;
    HidData[i++] = 0xa5;
    HidData[i++] = 0x01;
    HidData[i++] = 0x79;
    HidData[i++] = 0x16;
    res = hid_write(handle, (unsigned char*)HidData.c_str(), HidData.length());
}

void Update004(std::string &HidData, int isTry, hid_device* handle)
{
    int res;
    int i = 1;
    // Generate command
    HidData.clear();
    HidData.resize(64 + isTry, 0);
    // 46 b9 6a 00  0b 03 01 00  5a a5 01 78  16
    if (isTry) { HidData[i++] = 0; }
    HidData[i++] = 0x46;
    HidData[i++] = 0xb9;
    HidData[i++] = 0x6a;
    HidData[i++] = 0x00;
    HidData[i++] = 0x0b;
    HidData[i++] = 0x03;
    HidData[i++] = 0x01;
    HidData[i++] = 0x00;
    HidData[i++] = 0x5a;
    HidData[i++] = 0xa5;
    HidData[i++] = 0x01;
    HidData[i++] = 0x78;
    HidData[i++] = 0x16;
    res = hid_write(handle, (unsigned char*)HidData.c_str(), HidData.length());
}

void UpdateFileStart(std::string& HidData, const std::vector<unsigned char>& dataVal, int isTry)
{
    int i = 1;
    int idx = 0;
    int start = 0;
    int end = 16 * 3 + 6; 
    // Generate command
    HidData.clear();
    HidData.resize(65 + isTry, 0);
    // 46 b9 6a 00  8b 02 00 00  5a a5
    if (isTry) { HidData[i++] = 0; }
    HidData[i++] = 0x46;
    HidData[i++] = 0xb9;
    HidData[i++] = 0x6a;
    HidData[i++] = 0x00;
    HidData[i++] = 0x8b;
    HidData[i++] = 0x02;
    HidData[i++] = 0x00;
    HidData[i++] = 0x00;
    HidData[i++] = 0x5a;
    HidData[i++] = 0xa5;

    for (idx = start; idx < end; idx++) {
        HidData[i++] = dataVal[idx];
    }
}

void UpdateFileCtx(std::string &HidData, const std::vector<unsigned char>& dataVal, int isTry)
{
    int i = 1;
    int idx = 0;
    int start = 16 * 3 + 6;
    int end = 16 * 3 + 6 + 64;
    // Generate command
    HidData.clear();
    HidData.resize(65 + isTry, 0);
    if (isTry) { HidData[i++] = 0; }
    for (idx = start; idx < end && idx < dataVal.size(); idx++) {
        HidData[i++] = dataVal[idx];
    }
}

void UpdateFileEnd(std::string &HidData, const std::vector<unsigned char>& dataVal, int isTry, int crc24_val)
{
    int i = 1;
    int idx = 0;
    int start = 16 * 3 + 6 + 64;
    int end = 128; 
    // Generate command
    HidData.clear();
    HidData.resize(65 + isTry, 0);
    if (isTry) { HidData[i++] = 0; }
    for (idx = start; idx < end; idx++) {
        HidData[i++] = dataVal[idx];
    }
    HidData[i++] = 0x01;
    HidData[i++] = 0x02;
    HidData[i++] = 0x03;
    HidData[i++] = 0x04;
    HidData[i++] = 0x05;
    HidData[i++] = 0x06;
}

void test_data_end(std::vector<std::string>& data)
{
    data.push_back("46b96a00330400005aa5ffffff00ffff00ffffffffffffffffffffffffffffff00ff016e3600462001ffffff3e3fbfaff7001d76160000000000000000000000");
    data.push_back("46b96a0007ff01701600000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000");
    data.push_back("000000000000");
}

void test_hex_single_line()
{
    HexFile hex("test.hex");
    int lineNo = 2;
    unsigned char dataLen;
    unsigned short recordAddr;
    unsigned char recordType;
    std::string lineData;
    unsigned char checkCode;
    hex.GetSingleLineData(lineNo, dataLen, recordAddr, recordType, lineData, checkCode);
}

void test_hex_multi_line(std::string & hexfile)
{
    std::string data;
    std::vector<unsigned char> dataVal;
    HexFile hex(hexfile);
    uint32_t crc24_val;
    int idx = 0;
    hex.GetMultiLineData(1 + (idx * 8), 8 + (idx * 8), data);
    std::cout << idx << " " << data << std::endl;
    HexStrToHexVal(data, dataVal);
    crc24_val = crc24(0, dataVal.data(), dataVal.size());
    std::cout << "CRC:" << std::hex << crc24_val << std::endl;
}

void test_cdc_hex(const std::string& hexfile, std::vector<std::string>& data_vc, int& net_num)
{
    HexFile hex(hexfile);

    uint32_t crc24_val = 0;
    uint32_t line_num = hex.Exist();
    uint32_t line_num_pernet = 8;
    if (!line_num) {
        std::cout << "hex file does not exist." << std::endl;
        return;
    }
    int idx = 0;
    net_num = net_num % line_num_pernet ? net_num / line_num_pernet + 1 : net_num / line_num_pernet;
    data_vc.clear();
    for (idx = 0; idx < net_num; idx++) {
        int lineStart = 1 + (idx * line_num_pernet);
        int lineEnd = (1 + idx) * line_num_pernet;
        std::string multiData;

        std::string msgStart;
        std::string msgMid;
        std::string msgEnd;

        hex.GetMultiLineData(lineStart, lineEnd, multiData);

        CdcMsg msg;
        msg.SetHexCtx(multiData, idx * 0x80, crc24_val);
        msg.GenNetMsg(msgStart, msgMid, msgEnd);

        data_vc.push_back(msgStart);
        data_vc.push_back(msgMid);
        data_vc.push_back(msgEnd);
    }
    test_data_end(data_vc);
}
void test_cdc_bin(const std::string& binfile, std::vector<std::string>& data_vc, int& net_num)
{
    int idx = 0;
    std::string multiVal;
    std::vector<char> multiValVc;
    BinFile bin(binfile);
    uint32_t crc24_val = 0;
    net_num = bin.Exist();
    uint32_t line_num_pernet = 8;
    if (!net_num) {
        std::cout << "bin file does not exist." << std::endl;
        return;
    }
    net_num = net_num % line_num_pernet ? net_num / line_num_pernet + 1 : net_num / line_num_pernet;
    data_vc.clear();
    for (idx = 0; idx < net_num; idx++) {
        int lineStart = 1 + (idx * line_num_pernet);
        int lineEnd = (1 + idx) * line_num_pernet;
        std::string multiData;
        bin.GetMultiLineData(lineStart, lineEnd, multiVal);
        multiValVc.clear();
        for (auto iter = multiVal.begin(); iter != multiVal.end(); iter++) {
            multiValVc.push_back(*iter);
        }
        HexVal2HexStr(multiValVc, multiData);
        std::cout << "bin net["  << idx << "]:" << multiData << std::endl;
        CdcMsg msg;
        std::string msgStart;
        std::string msgMid;
        std::string msgEnd;
        msg.SetHexCtx(multiData, idx * 0x80, crc24_val);
        msg.GenNetMsg(msgStart, msgMid, msgEnd);

        data_vc.push_back(msgStart);
        data_vc.push_back(msgMid);
        data_vc.push_back(msgEnd);
    }
    test_data_end(data_vc);
}

void test_cdc(const std::string& file, std::vector<std::string>& data_vc, int &net_num)
{
    test_cdc_bin(file, data_vc, net_num);
}

int show_char(const char data_ch, const char cdc_ch)
{
    if (data_ch == cdc_ch) {
        std::cout << data_ch;
        return 0;
    }
    else {
        std::cout << "\033[31m";
        std::cout << data_ch;
        std::cout << "\033[0m";
        return 1;
    }
}

void show_string(const std::string &data_str, const std::string & cdc_str, const int net_part_idx)
{
    int idx = 0;
    int data_max = data_str.length();
    int cdc_max = cdc_str.length();
    std::cout << "data_vc[" << net_part_idx << "]:";
    std::vector<int> str_idx;
    for (idx = 0; idx < data_max; ++idx) {
        if (idx % 0) { std::cout << " "; }
        if (show_char(data_str.at(idx), cdc_str.at(idx))) {
            str_idx.push_back(idx + 1);
        }
    }
    std::cout << std::endl;
    std::cout << " cdc_vc[" << net_part_idx << "]:";
    for (idx = 0; idx < cdc_max; ++idx) {
        if (idx % 0) { std::cout << " "; }
        show_char(cdc_str.at(idx), data_str.at(idx));
    }
    std::cout << std::endl;
    std::cout << "str_idx("<< str_idx.size() << "):[";
    for (idx = 0; idx < str_idx.size(); ++idx) {
        std::cout << str_idx.at(idx) << ", ";
    }
    std::cout << "]" << std::endl;
}

int show_hex_char(const char *data_ch, const char *cdc_ch)
{
    if (data_ch[0] == cdc_ch[0] && data_ch[1] == cdc_ch[1]) {
        std::cout << data_ch[0] << data_ch[1];
        return 0;
    }
    else {
        std::cout << "\033[31m";
        std::cout << data_ch[0] << data_ch[1];
        std::cout << "\033[0m";
        return 1;
    }
}

void show_hex_string(const std::string& data_str, const std::string& cdc_str, const int net_part_idx)
{
    int idx = 0;
    int data_max = data_str.length();
    int cdc_max = cdc_str.length();
    std::cout << "data_vc[" << net_part_idx << "]:";
    std::vector<int> str_idx;
    for (idx = 0; idx < data_max; idx += 2) {
        std::cout << " ";
        if (show_hex_char(data_str.c_str() + idx, cdc_str.c_str() + idx)) {
            str_idx.push_back(idx / 2 + 1);
        }
    }
    std::cout << std::endl;
    std::cout << " cdc_vc[" << net_part_idx << "]:";
    for (idx = 0; idx < cdc_max; idx += 2) {
        std::cout << " ";
        show_hex_char(cdc_str.c_str() + idx, data_str.c_str() + idx);
    }
    std::cout << std::endl;
    std::cout << "str_idx(" << str_idx.size() << "):[";
    for (idx = 0; idx < str_idx.size(); ++idx) {
        std::cout << str_idx.at(idx) << ", ";
    }
    std::cout << "]" << std::endl;
}

void cmp_net_part(const std::vector<std::string>& data_vc, const std::vector<std::string>& cdc_vc, const int net_part_idx)
{
    if (data_vc[net_part_idx] != cdc_vc[net_part_idx]) {
        // show_string(data_vc[net_part_idx], cdc_vc[net_part_idx], net_part_idx);
        show_hex_string(data_vc[net_part_idx], cdc_vc[net_part_idx], net_part_idx);
    }
}

void cmp_net(const std::vector<std::string>& data_vc, const std::vector<std::string>& cdc_vc, const int idx)
{
    int net_part_idx;
    int net_end = idx * 3 + 3;
    for (net_part_idx = idx * 3; net_part_idx < net_end; net_part_idx++)
    {
        cmp_net_part(data_vc, cdc_vc, net_part_idx);
    }
}

void cmp_data(const std::vector<std::string> &data_vc, const std::vector<std::string> &cdc_vc, int net_num)
{
    int idx = 0;
    uint32_t line_num_pernet = 3;
    if (data_vc.size() < net_num * 3 || cdc_vc.size() < net_num * 3) {
        std::cout << "data_vc size:" << data_vc.size() << std::endl;
        std::cout << " cdc_vc size:" << cdc_vc.size() << std::endl;

    }
    net_num = data_vc.size() < cdc_vc.size() ? data_vc.size() : cdc_vc.size();
    net_num = net_num - 3;
    net_num = net_num % line_num_pernet ? net_num / line_num_pernet + 1 : net_num / line_num_pernet;
    for (idx = 0; idx < net_num ; idx++)
    {
        cmp_net(data_vc, cdc_vc, idx);
    }
}

void update_hex(std::ostream& os, std::string& hexfile)
{
    int idx = 0;
    std::string data;
    std::vector<unsigned char> dataVal;
    HexFile hex(hexfile);
    uint32_t crc24_val;
    for (idx = 0; idx < 6; ++idx) {
        data = "";
        hex.GetMultiLineData(1 + (idx * 8), 8 + (idx * 8), data);
        os << idx << " " << data << std::endl;
        HexStrToHexVal(data, dataVal);
        crc24_val = crc24(0, dataVal.data(), dataVal.size());
        std::cout << "CRC:" << std::hex << crc24_val << std::endl;

        UpdateFileStart(data, dataVal, 0);
        UpdateFileCtx(data, dataVal, 0);
        UpdateFileEnd(data, dataVal, 0, crc24_val);
        ReadsFifoBuffer(data, 0, std::cout);
    }
}

void update_firmware_msg(std::string &HidData, const std::vector<unsigned char>& dataVal, int isTry, hid_device* handle)
{
    int res;
    int i = 1;
    int idx = 0;
    int start = 0;
    int end = 64;
    // Generate command
    HidData.clear();
    HidData.resize(65 + isTry, 0);
    if (isTry) { HidData[i++] = 0; }
    for (idx = start; idx < end && idx < dataVal.size(); idx++) {
        HidData[i++] = dataVal[idx];
    }
    res = hid_write(handle, (unsigned char*)HidData.c_str(), HidData.length());
}

void update_firmware(const std::string & binfile, std::ostream& os, hid_device* handle)
{
    int idx = 0;
    std::vector<std::string> data;
    std::vector<unsigned char> dataVal;
    int net_num = 0;
    int cmd_num = 0;
    std::fstream fs_log;
    std::string HidData;
    fs_log.open("update.log", std::ios::out| std::ios::trunc);

    test_cdc(binfile, data, net_num);


    for (idx = 1; idx <= net_num * 3; ++idx) {
        HexStrToHexVal(data[idx - 1], dataVal);
        update_firmware_msg(HidData, dataVal, 0, handle);
        if (idx%3 == 0) {
            ReadsFifoBuffer(HidData, idx, fs_log, handle);
        }
    }
    std::cout << std::dec << "net_num:" << net_num << ",data:" << data.size() << std::endl;
    for (cmd_num = idx + 3; idx < cmd_num; ++idx) {
        HexStrToHexVal(data[idx - 1], dataVal);
        update_firmware_msg(HidData, dataVal, 0, handle);
        ReadsFifoBuffer(HidData, idx, fs_log, handle);
    }
}

void update_main(const std::string& binfile, hid_device* handle)
{
    std::string HidData;
    Update001(HidData, 0, handle);
    read_requested_state(handle);
    Update002(HidData, 0, handle);
    read_requested_state(handle);
    Update003(HidData, 0, handle);
    read_requested_state(handle);
    Update004(HidData, 0, handle);
    read_requested_state(handle);
    update_firmware(binfile, std::cout, handle);
    std::cout << "update finished !" << std::endl;
}

void usage(int argc, char** argv)
{
    std::cout << "Usage:" << std::endl;
    std::cout << "\t" << argv[0] << " < hex_file_name > " << std::endl;
}

void prompt(int argc, char** argv)
{
    stcisp_usage(argc, argv);
}

void test_hex(const std::string &hexfile)
{
    int i = 0;
    const char* hex_mem = NULL;
    unsigned int bin_len = 1*1024*2024;
    char* bin_mem = (char*)malloc(bin_len);
    bin_len = load_hex_file(hexfile.c_str());
    hex_mem = get_hex_data_mem();
 
    // getData(hex_mem, bin_mem, 0, &bin_len);
    // dump_memory((uint8_t*)hex_mem, bin_len);
    dump_hex_file((uint8_t*)hex_mem, bin_len);
    free(bin_mem);
}

int stc_main(int argc, char** argv, hid_device* handle)
{
    IniFile inifile("stcisp.ini");
    std::string hexfile = "test.hex";
    std::string binfile = "test.bin";
    // BusHoundLog usb_log("log.txt");
    if (argc == 2)
    {
        hexfile = argv[1];
    }
    else
    {
        // usage(argc, argv);
        prompt(argc, argv);
        inifile.GetHexPath(IniKeyHexPath, hexfile);
    }

    HexFile hex(hexfile);
    BinFile bin(binfile);
    FileFmtConverter fmt;
    fmt.Hex2Bin(hex, bin);

    // usb_log.ParseContent("net.log", 0, 0, 0);
    set_hex_file_name(binfile);

    // test_main(binfile, freqMeter);

    // Flushes output stream buffer, after each character inserted
    std::cout.setf(std::ios_base::unitbuf);

    //    Arzawa Instrument's Frequency Meter device properties
    //    --------------------------------------------------
    //       Path        : \\?\HID#VID_0483&PID_3256&MI_00#8&6A72FA&0&0000#{4D1E55B2-F16F-11CF-88CB-001111000030}
    //       Manufacturer: Arzawa Instruments
    //       Product     : Arzawa FrMeter ARZFC1
    //       Serial      : 204EB0425642
    //       Vendor Id   : 1155
    //       Product Id  : 12886
    //       Release No  : 512
    //       Usage Page  : 255
    //       Usage       : 1
    //       Interface   : 0
    //       --------------------------------------------------

    // Scan(enumarate) devices with filter parameters
    // 0x34BF,             // vendor id

    update_main(binfile, handle);

    // main thread still running
    return 0;
}

int test_dev(int argc, char *argv[])
{
    unsigned short vendor_id = 0x34BF;
    unsigned short product_id = 0x1001;
    hid_device *handle;

    // test_all_dev();

    // Open the device using the VID, PID,
    // and optionally the Serial number.
    ////handle = hid_open(0x4d8, 0x3f, L"12345");
    handle = hid_open(vendor_id, product_id, NULL);
    if (!handle)
    {
        printf("unable to open stc device(0x%04x,0x%04x)\n", vendor_id, product_id);
        hid_exit();
        return 1;
    }

    // Set the hid_read() function to be non-blocking.
    hid_set_nonblocking(handle, 0);

    stc_main(argc, argv, handle);

    hid_close(handle);
    return 0;
}

int test_hex2bin(int argc, char *argv[])
{
    IniFile inifile("stcisp.ini");
    std::string hexfile = "test.hex";
    std::string binfile = "test.bin";

    inifile.GetHexPath(IniKeyHexPath, hexfile);

    HexFile hex(hexfile);
    BinFile bin(binfile);
    FileFmtConverter fmt;
    fmt.Hex2Bin(hex, bin);

    return 0;
}

int main(int argc, char *argv[])
{

    (void)argc;
    (void)argv;
    // test_hex2bin(argc, argv);
    linked_version(argc, argv, __FUNCTION__);
    /* --- HIDAPI R&D: this is just to force the compiler to ensure
           each of those functions are implemented (even as a stub)
           by each backend. --- */
    (void)&hid_open;
    (void)&hid_open_path;
    (void)&hid_read_timeout;
    (void)&hid_get_input_report;
#if HID_API_VERSION >= HID_API_MAKE_VERSION(0, 15, 0)
    (void)&hid_send_output_report;
#endif
    (void)&hid_get_feature_report;
    (void)&hid_send_feature_report;
#if HID_API_VERSION >= HID_API_MAKE_VERSION(0, 14, 0)
    (void)&hid_get_report_descriptor;
#endif
    /* --- */

    hid_usage();

    if (hid_init()) {
        return -1;
    }

#if defined(__APPLE__) && HID_API_VERSION >= HID_API_MAKE_VERSION(0, 12, 0)
    // To work properly needs to be called before hid_open/hid_open_path after hid_init.
    // Best/recommended option - call it right after hid_init.
    hid_darwin_set_open_exclusive(0);
#endif

    test_all_dev();

    test_dev(argc, argv);

    /* Free static HIDAPI objects. */
    hid_exit();

    return 0;
}


// ######################################### CALLBACK FUNCTIONS' DEFINITIONS BEGIN ######################################### //


// ########################################## CALLBACK FUNCTIONS' DEFINITIONS END ########################################## //










// ##################################################### EXAMPLE OUTPUT #################################################### //
//
//    HidAPi is inited? : true
//    Found device count: 1
//    1. DEVICE
//    Path        : \\?\HID#VID_0483&PID_3256&MI_00#8&6A72FA&0&0000#{4D1E55B2-F16F-11CF-88CB-001111000030}
//    Manufacturer: Arzawa Instruments
//    Product     : Arzawa FrMeter ARZFC1
//    Serial      : 204EB0425642
//    Vendor Id   : 1155
//    Product Id  : 12886
//    Release No  : 512
//    Usage Page  : 255
//    Usage       : 1
//    Interface   : 0
//    --------------------------------------------------
//    Arzawa Instrument's Frequency Meter is found
//
//    <<<<<< OPERATIONS ARE STARTED >>>>>>
//
//    open result : true
//    write result: 65
//    read result :
//    @ Device:Awa FrMeter FC-1
//    HwType:1GHz-32K
//    SeriNo:12345498
//
//    open result : true
//    write result: 65
//    read result :
//    A Sw:Awa FrMeter FC-1
//    SwType:1GHz-32K
//
//
//    write result 1: 65
//    write result 2: 65
//    write result 3: 65
//    write result 4: 65
//    write result 5: 65
//    write result 6: 65
//    write result 7: 65
//    write result 8: 65
//
//    available data count : 8
//    read result          :
//    @ Device:Awa FrMeter FC-1
//    HwType:1GHz-32K
//    SeriNo:12345498
//
//    available data count : 7
//    read result          :
//    @ Device:Awa FrMeter FC-1
//    HwType:1GHz-32K
//    SeriNo:12345498
//
//    available data count : 6
//    read result          :
//    @ Device:Awa FrMeter FC-1
//    HwType:1GHz-32K
//    SeriNo:12345498
//
//    available data count : 5
//    read result          :
//    @ Device:Awa FrMeter FC-1
//    HwType:1GHz-32K
//    SeriNo:12345498
//
//    available data count : 4
//    read result          :
//    @ Device:Awa FrMeter FC-1
//    HwType:1GHz-32K
//    SeriNo:12345498
//
//    available data count : 3
//    read result          :
//    @ Device:Awa FrMeter FC-1
//    HwType:1GHz-32K
//    SeriNo:12345498
//
//    available data count : 2
//    read result          :
//    @ Device:Awa FrMeter FC-1
//    HwType:1GHz-32K
//    SeriNo:12345498
//
//    available data count : 1
//    read result          :
//    @ Device:Awa FrMeter FC-1
//    HwType:1GHz-32K
//    SeriNo:12345498
//
//    .......
//    This device removed: \\?\HID#VID_0483&PID_3256&MI_00#8&6A72FA&0&0000#{4D1E55B2-F16F-11CF-88CB-001111000030}
//    ....
//    This device added:   \\?\HID#VID_0483&PID_3256&MI_00#8&6A72FA&0&0000#{4D1E55B2-F16F-11CF-88CB-001111000030}
//    ...............
//
// ##################################################### EXAMPLE OUTPUT #################################################### //
