#ifndef INETPORT_H
#define INETPORT_H

#include <stdint.h>

#include "ptpmessagebase.h"
#include "receivepackage.h"

class INetPort
{
  public:
    virtual bool Initialize() = 0;
    virtual bool SetRxQueueEnabled(bool settoenabled) = 0;
    virtual bool SendGenericMessage(PtpMessageBase* Msg) = 0;
    virtual uint16_t GetPortNumber() = 0;
    virtual UScaledNs SendEventMessage(PtpMessageBase* Msg) = 0;
    virtual void ReceiveMessage(ReceivePackage* package) = 0;
    virtual uint8_t const* GetMAC() = 0;
    virtual uint32_t GetRxLinkDelay_ns() = 0;
    virtual uint32_t GetTxLinkDelay_ns() = 0;
    virtual uint64_t GetPDelay() = 0;
    virtual void SetPDelay(uint64_t pDelay) = 0;
    virtual double GetNeighborRatio() = 0;
    virtual void SetNeighborRatio(double ratio) = 0;
    virtual bool GetASCapable() = 0;
    virtual void SetASCapable(bool asCapable) = 0;
    virtual ~INetPort(){}
  protected:
    INetPort(){}
};

#endif //INETPORT_H
