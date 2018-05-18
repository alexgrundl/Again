#ifndef WINDOWSNETPORT_H
#define WINDOWSNETPORT_H

#ifndef __linux__

#include <deque>

#include "inetport.h"
#include "types.h"
#include "platform.h"
#include "ptpclock.h"

class NetPortWindows : public INetPort
{
public:

    NetPortWindows();

    ~NetPortWindows();

    bool Initialize() {return true;}
    bool SetRxQueueEnabled(bool settoenabled) {return true;}
    bool SendGenericMessage(PtpMessageBase* Msg) {return true;}
    uint16_t GetPortNumber() {return 0;}
    UScaledNs SendEventMessage(PtpMessageBase* Msg) {return {0, 0};}
    void ReceiveMessage(ReceivePackage* package) {}
    uint8_t const* GetMAC() {return 0;}
    uint32_t GetRxLinkDelay_ns(){return 0;}
    uint32_t GetTxLinkDelay_ns() {return 0;}
    uint64_t GetPDelay() {return 0;}
    void SetPDelay(uint64_t pDelay) {}
    double GetNeighborRatio() {return 1.0;}
    void SetNeighborRatio(double ratio) {}
    bool GetASCapable() {return true;}
    void SetASCapable(bool asCapable) {}
    PtpClock* GetPtpClock() {return m_ptpClock;}
    NetworkCardType GetNetworkCardType() {return NETWORK_CARD_TYPE_UNKNOWN;}
    bool IsWireless() {return false;}
//    int GetSpeed() {return -1;}

private:

    PtpClock* m_ptpClock;
};

#endif

#endif // WINDOWSNETPORT_H
