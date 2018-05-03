#include "portannouncereceive.h"

PortAnnounceReceive::PortAnnounceReceive(TimeAwareSystem* timeAwareSystem, PortGlobal* port,
                                         PortAnnounceInformation* portAnnounceInformation) : StateMachineBasePort(timeAwareSystem, port)
{
    m_rcvdAnnounce = false;
    m_portAnnounceInformation = portAnnounceInformation;
}

PortAnnounceReceive::~PortAnnounceReceive()
{

}

bool PortAnnounceReceive::QualifyAnnounce(PtpMessageAnnounce* rcvdAnnouncePtr)
{
    if(memcmp(rcvdAnnouncePtr->GetSourcePortIdentity().clockIdentity, m_timeAwareSystem->GetClockIdentity(), CLOCK_ID_LENGTH) == 0)
        return false;
    else if(rcvdAnnouncePtr->GetStepsRemoved() >= 255)
        return false;

    PtpMessageAnnounce::AnnounceTLV tlv = rcvdAnnouncePtr->GetTLV();
    for (std::vector<int>::size_type i = 0; i < tlv.pathSequence.size(); ++i)
    {
        if(memcmp(tlv.pathSequence[i], m_timeAwareSystem->GetClockIdentity(), CLOCK_ID_LENGTH) == 0)
            return false;
    }

    /**If a path trace TLV is present and the portRole of the port is SlavePort, the pathSequence array field of the TLV is copied to the global array pathTrace,
     * and thisClock is appended to pathTrace (i.e., is added to the end of the array). */
    bool pathSequenceCopied = false;

    m_timeAwareSystem->ClearPathTrace();
    for (std::vector<uint8_t*>::size_type i = 0; i < tlv.pathSequence.size(); ++i)
    {
        if(m_timeAwareSystem->GetSelectedRole(m_portGlobal->identity.portNumber) == PORT_ROLE_SLAVE)
        {
            m_timeAwareSystem->AddPath(tlv.pathSequence[i]);
            pathSequenceCopied = true;
        }
    }
    if(pathSequenceCopied)
        m_timeAwareSystem->AddPath(m_timeAwareSystem->GetClockIdentity());


    return true;
}

void PortAnnounceReceive::ProcessState()
{
    if(m_timeAwareSystem->BEGIN || (m_rcvdAnnounce && (!m_portGlobal->portEnabled || !m_portGlobal->pttPortEnabled || !m_portGlobal->asCapable)))
    {
        m_rcvdAnnounce = false;
        m_portGlobal->rcvdMsg = false;
        m_state = STATE_DISCARD;
    }
    else
    {
        switch (m_state)
        {
        case STATE_DISCARD:
            if(m_rcvdAnnounce && m_portGlobal->portEnabled && m_portGlobal->pttPortEnabled && m_portGlobal->asCapable)
            {
                m_rcvdAnnounce = false;
                if(QualifyAnnounce(m_portGlobal->rcvdAnnouncePtr))
                {
//                    printf("Qualifiying announce...\n");
                    m_portGlobal->rcvdMsg = true;
                }
                m_state = STATE_RECEIVE;
            }
            break;
        case STATE_RECEIVE:
            if(m_rcvdAnnounce && m_portGlobal->portEnabled && m_portGlobal->pttPortEnabled && m_portGlobal->asCapable && !m_portGlobal->rcvdMsg)
            {
                m_rcvdAnnounce = false;
                if(QualifyAnnounce(m_portGlobal->rcvdAnnouncePtr))
                {
//                    printf("Qualifiying announce...\n");
                    m_portGlobal->rcvdMsg = true;
                }
            }
            break;
        default:
            fprintf(stderr, "State not allowed: %i, File: %s\n", m_state, __FILE__);
            break;
        }

    }
}

void PortAnnounceReceive::SetAnnounce(ReceivePackage *package)
{
    delete m_portGlobal->rcvdAnnouncePtr;
    m_portGlobal->rcvdAnnouncePtr = new PtpMessageAnnounce();
    m_portGlobal->rcvdAnnouncePtr->ParsePackage(package->GetBuffer());
    m_portGlobal->rcvdAnnouncePtr->SetReceiveTime(package->GetTimestamp());
    m_rcvdAnnounce = true;
}
