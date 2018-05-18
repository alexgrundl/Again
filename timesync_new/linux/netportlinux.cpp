#ifdef __linux__

#include "netport.h"
#include "platform.h"

#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <deque>

#include <net/if.h>
#include <net/if_arp.h>
#include <linux/if_packet.h>
#include <linux/ethtool.h>
#include <linux/net_tstamp.h>

#include "TimeMeas.h"
#include "types.h"
#include "commondefs.h"
#include "ptpmessagebase.h"
#include "netportlinux.h"

NetPortLinux::NetPortLinux(char const* const devname)
{
    m_PtpClockIndex = -1;
    m_PDelay = 0;
    m_asCapable = false;
    m_neighborRatio = 1.0;
    m_EventLock = pal::LockedRegionCreate();
    m_IfcName = std::string(devname);
    m_IfcIndex = -1;
    m_EventSock = pal::SocketCreate(PF_PACKET, SOCK_DGRAM, 0);
    m_GeneralSock = pal::SocketCreate(PF_PACKET, SOCK_DGRAM, 0);
    m_ptpClock = new PtpClockLinux();
    m_cardType = ReadNetworkCardTypeFromSysFs();
}

NetPortLinux::~NetPortLinux()
{
    pal::LockedRegionDelete(m_EventLock);
    pal::SocketDelete(m_GeneralSock);
    pal::SocketDelete(m_EventSock);

    delete m_ptpClock;
}

uint64_t NetPortLinux::GetPDelay()
{
    return m_PDelay;
}

void NetPortLinux::SetPDelay(uint64_t pDelay)
{
    m_PDelay = pDelay;
}

bool NetPortLinux::GetASCapable()
{
    return m_asCapable;
}

void NetPortLinux::SetASCapable(bool asCapable)
{
    m_asCapable = asCapable;
}

double NetPortLinux::GetNeighborRatio()
{
    return m_neighborRatio;
}

void NetPortLinux::SetNeighborRatio(double ratio)
{
    m_neighborRatio = ratio;
}

bool NetPortLinux::Initialize()
{
    struct ifreq ifr;
    pal::SocketGetHwAddr(m_EventSock, m_IfcName.c_str(), m_MAC, &m_IfcIndex);

    /* find PTP clock */
    {
        struct ethtool_ts_info info;

        memset(&ifr, 0, sizeof(ifr));
        memset(&info, 0, sizeof(info));


        pal::Socket_t sock = pal::SocketCreate(AF_UNIX, SOCK_DGRAM, 0);

        info.cmd = ETHTOOL_GET_TS_INFO;
        strcpy(ifr.ifr_name, m_IfcName.c_str());
        ifr.ifr_data = (char*)&info;
        if(pal::DeviceIoCtlWrite(sock, SIOCETHTOOL, &ifr, sizeof(ifr)) == pal::Failure)
        {
            logerror("failed to contact ETHTOOL");
            return false;
        }
        else
        {
            m_PtpClockIndex = info.phc_index;
            m_ptpClock->Open(m_PtpClockIndex);
            lognotice("found clock as ptp%d", m_PtpClockIndex);
        }
    }

    /* add multicast membership */
    if(!SetRxQueueEnabled(true))
    {
      return false;
    }

    /* enable raw socket access */
    {
        struct sockaddr_ll ifsock_addr;
        memset(&ifsock_addr, 0, sizeof(ifsock_addr));
        ifsock_addr.sll_family = AF_PACKET;
        ifsock_addr.sll_ifindex = m_IfcIndex;
        ifsock_addr.sll_protocol = htons(PTP_ETHERTYPE);
        if(pal::SocketBindTo(m_EventSock, &ifsock_addr, sizeof(ifsock_addr)))
        {
            lognotice("added raw socket to port %u", m_IfcIndex);
        }
        else
        {
            logerror("failed to add raw socket to port %u", m_IfcIndex);
            return false;
        }
    }

    {
        struct hwtstamp_config hwconfig;
        /* reuse ifr from ptp clock search */
        memset(&hwconfig, 0, sizeof(hwconfig));

        hwconfig.rx_filter = HWTSTAMP_FILTER_PTP_V2_EVENT;
        hwconfig.tx_type = HWTSTAMP_TX_ON;
        ifr.ifr_data = (char*)&hwconfig;
        if(pal::DeviceIoCtlWrite(m_EventSock, SIOCSHWTSTAMP, &ifr, sizeof(ifr)) == pal::Failure)
        {
            logerror("failed to set HW Timestamping filter");
            return false;
        }
        else
        {
            lognotice("set filter for HW Timestamping on PTP%d", m_PtpClockIndex);
        }

        uint32_t tsflags = 0;
        tsflags |= SOF_TIMESTAMPING_TX_HARDWARE;
        tsflags |= SOF_TIMESTAMPING_RX_HARDWARE;
        tsflags |= SOF_TIMESTAMPING_SYS_HARDWARE;
        tsflags |= SOF_TIMESTAMPING_RAW_HARDWARE;

        if(!pal::SocketSetOption(m_EventSock, SOL_SOCKET, SO_TIMESTAMPING, &tsflags, sizeof(tsflags)))
        {
            logerror("failed to configure Timestampin on PTP%d ", m_PtpClockIndex);
            return false;
        }
        else
        {
            lognotice("configured Timestamping on PTP%d", m_PtpClockIndex);
        }

        uint32_t option = 1;
        if(!pal::SocketSetOption(m_EventSock, SOL_SOCKET, SO_SELECT_ERR_QUEUE, &option, sizeof(option)))
        {
            logerror("failed to activate error queue on PTP%d ", m_PtpClockIndex);
            return false;
        }
        else
        {
            lognotice("activated error queue on PTP%d", m_PtpClockIndex);
        }
    }
    return true;
}

bool NetPortLinux::SetRxQueueEnabled(bool settoenabled)
{
    struct packet_mreq mr_8021as;
    memset(&mr_8021as, 0, sizeof(mr_8021as));
    mr_8021as.mr_ifindex = m_IfcIndex;
    mr_8021as.mr_type = PACKET_MR_MULTICAST;
    mr_8021as.mr_alen = 6;
    memcpy(mr_8021as.mr_address, P8021AS_MULTICAST, mr_8021as.mr_alen);

    if(settoenabled)
    {
        uint8_t buf[256];
        while(pal::SocketRecvFrom(m_EventSock, buf, 256, MSG_DONTWAIT ) != -1);

        if(pal::SocketSetOption(m_EventSock, SOL_PACKET, PACKET_ADD_MEMBERSHIP, &mr_8021as, sizeof(mr_8021as)))
        {
            lognotice("added PTP multicast to port %u", m_IfcIndex);
            return true;
        }
        else
        {
            logerror("failed to add PTP multicast to port %u", m_IfcIndex);
            return false;
        }
    }
    else
    {
        if(pal::SocketSetOption(m_EventSock, SOL_PACKET, PACKET_DROP_MEMBERSHIP, &mr_8021as, sizeof(mr_8021as)))
        {
            lognotice("removed PTP multicast from port %u", m_IfcIndex);
            return true;
        }
        else
        {
            logerror("failed to remove PTP multicast from port %u", m_IfcIndex);
            return false;
        }
    }
}

uint16_t NetPortLinux::GetPortNumber()
{
    return m_IfcIndex;
}

bool NetPortLinux::SendGenericMessage(PtpMessageBase* Msg)
{
    if(IsCarrierSet())
    {
        struct sockaddr_ll remote;
        remote.sll_family = AF_PACKET;
        remote.sll_protocol = htons(PtpMessageBase::kEtherType);
        remote.sll_ifindex = m_IfcIndex;
        remote.sll_halen = ETH_ALEN;
        memcpy(remote.sll_addr, PtpMessageBase::kMacMulticast, ETH_ALEN);
        uint8_t pBuffer[100];

        Msg->GetPtpMessage(pBuffer);
        if(pal::SocketSendToRaw(m_GeneralSock, pBuffer, Msg->GetMessageLength(), &remote, sizeof(remote)) < 0)
        {

            logerror("failed to send general data frame: %d. Datasize: %u\n", errno, Msg->GetMessageLength());
            return false;
        }
    }
    else
        return false;
    return true;
}

UScaledNs NetPortLinux::SendEventMessage(PtpMessageBase* Msg)
{
    UScaledNs timestamp = {0, 0};
    if(IsCarrierSet())
    {
        pal::LockedRegionEnter(m_EventLock);
        {
            struct sockaddr_ll remote;
            remote.sll_family = AF_PACKET;
            remote.sll_protocol = htons(PtpMessageBase::kEtherType);
            remote.sll_ifindex = m_IfcIndex;
            remote.sll_halen = ETH_ALEN;
            memcpy(remote.sll_addr, PtpMessageBase::kMacMulticast, ETH_ALEN);

            uint8_t pBuffer[100];
            Msg->GetPtpMessage(pBuffer);

            if(pal::SocketSendToRaw(m_EventSock, pBuffer, Msg->GetMessageLength(), &remote, sizeof(remote)) < 0)
            {
                logerror("failed to send event data frame");
            }
            else
            {
                timestamp = GetLastTxMessage();
            }
        }
        pal::LockedRegionLeave(m_EventLock);
    }
    return timestamp;
}

void NetPortLinux::ReceiveMessage(ReceivePackage* pRet)
{
    if(IsCarrierSet())
    {
        if(pal::SocketCheck(m_EventSock, 16000) == pal::SocketReaction_e::SockReady)
        {
            ReceivePackage* pPackage = (ReceivePackage*)pRet;
            struct msghdr msg;
            struct cmsghdr *cmsg;
            struct {
            struct cmsghdr cm;
            char control[256];
            } control;
            struct sockaddr_ll remote;
            struct iovec sgentry;

            memset(&msg, 0, sizeof(msg));
            memset(&remote, 0, sizeof(remote));

            msg.msg_iov = &sgentry;
            msg.msg_iovlen = 1;
            sgentry.iov_base = (void*)pRet->GetBuffer();
            sgentry.iov_len = pRet->GetSize();
            msg.msg_name = (caddr_t)&remote;
            msg.msg_namelen = sizeof(remote);
            msg.msg_control = &control;
            msg.msg_controllen = sizeof(control);

            int32_t icnt = pal::SocketRecvMsg(m_EventSock, &msg);

            if(icnt < 0)
            {
                logwarning("failed to receive!\n");
            }
            else if(pRet->GetBuffer()[0] & 0x08)
            {
                pPackage->SetValid();
            }
            else
            {
                cmsg = CMSG_FIRSTHDR(&msg);
                while(cmsg != NULL)
                {
    //                if(cmsg->cmsg_level == SOL_SOCKET && cmsg->cmsg_type == SO_TIMESTAMPING)
    //                {
                        struct timespec* ts_system = ((struct timespec*)CMSG_DATA(cmsg)) + 1;
                        struct timespec* ts_device = ts_system + 1;
                        pPackage->SetRealSize(icnt);
                        UScaledNs timestamp;
                        timestamp.ns = (uint64_t)ts_device->tv_sec * NS_PER_SEC + ts_device->tv_nsec;
                        timestamp.ns -= GetRxLinkDelay_ns();
                        pPackage->SetTimestamp(timestamp);
                        //pRet->GetTimestamp(false)->Correct(-(int32_t)GetRxLinkDelay_ns());
                        pPackage->SetValid();
                        break;
    //                }
                    cmsg = CMSG_NXTHDR(&msg,cmsg);
                }

                if(!pPackage->IsValid())
                {
                    printf("Message type: %i\n", pRet->GetBuffer()[0] & 0x0F);
                }
            }
        }
    }
}

void NetPortLinux::PushRxTime(UScaledNs& ts)
{
    m_Timestamps.push_front(ts);
}

int32_t NetPortLinux::GetPtpClockIndex()
{
    return m_PtpClockIndex;
}

std::string NetPortLinux::GetInterfaceName()
{
    return m_IfcName;
}

uint8_t const* NetPortLinux::GetMAC()
{
    return (uint8_t*)m_MAC;
}

uint32_t NetPortLinux::GetRxLinkDelay_ns()
{
    return m_cardType != NETWORK_CARD_TYPE_X540 ? 0 : 750;
}
uint32_t NetPortLinux::GetTxLinkDelay_ns()
{
    return m_cardType != NETWORK_CARD_TYPE_X540 ? 0 : 750;
}

PtpClock* NetPortLinux::GetPtpClock()
{
    return m_ptpClock;
}

UScaledNs NetPortLinux::GetLastTxMessage(int timeout_ms)
{
    UScaledNs timestamp = {0, 0};
    CTimeMeas ct;
    bool tsGotten = false;

    do
    {

        if(pal::SocketCheck(m_EventSock, 10000) == pal::SocketReaction_e::SockReady)
        {
            struct msghdr msg;
            struct cmsghdr *cmsg;
            struct {
            struct cmsghdr cm;
            char control[256];
            } control;
            struct sockaddr_ll remote;
            struct iovec sgentry;

            memset(&msg, 0, sizeof(msg));
            memset(&remote, 0, sizeof(remote));

            msg.msg_iov = &sgentry;
            msg.msg_iovlen = 1;
            sgentry.iov_base = NULL;
            sgentry.iov_len = 0;

            msg.msg_name = (caddr_t)&remote;
            msg.msg_namelen = sizeof(remote);
            msg.msg_control = &control;
            msg.msg_controllen = sizeof(control);

            int err = pal::SocketRecvMsg(m_EventSock, &msg, MSG_ERRQUEUE);
            if(err < 0)
            {
                tsGotten = false;
            }
            else
            {
                cmsg = CMSG_FIRSTHDR(&msg);
                while(cmsg != NULL)
                {
                    if(cmsg->cmsg_level == SOL_SOCKET && cmsg->cmsg_type == SO_TIMESTAMPING)
                    {
                        struct timespec* ts_system = ((struct timespec*)CMSG_DATA(cmsg))+1;
                        struct timespec* ts_device = ts_system + 1;

                        timestamp.ns = (uint64_t)ts_device->tv_sec * NS_PER_SEC + ts_device->tv_nsec;
                        timestamp.ns += GetTxLinkDelay_ns();
                        tsGotten = true;
                        //pRet->Correct(GetTxLinkDelay_ns());
                        break;
                    }
                    cmsg = CMSG_NXTHDR(&msg,cmsg);
                }
            }
        }
    if(tsGotten)
        break;
    }
    while(ct.GetCurrentTimeMeas(CTimeMeas::TimeBaseMili) < timeout_ms );
    return timestamp;
}

bool NetPortLinux::IsCarrierSet()
{
    int nRead = 0;
    int fdCarrier = open((std::string("/sys/class/net/") + std::string(m_IfcName) + std::string("/carrier")).c_str(), O_RDONLY);
    char val[10] = {0};

    if(fdCarrier != -1)
    {
        nRead = read(fdCarrier, val, sizeof(val));
        close(fdCarrier);
    }

    return nRead > 0 && val[0] == '1';
}

NetworkCardType NetPortLinux::GetNetworkCardType()
{
    return m_cardType;
}

NetworkCardType NetPortLinux::ReadNetworkCardTypeFromSysFs()
{
    NetworkCardType cardType = NETWORK_CARD_TYPE_UNKNOWN;
    char bufRead[100] = {0};
    std::string deviceIDPath = std::string("/sys/class/net/") + m_IfcName + std::string("/device/device");
    std::string vendorIDPath = std::string("/sys/class/net/") + m_IfcName + std::string("/device/vendor");
    int fdVendorID = open(vendorIDPath.c_str(), O_RDONLY);

    int fdDeviceID;

    if(fdVendorID != -1)
    {
        if(read(fdVendorID, bufRead, sizeof(bufRead)) > 0)
        {
            if(memcmp(bufRead, intelVendorID, strlen(intelVendorID)) == 0)
            {
                fdDeviceID = open(deviceIDPath.c_str(), O_RDONLY);
                if(fdDeviceID != -1)
                {
                    if(read(fdDeviceID, bufRead, sizeof(bufRead)) > 0)
                    {
                        if(memcmp(bufRead, i210DeviceID, strlen(i210DeviceID)) == 0)
                            cardType = NETWORK_CARD_TYPE_I210;
                        else if(memcmp(bufRead, x540DeviceID, strlen(x540DeviceID)) == 0)
                            cardType = NETWORK_CARD_TYPE_X540;
                    }
                    else
                        logerror("Read of path %s failed.", deviceIDPath.c_str());

                    close(fdDeviceID);
                }
                else
                    logerror("Couldn't open path %s.", deviceIDPath.c_str());
            }
        }
        else
            logerror("Read of path %s failed.", vendorIDPath.c_str());

        close(fdVendorID);
    }
    else
        logerror("Couldn't open path %s.", vendorIDPath.c_str());


    return cardType;
}

bool NetPortLinux::IsWireless()
{
      int sock = -1;
      struct iwreq pwrq;
      memset(&pwrq, 0, sizeof(pwrq));
      strncpy(pwrq.ifr_name, m_IfcName.c_str(), IFNAMSIZ);

      if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
      {
        perror("socket");
        return false;
      }

      if (ioctl(sock, SIOCGIWNAME, &pwrq) != -1)
      {
        close(sock);
        return true;
      }

      close(sock);

      return false;
}

//int LinuxNetPort::GetSpeed()
//{
//    struct ifreq ifr;
//    struct ethtool_cmd edata;
//    int rc;

//    strncpy(ifr.ifr_name, m_IfcName.c_str(), sizeof(ifr.ifr_name));
//    ifr.ifr_data = (char*)&edata;

//    edata.cmd = ETHTOOL_GSET;

//    // issue ioctl
//    rc = ioctl(pal::GetRawSocket(m_GeneralSock), SIOCETHTOOL, &ifr);
//    if (rc < 0)
//    {
//        logerror("Get speed of interface %s failed.", ifr.ifr_name);
//        return 0;
//    }

//    return edata.speed;
//}

#endif
