#include "portsyncsyncreceive.h"

PortSyncSyncReceive::PortSyncSyncReceive(TimeAwareSystem* timeAwareSystem, PortGlobal* port, std::shared_ptr<SiteSyncSync> siteSyncSync) :
    StateMachineBasePort(timeAwareSystem, port)
{
    m_rcvdMDSync = false;
    m_rcvdMDSyncPtr = NULL;
    m_txPSSyncPtr = NULL;
    m_rateRatio = 1.0;

    m_siteSyncSync = siteSyncSync;
}

PortSyncSyncReceive::~PortSyncSyncReceive()
{
    delete m_txPSSyncPtr;
}

PortSyncSync* PortSyncSyncReceive::SetPSSyncPSSR (MDSyncReceive* rcvdMDSyncPtr, UScaledNs syncReceiptTimeoutTimeInterval, double rateRatio)
{
    PortSyncSync* txPSSyncPtr = new PortSyncSync();
    txPSSyncPtr->localPortNumber = m_portGlobal->thisPort;
    txPSSyncPtr->followUpCorrectionField = rcvdMDSyncPtr->followUpCorrectionField;
    txPSSyncPtr->sourcePortIdentity = rcvdMDSyncPtr->sourcePortIdentity;
    txPSSyncPtr->logMessageInterval = rcvdMDSyncPtr->logMessageInterval;
    txPSSyncPtr->preciseOriginTimestamp = rcvdMDSyncPtr->preciseOriginTimestamp;
    txPSSyncPtr->upstreamTxTime = rcvdMDSyncPtr->upstreamTxTime;
    txPSSyncPtr->syncReceiptTimeoutTime = m_timeAwareSystem->GetCurrentTime() + syncReceiptTimeoutTimeInterval;
    txPSSyncPtr->rateRatio = rateRatio;

    return txPSSyncPtr;
}


void PortSyncSyncReceive::TxPSSyncPSSR (PortSyncSync* txPSSyncPtr)
{
    m_siteSyncSync->SetSync(txPSSyncPtr);
}

void PortSyncSyncReceive::ProcessSync(MDSyncReceive* rcvd)
{
    m_rcvdMDSyncPtr = rcvd;
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
                    /*&& (m_state == STATE_DISCARD || m_rcvdMDSyncPtr->sourcePortIdentity == gmPriority.sourcePortIdentity)*/)
            {
                m_rcvdMDSync = false;
                m_rateRatio = m_rcvdMDSyncPtr->rateRatio;
                m_rateRatio += m_portGlobal->neighborRateRatio - 1.0;
                /* Irgendwas ist da faul......... Beim Standard sollte hier doch 3 * 125 ms rauskommen und nicht
                   3 * 2^13 Sekunden... "16 +" wird jetzt mal auskommentiert... */
                m_portGlobal->syncReceiptTimeoutTimeInterval.ns = (uint64_t)m_portGlobal->syncReceiptTimeout * NS_PER_SEC
                        * pow(2, /*16 +*/ m_rcvdMDSyncPtr->logMessageInterval);
                m_portGlobal->syncReceiptTimeoutTimeInterval.ns_frac = 0;

                delete m_txPSSyncPtr;
                m_txPSSyncPtr = SetPSSyncPSSR(m_rcvdMDSyncPtr, m_portGlobal->syncReceiptTimeoutTimeInterval, m_rateRatio);
                TxPSSyncPSSR(m_txPSSyncPtr);

                m_state = STATE_RECEIVED_SYNC;
            }
            break;

        default:
            fprintf(stderr, "State not allowed: %i, File: %s\n", m_state, __FILE__);
            break;
        }
    }
}
