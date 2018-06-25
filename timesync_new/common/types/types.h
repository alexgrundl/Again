#ifndef TYPES_H
#define TYPES_H

#define __COMMONDEFS_LOGTAGS "<xtss-daemon v2>"
#include "commondefs.h"

#include <stdint.h>
#include <cstddef>
#include <math.h>
#include <stdio.h>

#include "ipcdef.hpp"
#include "scaledns.h"
#include "uscaledns.h"
#include "timestamp.h"
#include "extendedtimestamp.h"

typedef ScaledNs TimeInterval;

struct PortIdentity
{
    uint8_t clockIdentity[CLOCK_ID_LENGTH];
    uint16_t portNumber;
};

struct ClockQuality
{
    uint8_t clockClass;
    ClockAccuracy clockAccuracy;
    uint16_t offsetScaledLogVariance;
};

#define PRIOTITY_TIME_AWARE_SYSTEM_NETWORK  246 // Network infrastructure time-aware system
#define PRIOTITY_TIME_AWARE_SYSTEM_OTHER    248 // Other time-aware systems
#define PRIOTITY_TIME_AWARE_SYSTEM_PORTABLE 250 // Portable time-aware system
#define PRIOTITY_TIME_AWARE_SYSTEM_SLAVE    255 // Slave only time-aware system

enum PortRole
{
    /**
     * Any port of the time-aware system for which portEnabled, pttPortEnabled, and asCapable are not all TRUE.
     */
    PORT_ROLE_DISABLED = 3,
    /**
     * Any port, P, of the time-aware system that is closer to the root than any other port of the gPTP communication path connected to P
     */
    PORT_ROLE_MASTER = 6,
    /**
     * Any port of the time-aware system whose port role is not MasterPort, SlavePort, or DisabledPort.
     */
    PORT_ROLE_PASSIVE = 7,
    /**
     * The one port of the time-aware system that is closest to the root time-aware system. If the root is grandmaster-capable,
     * the SlavePort is also closest to the grandmaster. The time- aware system does not transmit Sync or Announce messages on the SlavePort.
     */
    PORT_ROLE_SLAVE = 9
};

typedef UScaledNs MDTimestampReceive;


struct SystemIdentity
{
    uint8_t priority1;
    ClockQuality clockQuality;
    uint8_t priority2;
    uint8_t clockIdentity[CLOCK_ID_LENGTH];

    enum Info
    {
        INFO_SUPERIOR,
        INFO_EQUAL,
        INFO_INFERIOR
    };


    Info Compare(SystemIdentity identity)
    {
        if(priority1 == identity.priority1)
        {
            if(clockQuality.clockClass == identity.clockQuality.clockClass)
            {
                if(clockQuality.clockAccuracy == identity.clockQuality.clockAccuracy)
                {
                    if(clockQuality.offsetScaledLogVariance == identity.clockQuality.offsetScaledLogVariance)
                    {
                        if(priority2 == identity.priority2)
                        {
                            for (uint8_t i = 0; i < sizeof(clockIdentity); ++i)
                            {
                                if(clockIdentity[i] != identity.clockIdentity[i])
                                    return clockIdentity[i] < identity.clockIdentity[i] ? INFO_SUPERIOR : INFO_INFERIOR;
                            }
                            return INFO_EQUAL;
                        }
                        return priority2 < identity.priority2 ? INFO_SUPERIOR : INFO_INFERIOR;
                    }
                    return clockQuality.offsetScaledLogVariance < identity.clockQuality.offsetScaledLogVariance ? INFO_SUPERIOR : INFO_INFERIOR;
                }
                return clockQuality.clockAccuracy < identity.clockQuality.clockAccuracy ? INFO_SUPERIOR : INFO_INFERIOR;
            }
            return clockQuality.clockClass < identity.clockQuality.clockClass ? INFO_SUPERIOR : INFO_INFERIOR;
        }
        return priority1 < identity.priority1 ? INFO_SUPERIOR : INFO_INFERIOR;
    }
};

struct PriorityVector
{
    SystemIdentity identity;
    uint16_t stepsRemoved;

    /**
     * @brief The port identity of the transmitting time-aware-system
     */
    PortIdentity sourcePortIdentity;

    /**
     * @brief Port number of the receiving port.
     */
    uint16_t portNumber;


    SystemIdentity::Info Compare(PriorityVector vector)
    {
        SystemIdentity::Info info = identity.Compare(vector.identity);
        if(info == SystemIdentity::INFO_EQUAL)
        {
            if(stepsRemoved == vector.stepsRemoved)
            {
                for (uint8_t i = 0; i < sizeof(sourcePortIdentity.clockIdentity); ++i)
                {
                    if(sourcePortIdentity.clockIdentity[i] != vector.sourcePortIdentity.clockIdentity[i])
                        return sourcePortIdentity.clockIdentity[i] < vector.sourcePortIdentity.clockIdentity[i] ? SystemIdentity::INFO_SUPERIOR : SystemIdentity::INFO_INFERIOR;
                }
                if(sourcePortIdentity.portNumber == vector.sourcePortIdentity.portNumber)
                {
                    if(portNumber != vector.portNumber)
                        return portNumber < vector.portNumber ? SystemIdentity::INFO_SUPERIOR : SystemIdentity::INFO_INFERIOR;
                    return SystemIdentity::INFO_EQUAL;
                }
                return sourcePortIdentity.portNumber < vector.sourcePortIdentity.portNumber ? SystemIdentity::INFO_SUPERIOR : SystemIdentity::INFO_INFERIOR;
            }
            return stepsRemoved < vector.stepsRemoved ? SystemIdentity::INFO_SUPERIOR : SystemIdentity::INFO_INFERIOR;
        }
        return info;
    }
};

enum SpanningTreePortState
{
    /**
     * The port has received current information (i.e., announce receipt timeout has not occurred and, if gmPresent is TRUE, sync receipt timeout
     * also has not occurred) from the master time-aware system for the attached communication path.
     */
    SPANNING_TREE_PORT_STATE_RECEIVED,
    /**
     * Information for the port has been derived from the SlavePort for the time-aware system (with the addition of SlavePort stepsRemoved).
     * This includes the possibility that the SlavePort is the port whose portNumber is 0, i.e., the time-aware system is the root of the
     * gPTP domain.
     */
    SPANNING_TREE_PORT_STATE_MINE,
    /**
     * Announce receipt timeout or, in the case where gmPresent is TRUE, sync receipt timeout have occurred.
     */
    SPANNING_TREE_PORT_STATE_AGED,
    /**
     * PortEnabled, pttPortEnabled, and asCapable are not all TRUE.
     */
    SPANNING_TREE_PORT_STATE_DISABLED
};

#endif // TYPES_H

