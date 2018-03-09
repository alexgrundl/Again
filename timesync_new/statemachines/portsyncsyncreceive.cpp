#include "portsyncsyncreceive.h"

PortSyncSyncReceive::PortSyncSyncReceive(TimeAwareSystem* timeAwareSystem, PortGlobal* port, SiteSyncSync* siteSyncSync) :
    StateMachineBasePort(timeAwareSystem, port)
{
    m_rcvdMDSync = false;
    m_rcvdMDSyncPtr = new MDSyncReceive();
    m_txPSSyncPtr = new PortSyncSync();
    m_rateRatio = 1.0;

    m_siteSyncSync = siteSyncSync;
}

PortSyncSyncReceive::~PortSyncSyncReceive()
{
    delete m_rcvdMDSyncPtr;
    delete m_txPSSyncPtr;
}

void PortSyncSyncReceive::SetPSSyncPSSR (MDSyncReceive* rcvdMDSyncPtr, UScaledNs syncReceiptTimeoutTimeInterval, double rateRatio)
{
    m_txPSSyncPtr->localPortNumber = m_portGlobal->identity.portNumber;
    m_txPSSyncPtr->followUpCorrectionField = rcvdMDSyncPtr->followUpCorrectionField;
    m_txPSSyncPtr->sourcePortIdentity = rcvdMDSyncPtr->sourcePortIdentity;
    m_txPSSyncPtr->logMessageInterval = rcvdMDSyncPtr->logMessageInterval;
    m_txPSSyncPtr->preciseOriginTimestamp = rcvdMDSyncPtr->preciseOriginTimestamp;
    m_txPSSyncPtr->upstreamTxTime = rcvdMDSyncPtr->upstreamTxTime;
    m_txPSSyncPtr->syncReceiptTimeoutTime = m_timeAwareSystem->GetCurrentTime() + syncReceiptTimeoutTimeInterval;
    m_txPSSyncPtr->rateRatio = rateRatio;
}


void PortSyncSyncReceive::TxPSSyncPSSR()
{
    m_siteSyncSync->SetSync(m_txPSSyncPtr);
}

void PortSyncSyncReceive::ProcessSync(MDSyncReceive* rcvd)
{
    m_rcvdMDSyncPtr->followUpCorrectionField = rcvd->followUpCorrectionField;
    m_rcvdMDSyncPtr->gmTimeBaseIndicator = rcvd->gmTimeBaseIndicator;
    m_rcvdMDSyncPtr->lastGmFreqChange = rcvd->lastGmFreqChange;
    m_rcvdMDSyncPtr->lastGmPhaseChange = rcvd->lastGmPhaseChange;
    m_rcvdMDSyncPtr->logMessageInterval = rcvd->logMessageInterval;
    m_rcvdMDSyncPtr->preciseOriginTimestamp = rcvd->preciseOriginTimestamp;
    m_rcvdMDSyncPtr->rateRatio = rcvd->rateRatio;
    m_rcvdMDSyncPtr->sourcePortIdentity = rcvd->sourcePortIdentity;
    m_rcvdMDSyncPtr->upstreamTxTime = rcvd->upstreamTxTime;
    m_rcvdMDSync = true;
}


void PortSyncSyncReceive::ProcessState()
{
    if(m_timeAwareSystem->BEGIN || (m_rcvdMDSync && (!m_portGlobal->portEnabled || !m_portGlobal->pttPortEnabled || !m_portGlobal->asCapable)))
    {
        m_rcvdMDSync = false;
        m_state = STATE_DISCARD;
    }
    else
    {
        switch(m_state)
        {
        case STATE_DISCARD:
        case STATE_RECEIVED_SYNC:
            if(m_rcvdMDSync && m_portGlobal->portEnabled && m_portGlobal->pttPortEnabled && m_portGlobal->asCapable
                    && (m_state == STATE_DISCARD ||
                        (memcmp(m_rcvdMDSyncPtr->sourcePortIdentity.clockIdentity, m_timeAwareSystem->gmPriority.sourcePortIdentity.clockIdentity,
                                sizeof(m_rcvdMDSyncPtr->sourcePortIdentity.clockIdentity)) == 0 && m_rcvdMDSyncPtr->sourcePortIdentity.portNumber ==
                         m_timeAwareSystem->gmPriority.sourcePortIdentity.portNumber)))
            {
                m_rcvdMDSync = false;
                m_rateRatio = m_rcvdMDSyncPtr->rateRatio;
                m_rateRatio += m_portGlobal->neighborRateRatio - 1.0;
                m_portGlobal->syncReceiptTimeoutTimeInterval.ns = (uint64_t)m_portGlobal->syncReceiptTimeout * NS_PER_SEC
                        * pow(2, m_rcvdMDSyncPtr->logMessageInterval);
                m_portGlobal->syncReceiptTimeoutTimeInterval.ns_frac = 0;

                SetPSSyncPSSR(m_rcvdMDSyncPtr, m_portGlobal->syncReceiptTimeoutTimeInterval, m_rateRatio);
                TxPSSyncPSSR();

                m_state = STATE_RECEIVED_SYNC;
            }
            break;

        default:
            fprintf(stderr, "State not allowed: %i, File: %s\n", m_state, __FILE__);
            break;
        }
    }
}
