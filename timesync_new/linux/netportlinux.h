#ifndef LINUXNETPORT_H
#define LINUXNETPORT_H

#ifdef __linux__

#include <deque>
#include <ifaddrs.h>
#include <linux/ethtool.h>
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <linux/net_tstamp.h>
#include <net/if.h>
#include <linux/sockios.h>
#ifndef __arm__
#include <linux/wireless.h>
#endif
#include <sys/utsname.h>

#include "types.h"
#include "platform.h"
#include "TimeMeas.h"
#include "receivepackage.h"
#include "inetport.h"
#include "ptpclocklinux.h"

class NetPortLinux : public INetPort
{
    public:

        NetPortLinux(char const* const devname);

        ~NetPortLinux();

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

        void ReceiveMessage(ReceivePackage* pRet);

        int32_t GetPtpClockIndex();

        std::string GetInterfaceName();

        PtpClock* GetPtpClock();

        NetworkCardType GetNetworkCardType();


        bool IsWireless();


        bool IsUpAndConnected();

//        int GetSpeed();

        uint8_t const* GetMAC();

        uint32_t GetRxPhyDelay();

        void SetRxPhyDelay(uint32_t delay);

        uint32_t GetTxPhyDelay();

        void SetTxPhyDelay(uint32_t delay);

protected:

    const unsigned char P8021AS_MULTICAST[6] = {0x01, 0x80, 0xC2, 0x00, 0x00, 0x0E};

    const uint16_t PTP_ETHERTYPE = 0x88F7;

    UScaledNs GetLastTxMessage(int timeout_ms = 1000);

    pal::SectionLock_t m_EventLock;

private:

    struct KernelVersion
    {
        int kernel;
        int major;
        int minor;
    };

    const char* intelVendorID = "0x8086";

    const char* i210DeviceID = "0x1533";

    const char* x540DeviceID = "0x1528";

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

    NetworkCardType m_cardType;

    uint32_t m_rxPhyDelay;

    uint32_t m_txPhyDelay;


    /**
     * @brief IsCarrierSet Checks if the parameter "/sys/class/net/<m_IfcName>/carrier" is set.
     * This seems to be the information if the link is up and the interface is connected to another end.
     * @return True if "carrier" is set, false otherwise.
     */
    bool IsCarrierSet();


    NetworkCardType ReadNetworkCardTypeFromSysFs();


    void SetDefaultPhyDelays();


    KernelVersion GetKernelVersion();
};

#endif

#endif // LINUXNETPORT_H

