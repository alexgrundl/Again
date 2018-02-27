#ifndef PTPMESSAGEPDELAYRESP_H
#define PTPMESSAGEPDELAYRESP_H

#include "ptpmessagebase.h"

class PtpMessagePDelayResp : public PtpMessageBase
{

public:


    PtpMessagePDelayResp();


    void GetMessage(uint8_t* bytes);


    PortIdentity GetRequestingPortIdentity();


    void SetRequestingPortIdentity(PortIdentity port);


    Timestamp GetRequestReceiptTimestamp();


    void SetRequestReceiptTimestamp(Timestamp timestamp);


    void ParsePackage(const uint8_t* bytes);


protected:

    Timestamp m_requestReceiptTimestamp;


    PortIdentity m_requestingPortIdentity;
};

#endif // PTPMESSAGEPDELAYRESP_H
