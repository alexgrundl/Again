#include "systemport.h"

SystemPort::SystemPort()
{
    m_asCapable = false;
    m_syncReceiptTimeoutTimeInterval = {0, 0};
    m_currentLogSyncInterval = -3;
    m_initialLogSyncInterval = -3;
    m_syncInterval = {125 * 1000 * 1000, 0};
    m_neighborRateRatio = 1.0;
    m_neighborPropDelay = {0, 0};
    m_delayAsymmetry = {0, 0};
    m_computeNeighborRateRatio = true;
    m_computeNeighborPropDelay = true;
    m_portEnabled = true;
    m_pttPortEnabled = true;
    m_syncReceiptTimeout = 3;
    m_currentLogPdelayReqInterval = 0;
    m_initialLogPdelayReqInterval = 0;
    m_pdelayReqInterval = {NS_PER_SEC, 0};
    m_allowedLostResponses = 3;
    m_isMeasuringDelay = false;
    m_neighborPropDelayThresh = {9000, 0};
    m_syncSequenceId = 0;

    m_reselect = false;
    m_selected = false;
    announceReceiptTimeoutTimeInterval = {0, 0};
    infoIs = SPANNING_TREE_PORT_STATE_DISABLED;

    masterPriority.identity.priority1 = 255;
    masterPriority.identity.clockQuality.clockClass = CLOCK_CLASS_SLAVE_ONLY;
    masterPriority.identity.clockQuality.clockAccuracy = CLOCK_ACCURACY_UNKNOWN;
    masterPriority.identity.clockQuality.offsetScaledLogVariance = UINT16_MAX;
    masterPriority.identity.priority2 = 255;
    memset(masterPriority.identity.clockIdentity, 255, sizeof(masterPriority.identity.clockIdentity));
    masterPriority.stepsRemoved = UINT16_MAX;
    memset(masterPriority.sourcePortIdentity.clockIdentity, 255, sizeof(masterPriority.sourcePortIdentity.clockIdentity));
    masterPriority.sourcePortIdentity.portNumber = UINT16_MAX;
    masterPriority.portNumber = UINT16_MAX;

    currentLogAnnounceInterval = 0;
    initialLogAnnounceInterval = 0;
    announceInterval = {NS_PER_SEC, 0};
    messageStepsRemoved = 0;
    newInfo = false;

    ResetPortPriority();

    portStepsRemoved = 0;
    rcvdAnnouncePtr = NULL;
    rcvdMsg = false;
    updtInfo = true;
    annLeap61 = false;
    annLeap59 = false;
    annCurrentUtcOffsetValid = false;
    annTimeTraceable = false;
    annFrequencyTraceable = false;
    annCurrentUtcOffset = 0;
    annTimeSource = CLOCK_TIME_SOURCE_INTERNAL_OSCILLATOR;
    announceReceiptTimeout = 3;

    memset(identity.clockIdentity, 255, CLOCK_ID_LENGTH);
    identity.portNumber = 0;
    syncCount = 0;
    pdelayCount = 0;
}

SystemPort::~SystemPort()
{

}

void SystemPort::ResetPortPriority()
{
    portPriority.identity.priority1 = 255;
    portPriority.identity.clockQuality.clockClass = CLOCK_CLASS_SLAVE_ONLY;
    portPriority.identity.clockQuality.clockAccuracy = CLOCK_ACCURACY_UNKNOWN;
    portPriority.identity.clockQuality.offsetScaledLogVariance = UINT16_MAX;
    portPriority.identity.priority2 = 255;
    memset(portPriority.identity.clockIdentity, 255, sizeof(portPriority.identity.clockIdentity));
    portPriority.stepsRemoved = UINT16_MAX;
    memset(portPriority.sourcePortIdentity.clockIdentity, 255, sizeof(portPriority.sourcePortIdentity.clockIdentity));
    portPriority.sourcePortIdentity.portNumber = UINT16_MAX;
    portPriority.portNumber = UINT16_MAX;
}

bool SystemPort::GetAsCapable()
{
    return m_asCapable;
}

void SystemPort::SetAsCapable(bool capable)
{
    m_asCapable = capable;
}

UScaledNs SystemPort::GetSyncReceiptTimeoutTimeInterval()
{
    return m_syncReceiptTimeoutTimeInterval;
}

void SystemPort::SetSyncReceiptTimeoutTimeInterval(UScaledNs interval)
{
    m_syncReceiptTimeoutTimeInterval = interval;
}

int8_t SystemPort::GetCurrentLogSyncInterval()
{
    return m_currentLogSyncInterval;
}

void SystemPort::SetCurrentLogSyncInterval(int8_t interval)
{
    m_currentLogSyncInterval = interval;
}

int8_t SystemPort::GetInitialLogSyncInterval()
{
    return m_initialLogSyncInterval;
}

void SystemPort::SetInitialLogSyncInterval(int8_t interval)
{
    m_initialLogSyncInterval = interval;
}

UScaledNs SystemPort::GetSyncInterval()
{
    return m_syncInterval;
}

void SystemPort::SetSyncInterval(UScaledNs interval)
{
    m_syncInterval = interval;
}

double SystemPort::GetNeighborRateRatio()
{
    return m_neighborRateRatio;
}

void SystemPort::SetNeighborRateRatio(double ratio)
{
    m_neighborRateRatio = ratio;
}

UScaledNs SystemPort::GetNeighborPropDelay()
{
    return m_neighborPropDelay;
}

void SystemPort::SetNeighborPropDelay(UScaledNs delay)
{
    m_neighborPropDelay = delay;
}

UScaledNs SystemPort::GetDelayAsymmetry()
{
    return m_delayAsymmetry;
}

void SystemPort::SetDelayAsymmetry(UScaledNs asymmetry)
{
    m_delayAsymmetry = asymmetry;
}


bool SystemPort::GetComputeNeighborRateRatio()
{
    return m_computeNeighborRateRatio;
}

void SystemPort::SetComputeNeighborRateRatio(bool compute)
{
    m_computeNeighborRateRatio = compute;
}

bool SystemPort::GetComputeNeighborPropDelay()
{
    return m_computeNeighborPropDelay;
}

void SystemPort::SetComputeNeighborPropDelay(bool compute)
{
    m_computeNeighborPropDelay = compute;
}

bool SystemPort::IsPortEnabled()
{
    return m_portEnabled;
}

void SystemPort::EnablePort()
{
    m_portEnabled = true;
}

void SystemPort::DisablePort()
{
    m_portEnabled = false;
}

bool SystemPort::IsPttPortEnabled()
{
    return m_pttPortEnabled;
}

void SystemPort::EnablePttPort()
{
    m_pttPortEnabled = true;
}

void SystemPort::DisablePttPort()
{
    m_pttPortEnabled = false;
}

uint8_t SystemPort::GetSyncReceiptTimeout()
{
    return m_syncReceiptTimeout;
}

void SystemPort::SetSyncReceiptTimeout(uint8_t timeout)
{
    m_syncReceiptTimeout = timeout;
}

int8_t SystemPort::GetCurrentLogPdelayReqInterval()
{
    return m_currentLogPdelayReqInterval;
}

void SystemPort::SetCurrentLogPdelayReqInterval(int8_t interval)
{
    m_currentLogPdelayReqInterval = interval;
}

int8_t SystemPort::GetInitialLogPdelayReqInterval()
{
    return m_initialLogPdelayReqInterval;
}

void SystemPort::SetInitialLogPdelayReqInterval(int8_t interval)
{
    m_initialLogPdelayReqInterval = interval;
}

UScaledNs SystemPort::GetPdelayReqInterval()
{
    return m_pdelayReqInterval;
}

void SystemPort::SetPdelayReqInterval(UScaledNs interval)
{
    m_pdelayReqInterval = interval;
}

uint16_t SystemPort::GetAllowedLostResponses()
{
    return m_allowedLostResponses;
}

void SystemPort::SetAllowedLostResponses(uint16_t nAllowed)
{
    m_allowedLostResponses = nAllowed;
}

bool SystemPort::GetIsMeasuringDelay()
{
    return m_isMeasuringDelay;
}

void SystemPort::SetIsMeasuringDelay(bool isMeasuring)
{
    m_isMeasuringDelay = isMeasuring;
}

UScaledNs SystemPort::GetNeighborPropDelayThresh()
{
    return m_neighborPropDelayThresh;
}

void SystemPort::SetNeighborPropDelayThresh(UScaledNs threshold)
{
    m_neighborPropDelayThresh = threshold;
}

uint16_t SystemPort::GetSyncSequenceId()
{
    return m_syncSequenceId;
}

void SystemPort::SetSyncSequenceId(uint16_t id)
{
    m_syncSequenceId = id;
}

void SystemPort::IncreaseSyncSequenceId()
{
    m_syncSequenceId++;
}

bool SystemPort::GetReselect()
{
    return m_reselect;
}

void SystemPort::SetReselect(bool reselect)
{
    m_reselect = reselect;
}

bool SystemPort::IsSelected()
{
    return m_selected;
}

void SystemPort::SetSelected(bool selected)
{
    m_selected = selected;
}

UScaledNs SystemPort::GetAnnounceReceiptTimeoutTimeInterval()
{
    return announceReceiptTimeoutTimeInterval;
}

void SystemPort::SetAnnounceReceiptTimeoutTimeInterval(UScaledNs interval)
{
    announceReceiptTimeoutTimeInterval = interval;
}

SpanningTreePortState SystemPort::GetInfoIs()
{
    return infoIs;
}

void SystemPort::SetInfoIs(SpanningTreePortState info)
{
    infoIs = info;
}

PriorityVector SystemPort::GetMasterPriority()
{
    return masterPriority;
}

void SystemPort::SetMasterPriority(PriorityVector priority)
{
    masterPriority = priority;
}

int8_t SystemPort::GetCurrentLogAnnounceInterval()
{
    return currentLogAnnounceInterval;
}

void SystemPort::SetCurrentLogAnnounceInterval(int8_t interval)
{
    currentLogAnnounceInterval = interval;
}

int8_t SystemPort::GetInitialLogAnnounceInterval()
{
    return initialLogAnnounceInterval;
}

void SystemPort::SetInitialLogAnnounceInterval(int8_t interval)
{
    initialLogAnnounceInterval = interval;
}

UScaledNs SystemPort::GetAnnounceInterval()
{
    return announceInterval;
}

void SystemPort::SetAnnounceInterval(UScaledNs interval)
{
    announceInterval = interval;
}

uint16_t SystemPort::GetMessageStepsRemoved()
{
    return messageStepsRemoved;
}

void SystemPort::SetMessageStepsRemoved(uint16_t stepsRemoved)
{
    messageStepsRemoved = stepsRemoved;
}

bool SystemPort::GetNewInfo()
{
    return newInfo;
}

void SystemPort::SetNewInfo(bool newInfo)
{
    this->newInfo = newInfo;
}

PriorityVector SystemPort::GetPortPriority()
{
    return portPriority;
}

void SystemPort::SetPortPriority(PriorityVector priority)
{
    portPriority = priority;
}

uint16_t SystemPort::GetPortStepsRemoved()
{
    return portStepsRemoved;
}

void SystemPort::SetPortStepsRemoved(uint16_t stepsRemoved)
{
    portStepsRemoved = stepsRemoved;
}

PtpMessageAnnounce* SystemPort::GetRcvdAnnouncePtr()
{
    return rcvdAnnouncePtr;
}

void SystemPort::SetRcvdAnnouncePtr(PtpMessageAnnounce* ptr)
{
    rcvdAnnouncePtr = ptr;
}

bool SystemPort::GetRcvdMsg()
{
    return rcvdMsg;
}

void SystemPort::SetRcvdMsg(bool rcvd)
{
    rcvdMsg = rcvd;
}

bool SystemPort::GetUpdtInfo()
{
    return updtInfo;
}

void SystemPort::SetUpdtInfo(bool update)
{
    updtInfo = update;
}

bool SystemPort::GetAnnLeap61()
{
    return annLeap61;
}

void SystemPort::SetAnnLeap61(bool set)
{
    annLeap61 = set;
}

bool SystemPort::GetAnnLeap59()
{
    return annLeap59;
}

void SystemPort::SetAnnLeap59(bool set)
{
    annLeap59 = set;
}

bool SystemPort::GetAnnCurrentUtcOffsetValid()
{
    return annCurrentUtcOffsetValid;
}

void SystemPort::SetAnnCurrentUtcOffsetValid(bool valid)
{
    annCurrentUtcOffsetValid = valid;
}

bool SystemPort::GetAnnTimeTraceable()
{
    return annTimeTraceable;
}

void SystemPort::SetAnnTimeTraceable(bool tracable)
{
    annTimeTraceable = tracable;
}

bool SystemPort::GetAnnFrequencyTraceable()
{
    return annFrequencyTraceable;
}

void SystemPort::SetAnnFrequencyTraceable(bool tracable)
{
    annFrequencyTraceable = tracable;
}

int16_t SystemPort::GetAnnCurrentUtcOffset()
{
    return annCurrentUtcOffset;
}

void SystemPort::SetAnnCurrentUtcOffset(int16_t offset)
{
    annCurrentUtcOffset = offset;
}

ClockTimeSource SystemPort::GetAnnTimeSource()
{
    return annTimeSource;
}

void SystemPort::SetAnnTimeSource(ClockTimeSource source)
{
    annTimeSource = source;
}

uint8_t SystemPort::GetAnnounceReceiptTimeout()
{
    return announceReceiptTimeout;
}

void SystemPort::SetAnnounceReceiptTimeout(uint8_t timeout)
{
    announceReceiptTimeout = timeout;
}

PortIdentity SystemPort::GetIdentity()
{
    return identity;
}

void SystemPort::SetIdentity(PortIdentity identity)
{
    this->identity = identity;
}

uint32_t SystemPort::GetSyncCount()
{
    return syncCount;
}

void SystemPort::SetSyncCount(uint32_t count)
{
    syncCount = count;
}

void SystemPort::IncreaseSyncCount()
{
    syncCount++;
}

uint32_t SystemPort::GetPdelayCount()
{
    return pdelayCount;
}

void SystemPort::SetPdelayCount(uint32_t count)
{
    pdelayCount = count;
}

void SystemPort::IncreasePdelayCount()
{
    pdelayCount++;
}
