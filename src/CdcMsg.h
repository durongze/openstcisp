#ifndef CDC_MSG_H
#define CDC_MSG_H

#include <stdint.h>
#include <string>

#define CDC_MSG_CMD_HDR_A  "46b9"
#define CDC_MSG_CMD_HDR_B  "6a00"
#define CDC_MSG_CMD_HDR   "46b96a00"
#define CDC_MSG_CTX_LEN "8b"
#define CDC_MSG_CMD_VAL "02"

#define CDC_MSG_HEAD     CDC_MSG_CMD_HDR CDC_MSG_CTX_LEN CDC_MSG_CMD_VAL

#define CDC_MSG_START_HEAD "5aa5"

#define CDC_MSG_START_LEN  54
#define CDC_MSG_START_STR_LEN  ((CDC_MSG_START_LEN) * 2)

#define CDC_MSG_MID_LEN  64
#define CDC_MSG_MID_STR_LEN    ((CDC_MSG_MID_LEN) * 2)

#define CDC_MSG_END_LEN        ((CDC_MSG_MID_LEN) - (CDC_MSG_START_LEN))
#define CDC_MSG_END_STR_LEN    ((CDC_MSG_END_LEN) * 2)

#define CDC_MSG_TAIL "46b96a003304"

class CdcMsg {
public:
	CdcMsg();
	~CdcMsg();
	int SetHexCtx(const std::string & hexStr128, short msgIdx, int msgCrc24);
	int GenNetMsgHdr(std::string& msgHead);
	int GenNetMsgPayload(std::string& msgHead);
	int GenNetMsg(std::string& msgStart, std::string& msgMid, std::string& msgEnd);
private:
	int GenNetMsgStart(std::string& msgStart);
	int GenNetMsgMid(std::string& msgMid);
	int GenNetMsgEnd(std::string& msgEnd);
private:
	std::string m_hexStr128;
	short m_msgIdx;
	int m_msgCrc24;
};

#endif

