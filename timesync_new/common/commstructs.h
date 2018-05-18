#ifndef COMM_STRUCTS
#define COMM_STRUCTS

#include <vector>
#include <time.h>

#include "types.h"
#include "ptpmessageannounce.h"

struct MDSyncSend
{
    ScaledNs followUpCorrectionField;
    PortIdentity sourcePortIdentity;
    int8_t logMessageInterval;
    Timestamp preciseOriginTimestamp;
    UScaledNs upstreamTxTime;
    double rateRatio;
    uint16_t gmTimeBaseIndicator;
    ScaledNs lastGmPhaseChange;
    double lastGmFreqChange;
    uint8_t domain;
};

typedef MDSyncSend MDSyncReceive;

struct PortSyncSync
{
    uint16_t localPortNumber;
    UScaledNs syncReceiptTimeoutTime;
    ScaledNs followUpCorrectionField;
    PortIdentity sourcePortIdentity;
    int8_t logMessageInterval;
    Timestamp preciseOriginTimestamp;
    UScaledNs upstreamTxTime;
    double rateRatio;
    uint16_t gmTimeBaseIndicator;
    ScaledNs lastGmPhaseChange;
    double lastGmFreqChange;
    uint8_t domain;
};

#endif // COMM_STRUCTS

