#ifndef PTPMESSAGESYNC_H
#define PTPMESSAGESYNC_H

#include "ptpmessagebase.h"

class PtpMessageSync : public PtpMessageBase
{
public:

    PtpMessageSync();


    virtual ~PtpMessageSync();


    void GetMessage(uint8_t* bytes);


    void ParsePackage(const uint8_t* bytes);

private:


};

#endif // PTPMESSAGESYNC_H
