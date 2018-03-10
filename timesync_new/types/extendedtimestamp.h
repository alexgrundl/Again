#ifndef EXTENDEDTIMESTAMP_H
#define EXTENDEDTIMESTAMP_H

#include <stdint.h>
#include <math.h>

struct ScaledNs;
struct UScaledNs;
struct Timestamp;

struct ExtendedTimestamp
{
    uint64_t sec;
    /** The nanoseconds member is always less than 10^9 */
    uint32_t ns;
    uint16_t ns_frac;

    ExtendedTimestamp operator=(const Timestamp& ts);

    ExtendedTimestamp operator-(const ExtendedTimestamp& ts) const;

    ExtendedTimestamp operator+=(ScaledNs scaled);

    double operator/(const UScaledNs uscaled) const;

    operator ScaledNs() const;
};

#endif // EXTENDEDTIMESTAMP_H
