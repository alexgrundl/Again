#ifndef PTPMESSAGEPDELAYREQ_H
#define PTPMESSAGEPDELAYREQ_H

#include "ptpmessagebase.h"

class PtpMessagePDelayReq : public PtpMessageBase
{

public:

    PtpMessagePDelayReq();


    void GetMessage(uint8_t* bytes);


    void ParsePackage(const uint8_t* bytes);
};

#endif // PTPMESSAGEPDELAYREQ_H
