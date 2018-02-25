#include "ptpmessagesync.h"

PtpMessageSync::PtpMessageSync()
{
    m_messageLength = 44;
    m_messageType = PTP_MESSSAGE_TYPE_SYNC;
    m_flags |= (1 << FLAG_TWO_STEP);
    m_control = 0;
}

PtpMessageSync::~PtpMessageSync()
{

}

void PtpMessageSync::GetMessage(uint8_t *bytes)
{
    GetHeader(bytes);
//    for(int i = 0; i < 20; i++)
//        bytes[kMessageHeaderLength + i] = 0;
}