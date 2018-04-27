#include "portglobal.h"

PortGlobal::PortGlobal()
{
    asCapable = false;
    syncReceiptTimeoutTimeInterval = {0, 0};
    currentLogSyncInterval = -3;
    initialLogSyncInterval = -3;
    syncInterval = {125 * 1000 * 1000, 0};
    neighborRateRatio = 1.0;
    neighborPropDelay = {0, 0};
    delayAsymmetry = {0, 0};
    computeNeighborRateRatio = true;
    computeNeighborPropDelay = true;
    portEnabled = true;
    pttPortEnabled = true;
    syncReceiptTimeout = 3;
    currentLogPdelayReqInterval = 0;
    initialLogPdelayReqInterval = 0;
    pdelayReqInterval = {NS_PER_SEC, 0};
    allowedLostResponses = 3;
    isMeasuringDelay = false;
    neighborPropDelayThresh = {9000, 0};
    syncSequenceId = 0;

    reselect = false;
    selected = false;
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
    remoteLocalDelta = {0, 0};
    remoteLocalRate = 1.0;
}

PortGlobal::~PortGlobal()
{

}
