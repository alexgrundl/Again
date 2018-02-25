#include "mdpdelayreq.h"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <linux/if_packet.h>

MDPdelayReq::MDPdelayReq(TimeAwareSystem* timeAwareSystem, PortGlobal* port, MDGlobal* mdGlobal) :
    StateMachineBaseMD(timeAwareSystem, port, mdGlobal)
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
}

MDPdelayReq::~MDPdelayReq()
{
    delete m_txPdelayReqPtr;
}

PtpMessagePDelayReq* MDPdelayReq::SetPdelayReq()
{
    PtpMessagePDelayReq* pdelayReqPtr = new PtpMessagePDelayReq();

    /* 1) sourcePortIdentity is set equal to the port identity of the port corresponding to this MD entity
        (see 8.5.2),

       3) remaining parameters are set as specified in 11.4.2 and 11.4.5. */

    pdelayReqPtr->SetSequenceID(m_pdelayReqSequenceId);

    return pdelayReqPtr;
}

void MDPdelayReq::TxPdelayReq(PtpMessagePDelayReq* txPdelayReqPtr)
{
    /* transmits a Pdelay_Req message from the MD entity, containing the parameters in the structure pointed to by txPdelayReqPtr. */
    uint8_t sendbuf[1024];
    int sockfd;
    struct ether_header *ethheader = (struct ether_header *) sendbuf;

    sockfd = socket(AF_PACKET, SOCK_RAW, IPPROTO_RAW);

    struct ifreq if_mac;
    memset(&if_mac, 0, sizeof(struct ifreq));
    strncpy(if_mac.ifr_name, "wlo1", IFNAMSIZ-1);
    if (ioctl(sockfd, SIOCGIFHWADDR, &if_mac) < 0)
        perror("SIOCGIFHWADDR");

    ethheader->ether_shost[0] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[0];
    ethheader->ether_shost[1] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[1];
    ethheader->ether_shost[2] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[2];
    ethheader->ether_shost[3] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[3];
    ethheader->ether_shost[4] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[4];
    ethheader->ether_shost[5] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[5];

    ethheader->ether_dhost[0] = 0x01;
    ethheader->ether_dhost[1] = 0x80;
    ethheader->ether_dhost[2] = 0xC2;
    ethheader->ether_dhost[3] = 0x00;
    ethheader->ether_dhost[4] = 0x00;
    ethheader->ether_dhost[5] = 0x0E;

    ethheader->ether_type = htons(0x88F7);

    txPdelayReqPtr->GetMessage(sendbuf + sizeof(struct ether_header));

    struct ifreq if_idx;
    memset(&if_idx, 0, sizeof(struct ifreq));
    strncpy(if_idx.ifr_name, "wlo1", IFNAMSIZ-1);
    if (ioctl(sockfd, SIOCGIFINDEX, &if_idx) < 0)
        perror("SIOCGIFINDEX");

    /* Destination address */
    struct sockaddr_ll socket_address;
    /* Index of the network device */
    socket_address.sll_ifindex = if_idx.ifr_ifindex;
    /* Address length*/
    socket_address.sll_halen = ETH_ALEN;
    /* Destination MAC */
    socket_address.sll_addr[0] = 0x01;
    socket_address.sll_addr[1] = 0x80;
    socket_address.sll_addr[2] = 0xC2;
    socket_address.sll_addr[3] = 0x00;
    socket_address.sll_addr[4] = 0x00;
    socket_address.sll_addr[5] = 0x0E;

    if (sendto(sockfd, sendbuf, 54 + 14, 0, (struct sockaddr*)&socket_address, sizeof(struct sockaddr_ll)) < 0)
        printf("Send failed\n");
}

double MDPdelayReq::ComputePdelayRateRatio()
{
    double pdelayRateRatio = 1.0;

    return pdelayRateRatio;
}

UScaledNs MDPdelayReq::ComputePropTime()
{
     UScaledNs propTime = {0, 0};

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
                m_pdelayReqSequenceId = rand() % 65536;
                delete m_txPdelayReqPtr;
                m_txPdelayReqPtr = SetPdelayReq();
                TxPdelayReq(m_txPdelayReqPtr);
                m_pdelayIntervalTimer = m_timeAwareSystem->GetCurrentTime();
                m_lostResponses = 0;
                m_mdGlobal->isMeasuringDelay = false;
                m_portGlobal->asCapable = false;
                m_state = STATE_INITIAL_SEND_PDELAY_REQ;
            }
            break;

        case STATE_INITIAL_SEND_PDELAY_REQ:
            if(m_rcvdMDTimestampReceive)
            {
                m_rcvdMDTimestampReceive = false;
                m_state = STATE_WAITING_FOR_PDELAY_RESP;
            }
            break;

        case STATE_WAITING_FOR_PDELAY_RESP:
            if(m_rcvdPdelayResp && (m_rcvdPdelayRespPtr->GetSequenceID() == m_txPdelayReqPtr->GetSequenceID())
                    && memcmp(m_rcvdPdelayRespPtr->GetRequestingPortIdentity()->clockIdentity, m_timeAwareSystem->thisClock, sizeof(m_timeAwareSystem->thisClock)) == 0 &&
                    (m_rcvdPdelayRespPtr->GetRequestingPortIdentity()->portNumber == m_portGlobal->thisPort))
            {
                m_rcvdPdelayResp = false;
                m_state = STATE_WAITING_FOR_PDELAY_RESP_FOLLOW_UP;
            }
            else if((m_timeAwareSystem->GetCurrentTime() - m_pdelayIntervalTimer >= m_mdGlobal->pdelayReqInterval) ||
                    (m_rcvdPdelayResp &&
                    (memcmp(m_rcvdPdelayRespPtr->GetRequestingPortIdentity()->clockIdentity, m_timeAwareSystem->thisClock, sizeof(m_timeAwareSystem->thisClock)) != 0 ||
                    (m_rcvdPdelayRespPtr->GetRequestingPortIdentity()->portNumber != m_portGlobal->thisPort) ||
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
                m_mdGlobal->isMeasuringDelay = true;
                if ((m_portGlobal->neighborPropDelay <= m_mdGlobal->neighborPropDelayThresh) &&
                (m_rcvdPdelayRespPtr->GetSourcePortIdentity().clockIdentity != m_timeAwareSystem->thisClock) && m_neighborRateRatioValid)
                    m_portGlobal->asCapable = true;
                else
                    m_portGlobal->asCapable = false;
                m_state = STATE_WAITING_FOR_PDELAY_INTERVAL_TIMER;
            }
            else if((m_timeAwareSystem->GetCurrentTime() - m_pdelayIntervalTimer >= m_mdGlobal->pdelayReqInterval) ||
                    (m_rcvdPdelayResp && (m_rcvdPdelayRespPtr->GetSequenceID() == m_txPdelayReqPtr->GetSequenceID())))
            {
                ExecuteResetState();
                m_state = STATE_RESET;
            }
            break;

        case STATE_WAITING_FOR_PDELAY_INTERVAL_TIMER:
            if(m_timeAwareSystem->GetCurrentTime() - m_pdelayIntervalTimer >= m_mdGlobal->pdelayReqInterval)
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
    if (m_lostResponses <= m_mdGlobal->allowedLostResponses)
    m_lostResponses += 1;
    else
    {
        m_mdGlobal->isMeasuringDelay = false;
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

