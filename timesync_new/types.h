#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <cstddef>
#include <math.h>

#define NS_PER_SEC 1000000000

struct ScaledNs
{
    int64_t ns;
    uint16_t ns_frac;

    ScaledNs operator*(double d) const
    {
        ScaledNs scaled_new = *this;
        double dScaled = (double)ns + (double)ns_frac * pow(2, -16);

        dScaled *= d;
        scaled_new.ns = dScaled;
        scaled_new.ns_frac = (uint16_t)((dScaled - (double)ns) * 65536.0);

        return scaled_new;
    }

    ScaledNs operator+(const ScaledNs& scaled) const
    {
        ScaledNs scaled_new = *this;

        scaled_new.ns += scaled.ns;
        uint16_t ns_frac_new = ns_frac + scaled.ns_frac;
        if(ns_frac > ns_frac_new)
            scaled_new.ns++;
        scaled_new.ns_frac = ns_frac_new;
        return scaled_new;
    }

    ScaledNs operator+=(ScaledNs scaled)
    {
        ns += scaled.ns;
        uint16_t ns_frac_new = ns_frac + scaled.ns_frac;
        if(ns_frac > ns_frac_new)
            ns++;
        ns_frac = ns_frac_new;

        return *this;
    }

    ScaledNs operator/(double d) const
    {
        ScaledNs scaled_new = *this;
        double dScaled = (double)ns + (double)ns_frac * pow(2, -16);

        dScaled /= d;
        scaled_new.ns = dScaled;
        scaled_new.ns_frac = (uint16_t)((dScaled - (double)ns) * 65536.0);

        return scaled_new;
    }
};

struct UScaledNs
{
    uint64_t ns;
    uint16_t ns_frac;

    UScaledNs operator+(const UScaledNs& uscaled) const
    {
        UScaledNs uscaled_new = *this;

        uscaled_new.ns += uscaled.ns;
        uint16_t ns_frac_new = ns_frac + uscaled.ns_frac;
        if(ns_frac > ns_frac_new)
            uscaled_new.ns++;
        uscaled_new.ns_frac = ns_frac_new;
        return uscaled_new;
    }

    UScaledNs operator-(const UScaledNs& uscaled) const
    {
        UScaledNs uscaled_new = *this;

        uscaled_new.ns -= uscaled.ns;
        uint16_t ns_frac_new = ns_frac - uscaled.ns_frac;
        if(ns_frac < ns_frac_new)
            uscaled_new.ns--;
        uscaled_new.ns_frac = ns_frac_new;
        return uscaled_new;
    }

    UScaledNs operator*(double d) const
    {
        UScaledNs uscaled_new;
        double dScaled = (double)ns + (double)ns_frac * pow(2, -16);

        dScaled *= d;
        uscaled_new.ns = dScaled;
        uscaled_new.ns_frac = (uint16_t)((dScaled - (double)ns) * 65536.0);

        return uscaled_new;
    }

    UScaledNs operator/(double d) const
    {
        UScaledNs uscaled_new;
        double dScaled = (double)ns + (double)ns_frac * pow(2, -16);

        dScaled /= d;
        uscaled_new.ns = dScaled;
        uscaled_new.ns_frac = (uint16_t)((dScaled - (double)ns) * 65536.0);

        return uscaled_new;
    }

    operator ScaledNs() const
    {
        ScaledNs scaled;
        scaled.ns = ns;
        scaled.ns_frac = ns_frac;
        return scaled;
    }

    bool operator>(const UScaledNs& uscaled) const
    {
        return ns > uscaled.ns || (ns == uscaled.ns && ns_frac > uscaled.ns_frac);
    }

    bool operator>=(const UScaledNs& uscaled) const
    {
        return ns > uscaled.ns || (ns == uscaled.ns && ns_frac >= uscaled.ns_frac);
    }

    bool operator<(const UScaledNs& uscaled) const
    {
        return ns < uscaled.ns || (ns == uscaled.ns && ns_frac < uscaled.ns_frac);
    }

    bool operator<=(const UScaledNs& uscaled) const
    {
        return ns < uscaled.ns || (ns == uscaled.ns && ns_frac <= uscaled.ns_frac);
    }
};

typedef ScaledNs TimeInterval;

struct Timestamp
{
    uint64_t sec;
    /** The nanoseconds member is always less than 10^9 */
    uint32_t ns;

    Timestamp operator+(const ScaledNs& scaled) const
    {
        Timestamp ts_new = *this;

        ts_new.sec += scaled.ns / NS_PER_SEC;
        uint32_t ns_new = ns + (scaled.ns % NS_PER_SEC);
        if(ns > ns_new)
            ts_new.sec++;
        ts_new.ns = ns_new;
        return ts_new;
    }
};

struct ExtendedTimestamp
{
    uint64_t sec;
    /** The nanoseconds member is always less than 10^9 */
    uint32_t ns;
    uint16_t ns_frac;

    ExtendedTimestamp operator=(const Timestamp& ts)
    {
        sec = ts.sec;
        ns = ts.ns;
        ns_frac = 0;

        return *this;
    }

    ExtendedTimestamp operator-(const ExtendedTimestamp& ts) const
    {
        ExtendedTimestamp ts_new = *this;
        ts_new.sec -= ts.sec;

        uint32_t ns_new = ns - ts.ns;
        if(ns < ns_new)
            ts_new.sec--;
        ts_new.ns = ns_new;

        uint16_t ns_frac_new = ns_frac - ts.ns_frac;
        if(ns_frac < ns_frac_new)
            ts_new.ns--;
        ts_new.ns_frac = ns_frac_new;

        return ts_new;
    }

    ExtendedTimestamp operator+=(ScaledNs scaled)
    {
        sec += scaled.ns / NS_PER_SEC;

        uint32_t ns_new = ns + (scaled.ns % NS_PER_SEC);
        if(ns_new > NS_PER_SEC)
        {
            sec++;
            ns_new -= NS_PER_SEC;
        }
        ns = ns_new;

        return *this;
    }

    double operator/(const UScaledNs uscaled) const
    {
//        double dTs = (double)sec + ((double)ns + (double)ns_frac * pow(2, -16)) / 1000000000.0;
//        double dScaled = ((double)uscaled.ns + (double)uscaled.ns_frac * pow(2, -16)) / 1000000000.0;
//        double remainder;

//        dTs /= dScaled;
//        sec = (uint64_t)dTs;
//        remainder = (dTs - sec) * 1000000000.0;
//        ns = (uint32_t)remainder;
//        ns_frac = (uint16_t)((remainder - (double)ns) * 65536.0);

        double dTs = (double)sec + ((double)ns + (double)ns_frac * pow(2, -16)) / (double)NS_PER_SEC;
        double dScaled = ((double)uscaled.ns + (double)uscaled.ns_frac * pow(2, -16)) / (double)NS_PER_SEC;

        return dTs / dScaled;
    }


    operator ScaledNs() const
    {
        ScaledNs scaled;
        scaled.ns = sec * 1000000000 + ns;
        scaled.ns_frac = ns_frac;
        return scaled;
    }
};

struct PortIdentity
{
    uint8_t clockIdentity[8];
    uint16_t portNumber;
};

enum ClockClass
{
    CLOCK_CLASS_PRIMARY = 6,        // A clock that is synchronized to a primary reference time source.
    CLOCK_CLASS_PRIMARY_LOST = 7,    // A clock that has previously been designated as CLOCK_CLASS_PRIMARY but that has lost the ability to synchronize.
                                    // to a primary reference time source and is in holdover mode and within holdover specifications.
    CLOCK_CLASS_APPLICATION = 13,       // A clock that is synchronized to an application-specific source of time.
    CLOCK_CLASS_APPLICATION_LOST = 14,  // A clock that has previously been designated as CLOCK_CLASS_APPLICATION but that has lost the ability to synchronize to an
                                        // application-specific source of time and is in holdover mode and within holdover specifications.

    /** To be defined... */

//    52: Degradation alternative A for a clock of clockClass 7 that
//    is not within holdover specification,
//    58: Degradation alternative A for a clock of clockClass 14 that
//    is not within holdover specification,
//    68..122: For use by alternate PTP profiles (68..122),
//    133..170: For use by alternate PTP profiles (133..170),
//    187: Degradation alternative B for a clock of clockClass 7 that
//    is not within holdover specification,
//    193: Degradation alternative B for a clock of clockClass 14 that
//    is not within holdover specification,
//    216..232: For use by alternate PTP profiles,
//    248: Default none of the other clockClass definitions apply,
//    255: A slave-only clock(255)."
};

enum ClockAccuracy
{
    CLOCK_ACCURACY_NS_25 = 32,         // The time is accurate to within 25 ns.
    CLOCK_ACCURACY_NS_100 = 33,        // The time is accurate to within 100 ns.
    CLOCK_ACCURACY_NS_250 = 34,        // The time is accurate to within 250 ns.
    CLOCK_ACCURACY_NS_1000 = 35,       // The time is accurate to within 1000 ns.
    CLOCK_ACCURACY_NS_2500 = 36,       // The time is accurate to within 2500 ns.

    CLOCK_ACCURACY_US_25 = 37,         // The time is accurate to within 25 us.
    CLOCK_ACCURACY_US_100 = 38,        // The time is accurate to within 100 us.
    CLOCK_ACCURACY_US_250 = 39,        // The time is accurate to within 250 us.
    CLOCK_ACCURACY_US_1000 = 40,       // The time is accurate to within 1000 us.
    CLOCK_ACCURACY_US_2500 = 41,       // The time is accurate to within 2500 us.

    CLOCK_ACCURACY_MS_25 = 42,         // The time is accurate to within 25 ms.
    CLOCK_ACCURACY_MS_100 = 43,        // The time is accurate to within 100 ms.
    CLOCK_ACCURACY_MS_250 = 44,        // The time is accurate to within 250 ms.

    CLOCK_ACCURACY_S_1 = 45,           // The time is accurate to within 1 s.
    CLOCK_ACCURACY_S_10 = 46,          // The time is accurate to within 10 s.
    CLOCK_ACCURACY_S_GREATER_10 = 46,  // The time is accurate to within 10 s.
    CLOCK_ACCURACY_UNKNOWN = 254       // Default indicating unknown
};

struct ClockQuality
{
    ClockClass clockClass;
    ClockAccuracy clockAccuracy;
    uint16_t offsetScaledLogVariance;
};

#define TIME_AWARE_SYSTEM_NETWORK  246 // Network infrastructure time-aware system
#define TIME_AWARE_SYSTEM_OTHER    248 // Other time-aware systems
#define TIME_AWARE_SYSTEM_PORTABLE 250 // Portable time-aware system
#define TIME_AWARE_SYSTEM_SLAVE    255 // Slave only time-aware system

enum ClockTimeSource
{
    CLOCK_TIME_SOURCE_ATOMIC = 0x10, // ATOMIC_CLOCK Any device, or device directly connected to such a device, that is based on atomic resonance for frequency and
                                     // that has been calibrated against international standards for frequency and time.

    /** To be defined... */

//    0x20 GPS Any device synchronized to any of the satellite systems
//    that distribute time and frequency tied to international
//    standards
//    0x30 TERRESTRIAL_RADIO Any device synchronized via any of the radio distribution
//    systems that distribute time and frequency tied to
//    international standards
//    0x40 PTP Any device synchronized to an IEEE 1588 PTP-based
//    source of time external to the gPTP domain. See NOTE.
//    0x50 NTP Any device synchronized via NTP to servers that distribute
//    time and frequency tied to international standards
//    0x60 HAND_SET Used in all cases for any device whose time has been set by
//    means of a human interface based on observation of an
//    international standards source of time to within the claimed
//    clock accuracy
//    0x90 OTHER Any source of time and/or frequency not covered by other
//    values, or for which the source is not known
//    0xA0 INTERNAL_OSCILLATOR Any device whose frequency is not based on atomic
//    resonance nor calibrated against international standards for
//    frequency, and whose time is based on a free-running
//    oscillator with epoch determined in an arbitrary or
//    unknown manner

};

enum PortRole
{
    /** Any port of the time-aware system for which portEnabled, pttPortEnabled, and asCapable are not all TRUE.*/
    PORT_ROLE_DISABLED = 3,
    /** Any port, P, of the time-aware system that is closer to the root than any other port of the gPTP communication path connected to P */
    PORT_ROLE_MASTER = 6,
    /** Any port of the time-aware system whose port role is not MasterPort, SlavePort, or DisabledPort. */
    PORT_ROLE_PASSIVE = 7,
    /** The one port of the time-aware system that is closest to the root time-aware system. If the root is grandmaster-capable, the SlavePort is
     * also closest to the grandmaster. The time- aware system does not transmit Sync or Announce messages on the SlavePort. */
    PORT_ROLE_SLAVE = 9
};

typedef UScaledNs MDTimestampReceive;

#endif // TYPES_H

