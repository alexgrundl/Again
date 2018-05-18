#ifndef USCALEDNS_H
#define USCALEDNS_H

#include <stdint.h>
#include <math.h>

struct ScaledNs;
struct Timestamp;
struct ExtendedTimestamp;

struct UScaledNs
{
    uint64_t ns;
    uint16_t ns_frac;

    UScaledNs operator+(const UScaledNs& uscaled) const;

    UScaledNs operator-(const UScaledNs& uscaled) const;

    UScaledNs operator*(double d) const;

    UScaledNs operator/(double d) const;

    double operator/(const UScaledNs uscaled) const;

    operator ScaledNs() const;

    operator Timestamp() const;

    operator ExtendedTimestamp() const;

    bool operator>(const UScaledNs& uscaled) const;

    bool operator>=(const UScaledNs& uscaled) const;

    bool operator<(const UScaledNs& uscaled) const;

    bool operator<=(const UScaledNs& uscaled) const;
};

#endif // USCALEDNS_H
