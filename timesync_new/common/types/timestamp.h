#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include <stdint.h>
#include <math.h>

struct ScaledNs;
struct UScaledNs;
struct ExtendedTimestamp;

struct Timestamp
{
    uint64_t sec;
    /** The nanoseconds member is always less than 10^9 */
    uint32_t ns;

    Timestamp operator+(const ScaledNs& scaled) const;

    Timestamp operator-(const Timestamp& ts) const;

    Timestamp operator/(int value) const;

    Timestamp operator*=(double d);

    operator ExtendedTimestamp() const;

    bool operator>(const Timestamp& ts) const;

    bool operator>=(const Timestamp& ts) const;

    bool operator<(const Timestamp& ts) const;

    bool operator<=(const Timestamp& ts) const;
};

#endif // TIMESTAMP_H
