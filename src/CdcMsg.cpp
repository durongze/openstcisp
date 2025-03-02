#include "CdcMsg.h"

#include <strstream>
#include <iomanip>
#include <iostream>

#include "Crc24.h"
#include "HexFile.h"

CdcMsg::CdcMsg() {

}

CdcMsg::~CdcMsg() {

}

int CdcMsg::SetHexCtx(const std::string& hexStr128, short msgIdx, int msgCrc24)
{
	m_hexStr128 = hexStr128;
	m_msgIdx = msgIdx;

	std::vector<unsigned char> dataVal;
	std::string payload;
	GenNetMsgPayload(payload);
	HexStrToHexVal(payload, dataVal);
	msgCrc24 = sum16(dataVal.data(), dataVal.size());
	msgCrc24 = msgCrc24 << 8 | 0x16;
	m_msgCrc24 = msgCrc24;
	return 0;
}

int CdcMsg::GenNetMsgHdr(std::string& msgHead)
{
	std::strstream ostr;
	ostr << CDC_MSG_CMD_HDR_B;
	ostr << CDC_MSG_CTX_LEN;
	ostr << CDC_MSG_CMD_VAL;
	ostr << std::uppercase << std::setfill('0') << std::setw(4) << std::hex << m_msgIdx << std::nounitbuf;
	ostr << std::uppercase << CDC_MSG_START_HEAD;
	ostr << std::ends;
	msgHead = ostr.str();
	return 0;
}

int CdcMsg::GenNetMsgPayload(std::string& payload)
{
	std::strstream ostr;
	std::string msgHead;
	GenNetMsgHdr(msgHead);
	ostr << std::uppercase << msgHead;
	ostr << std::uppercase << m_hexStr128;
	ostr << std::ends;
	payload = ostr.str();
	return 0;
}

int CdcMsg::GenNetMsg(std::string& msgStart, std::string& msgMid, std::string& msgEnd)
{
	GenNetMsgStart(msgStart);
	GenNetMsgMid(msgMid);
	GenNetMsgEnd(msgEnd);
	return 0;
}

int CdcMsg::GenNetMsgStart(std::string& msgStart)
{
	std::strstream ostr;
	ostr << CDC_MSG_CMD_HDR_A;
	std::string msgHead;
	GenNetMsgHdr(msgHead);
	ostr << std::uppercase << msgHead;
	ostr << std::nouppercase << m_hexStr128.substr(0, CDC_MSG_START_STR_LEN);
	ostr << std::ends;
	msgStart = ostr.str();
	return 0;
}

int CdcMsg::GenNetMsgMid(std::string& msgMid)
{
	std::strstream ostr;
	ostr << std::nouppercase << m_hexStr128.substr(CDC_MSG_START_STR_LEN, CDC_MSG_MID_STR_LEN);
	ostr << std::ends;
	msgMid = ostr.str();
	return 0;
}

int CdcMsg::GenNetMsgEnd(std::string& msgEnd)
{
	std::strstream ostr;
	int less_len = 0;
	if (m_hexStr128.length() < CDC_MSG_START_STR_LEN + CDC_MSG_MID_STR_LEN + CDC_MSG_END_STR_LEN) {
		less_len = m_hexStr128.length() - (CDC_MSG_START_STR_LEN + CDC_MSG_MID_STR_LEN);
		if (less_len <= 0) {
			msgEnd = "";
			return 1;
		}
	}
	else {
		less_len = CDC_MSG_END_STR_LEN;
	}
	ostr << std::nouppercase << m_hexStr128.substr(CDC_MSG_START_STR_LEN + CDC_MSG_MID_STR_LEN, less_len);
	ostr << std::nouppercase << std::setfill('0') << std::setw(6) << std::hex << m_msgCrc24 << std::nounitbuf;
	ostr << std::ends;
	msgEnd = ostr.str();
	while (msgEnd.length() < 128) {
		msgEnd.append("00");
	}
	return 0;
}