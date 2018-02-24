#ifndef PTPMESSAGEPDELAYRESP_H
#define PTPMESSAGEPDELAYRESP_H

#include "ptpmessagebase.h"

class PtpMessagePDelayResp : public PtpMessageBase
{

public:


    PtpMessagePDelayResp();


    void GetMessage(uint8_t* bytes);


    PortIdentity* GetRequestingPortIdentity();


    Timestamp GetRequestReceiptTimestamp();


protected:

    Timestamp m_requestReceiptTimestamp;


    PortIdentity m_requestingPortIdentity;
};

#endif // PTPMESSAGEPDELAYRESP_H
