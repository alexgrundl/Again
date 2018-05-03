#ifndef LINUX_NETPORT_H
#define LINUX_NETPORT_H

#ifdef __linux__

#include <deque>
#include <ifaddrs.h>

#include "inetport.h"
#include "types.h"
#include "platform.h"
#include "ptpclock.h"

class LinuxNetPort : public INetPort
{
    public:

        LinuxNetPort(char const* const devname);

        ~LinuxNetPort();

        uint64_t GetPDelay();

        void SetPDelay(uint64_t pDelay);

        bool GetASCapable();

        void SetASCapable(bool asCapable);

        double GetNeighborRatio();

        void SetNeighborRatio(double ratio);

        bool Initialize();

        bool SetRxQueueEnabled(bool settoenabled);

        uint16_t GetPortNumber();

        bool SendGenericMessage(PtpMessageBase* Msg);

        UScaledNs SendEventMessage(PtpMessageBase* Msg);

        void ReceiveMessage(IReceivePackage* pRet);

        virtual void PushRxTime(UScaledNs& ts);

        int32_t GetPtpClockIndex();

        std::string GetInterfaceName();

        PtpClock* GetPtpClock();

        virtual uint8_t const* GetMAC();

        virtual uint32_t GetRxLinkDelay_ns();

        virtual uint32_t GetTxLinkDelay_ns();

        static const uint32_t SIOCETHTOOL = 0x8946;
        static const uint32_t SIOCSHWTSTAMP	= 0x89b0;

protected:

    const unsigned char P8021AS_MULTICAST[6] = {0x01, 0x80, 0xC2, 0x00, 0x00, 0x0E};

    const uint16_t PTP_ETHERTYPE = 0x88F7;

    UScaledNs GetLastTxMessage(int timeout_ms = 1000);

    pal::SectionLock_t m_EventLock;

private:
    pal::Socket_t m_EventSock;

    pal::Socket_t m_GeneralSock;

    std::deque<UScaledNs> m_Timestamps;

    int32_t m_PtpClockIndex;

    std::string m_IfcName;

    char m_MAC[6];

    int m_IfcIndex;

    uint64_t m_PDelay;

    PtpClock* m_ptpClock;

    bool m_asCapable;

    double m_neighborRatio;

    /**
     * @brief IsCarrierSet Checks if the parameter "/sys/class/net/<m_IfcName>/carrier" is set.
     * This seems to be the information if the link is up and the interface is connected to another end.
     * @return True if "carrier" is set, false otherwise.
     */
    bool IsCarrierSet();
};

#endif

#endif // LINUX_NETPORT_H

