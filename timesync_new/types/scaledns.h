#ifndef SCALEDNS_H
#define SCALEDNS_H

#include <stdint.h>
#include <math.h>

#define NS_PER_SEC 1000000000

struct UScaledNs;
struct Timestamp;
struct ExtendedTimestamp;

struct ScaledNs
{
    int64_t ns;
    uint16_t ns_frac;

    ScaledNs operator*(double d) const;

    ScaledNs operator+(const ScaledNs& scaled) const;

    ScaledNs operator+=(ScaledNs scaled);

    ScaledNs operator/(double d) const;

};

#endif // SCALEDNS_H
