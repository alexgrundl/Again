#include "mdpdelayreq.h"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <linux/if_packet.h>
#include "ptpmessage/ptpmessagesync.h"
#include "ptpmessage/ptpmessagefollowup.h"

MDPdelayReq::MDPdelayReq(TimeAwareSystem* timeAwareSystem, PortGlobal* port, INetworkInterfacePort* networkPort) :
    StateMachineBaseMD(timeAwareSystem, port, networkPort)
{
    m_pdelayIntervalTimer.ns = 0;
    m_pdelayIntervalTimer.ns_frac = 0;
    m_rcvdPdelayResp = false;
    m_rcvdPdelayRespPtr = NULL;
    m_rcvdPdelayRespFollowUp = false;
    m_rcvdPdelayRespFollowUpPtr = NULL;
    m_txPdelayReqPtr = NULL;
    m_rcvdMDTimestampReceive = false;
    m_pdelayReqSequenceId = 0;
    m_initPdelayRespReceived = false;
    m_lostResponses = 0;
    m_neighborRateRatioValid = false;

    m_pdelayRespTime.ns = 0;
    m_pdelayRespTime.ns_frac = 0;
    m_pdelayRespFollowUpCorrTime.ns = 0;
    m_pdelayRespFollowUpCorrTime.ns_frac = 0;
}

MDPdelayReq::~MDPdelayReq()
{
    delete m_txPdelayReqPtr;
    delete m_rcvdPdelayRespPtr;
    delete m_rcvdPdelayRespFollowUpPtr;
}

PtpMessagePDelayReq* MDPdelayReq::SetPdelayReq()
{
    PtpMessagePDelayReq* pdelayReqPtr = new PtpMessagePDelayReq();

    PortIdentity identity;
    identity.portNumber = m_portGlobal->thisPort;

    PtpMessageBase::GetClockIdentity(m_networkPort->GetMAC(), identity.clockIdentity);
    pdelayReqPtr->SetSourcePortIdentity(&identity);
    /* 1) sourcePortIdentity is set equal to the port identity of the port corresponding to this MD entity
        (see 8.5.2),

       3) remaining parameters are set as specified in 11.4.2 and 11.4.5. */

    pdelayReqPtr->SetSequenceID(m_pdelayReqSequenceId);

    return pdelayReqPtr;
}

void MDPdelayReq::TxPdelayReq(PtpMessagePDelayReq* txPdelayReqPtr)
{
    /* transmits a Pdelay_Req message from the MD entity, containing the parameters in the structure pointed to by txPdelayReqPtr. */
//    uint8_t sendbuf[1024];
//    int sockfd;
//    int messageLength;
//    struct ether_header *ethheader = (struct ether_header *) sendbuf;

//    sockfd = socket(AF_PACKET, SOCK_RAW, IPPROTO_RAW);

//    struct ifreq if_mac;
//    memset(&if_mac, 0, sizeof(struct ifreq));
//    strncpy(if_mac.ifr_name, "enx74da384a1b56", IFNAMSIZ-1);
//    if (ioctl(sockfd, SIOCGIFHWADDR, &if_mac) < 0)
//        perror("SIOCGIFHWADDR");

//    ethheader->ether_shost[0] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[0];
//    ethheader->ether_shost[1] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[1];
//    ethheader->ether_shost[2] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[2];
//    ethheader->ether_shost[3] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[3];
//    ethheader->ether_shost[4] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[4];
//    ethheader->ether_shost[5] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[5];

//    ethheader->ether_dhost[0] = 0x01;
//    ethheader->ether_dhost[1] = 0x80;
//    ethheader->ether_dhost[2] = 0xC2;
//    ethheader->ether_dhost[3] = 0x00;
//    ethheader->ether_dhost[4] = 0x00;
//    ethheader->ether_dhost[5] = 0x0E;

//    ethheader->ether_type = htons(0x88F7);

//    txPdelayReqPtr->GetMessage(sendbuf + sizeof(struct ether_header));
//    messageLength = txPdelayReqPtr->GetMessageLength();

//    struct ifreq if_idx;
//    memset(&if_idx, 0, sizeof(struct ifreq));
//    strncpy(if_idx.ifr_name, "enx74da384a1b56", IFNAMSIZ-1);
//    if (ioctl(sockfd, SIOCGIFINDEX, &if_idx) < 0)
//        perror("SIOCGIFINDEX");

//    /* Destination address */
//    struct sockaddr_ll socket_address;
//    /* Index of the network device */
//    socket_address.sll_ifindex = if_idx.ifr_ifindex;
//    /* Address length*/
//    socket_address.sll_halen = ETH_ALEN;
//    /* Destination MAC */
//    socket_address.sll_addr[0] = 0x01;
//    socket_address.sll_addr[1] = 0x80;
//    socket_address.sll_addr[2] = 0xC2;
//    socket_address.sll_addr[3] = 0x00;
//    socket_address.sll_addr[4] = 0x00;
//    socket_address.sll_addr[5] = 0x0E;

//    UScaledNs now = m_timeAwareSystem->GetCurrentTime();
////    PtpMessageFollowUp resp;
////    resp.SetRequestReceiptTimestamp(now);
////    resp.GetMessage(sendbuf + sizeof(struct ether_header));
////    messageLength = resp.GetMessageLength();

//    if (sendto(sockfd, sendbuf, messageLength + sizeof(struct ether_header), 0,
//               (struct sockaddr*)&socket_address, sizeof(struct sockaddr_ll)) < 0)
//        printf("Send failed\n");

    /* Remove in good code... */
    //txPdelayReqPtr->SetSendTime(m_timeAwareSystem->GetCurrentTime());
    /* End remove */

    txPdelayReqPtr->SetSendTime(m_networkPort->SendEventMessage(txPdelayReqPtr));

    /* Remove in good code... */
    //m_txTimestamp = txPdelayReqPtr->GetSendTime();
    /* End remove */

    if(txPdelayReqPtr->GetSendTime().ns > 0)
        m_rcvdMDTimestampReceive = true;
}

double MDPdelayReq::ComputePdelayRateRatio()
{
    double pdelayRateRatio = 1.0;
    UScaledNs currentResponderEventTimeCorrected;

    currentResponderEventTimeCorrected.ns = m_rcvdPdelayRespFollowUpPtr->GetRequestReceiptTimestamp().sec * NS_PER_SEC +
            m_rcvdPdelayRespFollowUpPtr->GetRequestReceiptTimestamp().ns;
    currentResponderEventTimeCorrected.ns_frac = m_rcvdPdelayRespFollowUpPtr->GetCorrectionField();
    if(m_pdelayRespTime.ns > 0 && m_pdelayRespFollowUpCorrTime.ns > 0)
    {
        m_portGlobal->neighborRateRatio = (currentResponderEventTimeCorrected - m_pdelayRespFollowUpCorrTime) /
                (m_rcvdPdelayRespPtr->GetReceiveTime() - m_pdelayRespTime);
        m_neighborRateRatioValid = true;

        printf("m_portGlobal->neighborRateRatio: %0.7f\n", m_portGlobal->neighborRateRatio);
    }
    else
        m_neighborRateRatioValid = false;

    m_pdelayRespTime = m_rcvdPdelayRespPtr->GetReceiveTime();
    m_pdelayRespFollowUpCorrTime = currentResponderEventTimeCorrected;

    return pdelayRateRatio;
}

UScaledNs MDPdelayReq::ComputePropTime()
{
     UScaledNs propTime;
     Timestamp tsPropTime;
     Timestamp t1, t2, t3, t4;

     t1 = m_txPdelayReqPtr->GetSendTime();
     t2 = m_rcvdPdelayRespPtr->GetRequestReceiptTimestamp();
     t3 = m_rcvdPdelayRespFollowUpPtr->GetRequestReceiptTimestamp();
     t4 = m_rcvdPdelayRespPtr->GetReceiveTime();

     tsPropTime = ((t4 - t1) - (t3 - t2)) / 2;
     if(m_neighborRateRatioValid)
         tsPropTime *= m_portGlobal->neighborRateRatio;

     propTime.ns = tsPropTime.sec * NS_PER_SEC + tsPropTime.ns;
     propTime.ns_frac = 0;

     printf("PropTime: %llu\n", propTime.ns);

     return propTime;
}

void MDPdelayReq::ProcessState()
{
    if(m_timeAwareSystem->BEGIN || !m_portGlobal->portEnabled || !m_portGlobal->pttPortEnabled)
        m_state = STATE_NOT_ENABLED;
    else
    {
        switch(m_state)
        {
        case STATE_NOT_ENABLED:
            if(m_portGlobal->portEnabled && m_portGlobal->pttPortEnabled)
            {
                m_initPdelayRespReceived = false;
                //Hmmmmmmmmmmmmmmmmmmmmm.....
                /* m_pdelayRateRatio = 1.0; */
                m_rcvdMDTimestampReceive = false;
                srand(time(NULL));
                m_pdelayReqSequenceId = rand() % 65536;
                delete m_txPdelayReqPtr;
                m_txPdelayReqPtr = SetPdelayReq();
                TxPdelayReq(m_txPdelayReqPtr);
                m_pdelayIntervalTimer = m_timeAwareSystem->GetCurrentTime();
                m_lostResponses = 0;
                m_portGlobal->isMeasuringDelay = false;
                m_portGlobal->asCapable = false;
                m_state = STATE_INITIAL_SEND_PDELAY_REQ;
            }
            break;

        case STATE_INITIAL_SEND_PDELAY_REQ:
        case STATE_SEND_PDELAY_REQ:
            if(m_rcvdMDTimestampReceive)
            {
                m_rcvdMDTimestampReceive = false;
//                NS_ReceiveMessage(false);
                m_state = STATE_WAITING_FOR_PDELAY_RESP;
            }
            break;

        case STATE_WAITING_FOR_PDELAY_RESP:
            uint8_t portClockIdentity[8];
            PtpMessageBase::GetClockIdentity(m_networkPort->GetMAC(), portClockIdentity);
            if(m_rcvdPdelayResp && (m_rcvdPdelayRespPtr->GetSequenceID() == m_txPdelayReqPtr->GetSequenceID())
                    && memcmp(m_rcvdPdelayRespPtr->GetRequestingPortIdentity().clockIdentity, portClockIdentity, sizeof(portClockIdentity)) == 0 &&
                    (m_rcvdPdelayRespPtr->GetRequestingPortIdentity().portNumber == m_portGlobal->thisPort))
            {
                m_rcvdPdelayResp = false;
                m_state = STATE_WAITING_FOR_PDELAY_RESP_FOLLOW_UP;
            }
            else if((m_timeAwareSystem->GetCurrentTime() - m_pdelayIntervalTimer >= m_portGlobal->pdelayReqInterval) ||
                    (m_rcvdPdelayResp &&
                    (memcmp(m_rcvdPdelayRespPtr->GetRequestingPortIdentity().clockIdentity, portClockIdentity, sizeof(portClockIdentity)) != 0 ||
                    (m_rcvdPdelayRespPtr->GetRequestingPortIdentity().portNumber != m_portGlobal->thisPort) ||
                    (m_rcvdPdelayRespPtr->GetSequenceID() != m_txPdelayReqPtr->GetSequenceID()))))
            {
                ExecuteResetState();
                m_state = STATE_RESET;
            }
            break;

        case STATE_WAITING_FOR_PDELAY_RESP_FOLLOW_UP:
            if(m_rcvdPdelayRespFollowUp && m_rcvdPdelayRespFollowUpPtr->GetSequenceID() == m_txPdelayReqPtr->GetSequenceID() &&
                    memcmp(m_rcvdPdelayRespFollowUpPtr->GetSourcePortIdentity().clockIdentity, m_rcvdPdelayRespPtr->GetSourcePortIdentity().clockIdentity,
                           sizeof(m_rcvdPdelayRespPtr->GetSourcePortIdentity().clockIdentity)) == 0 &&
                    m_rcvdPdelayRespFollowUpPtr->GetSourcePortIdentity().portNumber == m_rcvdPdelayRespPtr->GetSourcePortIdentity().portNumber)
            {
                m_rcvdPdelayRespFollowUp = false;
                if (m_portGlobal->computeNeighborRateRatio)
                    m_portGlobal->neighborRateRatio = ComputePdelayRateRatio();
                if (m_portGlobal->computeNeighborPropDelay)
                    m_portGlobal->neighborPropDelay = ComputePropTime();
                m_lostResponses = 0;
                m_portGlobal->isMeasuringDelay = true;
                if ((m_portGlobal->neighborPropDelay <= m_portGlobal->neighborPropDelayThresh) &&
                (m_rcvdPdelayRespPtr->GetSourcePortIdentity().clockIdentity != m_timeAwareSystem->thisClock) && m_neighborRateRatioValid)
                    m_portGlobal->asCapable = true;
                else
                    m_portGlobal->asCapable = false;
                m_state = STATE_WAITING_FOR_PDELAY_INTERVAL_TIMER;
            }
            else if((m_timeAwareSystem->GetCurrentTime() - m_pdelayIntervalTimer >= m_portGlobal->pdelayReqInterval) ||
                    (m_rcvdPdelayResp && (m_rcvdPdelayRespPtr->GetSequenceID() == m_txPdelayReqPtr->GetSequenceID())))
            {
                ExecuteResetState();
                m_state = STATE_RESET;
            }
            break;

        case STATE_WAITING_FOR_PDELAY_INTERVAL_TIMER:
            if(m_timeAwareSystem->GetCurrentTime() - m_pdelayIntervalTimer >= m_portGlobal->pdelayReqInterval)
            {
                ExecuteSendPDelayReqState();
                m_state = STATE_SEND_PDELAY_REQ;
            }
            break;

        case STATE_RESET:
            ExecuteSendPDelayReqState();
            m_state = STATE_SEND_PDELAY_REQ;
            break;

        default:
            fprintf(stderr, "State not allowed: %i, File: %s\n", m_state, __FILE__);
            break;
        }
    }
}

void MDPdelayReq::ExecuteResetState()
{
    m_initPdelayRespReceived = false;
    if (m_lostResponses <= m_portGlobal->allowedLostResponses)
    m_lostResponses += 1;
    else
    {
        m_portGlobal->isMeasuringDelay = false;
        m_portGlobal->asCapable = false;
    }
}

void MDPdelayReq::ExecuteSendPDelayReqState()
{
    m_pdelayReqSequenceId += 1;
    delete m_txPdelayReqPtr;
    m_txPdelayReqPtr = SetPdelayReq();
    TxPdelayReq(m_txPdelayReqPtr);
    m_pdelayIntervalTimer = m_timeAwareSystem->GetCurrentTime();
}


void MDPdelayReq::SetPDelayResponse(IReceivePackage* package)
{
    delete m_rcvdPdelayRespPtr;
    m_rcvdPdelayRespPtr = new PtpMessagePDelayResp();
    m_rcvdPdelayRespPtr->ParsePackage(package->GetBuffer());
    m_rcvdPdelayRespPtr->SetReceiveTime(package->GetTimestamp());
    m_rcvdPdelayResp = true;
}

void MDPdelayReq::SetPDelayResponseFollowUp(IReceivePackage* package)
{
    delete m_rcvdPdelayRespFollowUpPtr;
    m_rcvdPdelayRespFollowUpPtr = new PtpMessagePDelayRespFollowUp();
    m_rcvdPdelayRespFollowUpPtr->ParsePackage(package->GetBuffer());
    m_rcvdPdelayRespFollowUpPtr->SetReceiveTime(package->GetTimestamp());
    m_rcvdPdelayRespFollowUp = true;
}

//void MDPdelayReq::NS_ReceiveMessage(bool followUp)
//{
//    CLinuxReceivePackage package(128);
//    m_networkPort->ReceiveMessage(&package);
//    package.SetValid();
//    if(package.IsValid() > 0)
//    {
//        if(!followUp)
//        {
//            m_rcvdPdelayResp = true;
//            delete m_rcvdPdelayRespPtr;
//            m_rcvdPdelayRespPtr = new PtpMessagePDelayResp();
//            m_rcvdPdelayRespPtr->ParseHeader(package.GetBuffer());
//        }
//        else
//        {
//            m_rcvdPdelayRespFollowUp = true;
//            m_rcvdPdelayRespFollowUpPtr = NULL;
//        }
//    }
//}
