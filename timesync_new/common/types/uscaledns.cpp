#include "uscaledns.h"
#include "scaledns.h"
#include "timestamp.h"
#include "extendedtimestamp.h"

UScaledNs UScaledNs::operator+(const UScaledNs& uscaled) const
{
    UScaledNs uscaled_new = *this;

    uscaled_new.ns += uscaled.ns;
    uint16_t ns_frac_new = ns_frac + uscaled.ns_frac;
    if(ns_frac > ns_frac_new)
        uscaled_new.ns++;
    uscaled_new.ns_frac = ns_frac_new;
    return uscaled_new;
}

UScaledNs UScaledNs::operator-(const UScaledNs& uscaled) const
{
    UScaledNs uscaled_new = *this;

    uscaled_new.ns -= uscaled.ns;
    uint16_t ns_frac_new = ns_frac - uscaled.ns_frac;
    if(ns_frac < ns_frac_new)
        uscaled_new.ns--;
    uscaled_new.ns_frac = ns_frac_new;
    return uscaled_new;
}

UScaledNs UScaledNs::operator*(double d) const
{
    UScaledNs uscaled_new;
    double dScaled = (double)ns + (double)ns_frac * pow(2, -16);

    dScaled *= d;
    uscaled_new.ns = dScaled;
    uscaled_new.ns_frac = (uint16_t)((dScaled - (double)uscaled_new.ns) * 65536.0);

    return uscaled_new;
}

UScaledNs UScaledNs::operator/(double d) const
{
    UScaledNs uscaled_new;
    double dScaled = (double)ns + (double)ns_frac * pow(2, -16);

    dScaled /= d;
    uscaled_new.ns = dScaled;
    uscaled_new.ns_frac = (uint16_t)((dScaled - (double)ns) * 65536.0);

    return uscaled_new;
}

double UScaledNs::operator/(const UScaledNs uscaled) const
{
    double dTs = (double)ns + (double)ns_frac * pow(2, -16);
    double dScaled = (double)uscaled.ns + (double)uscaled.ns_frac * pow(2, -16);

    return dTs / dScaled;
}

UScaledNs::operator ScaledNs() const
{
    ScaledNs scaled;
    scaled.ns = ns;
    scaled.ns_frac = ns_frac;
    return scaled;
}

UScaledNs::operator Timestamp() const
{
    Timestamp timestamp;
    timestamp.sec = ns / NS_PER_SEC;
    timestamp.ns = ns % NS_PER_SEC;

    return timestamp;
}

UScaledNs::operator ExtendedTimestamp() const
{
    ExtendedTimestamp timestamp;
    timestamp.sec = ns / NS_PER_SEC;
    timestamp.ns = ns % NS_PER_SEC;
    timestamp.ns_frac = ns_frac;

    return timestamp;
}

bool UScaledNs::operator>(const UScaledNs& uscaled) const
{
    return ns > uscaled.ns || (ns == uscaled.ns && ns_frac > uscaled.ns_frac);
}

bool UScaledNs::operator>=(const UScaledNs& uscaled) const
{
    return ns > uscaled.ns || (ns == uscaled.ns && ns_frac >= uscaled.ns_frac);
}

bool UScaledNs::operator<(const UScaledNs& uscaled) const
{
    return ns < uscaled.ns || (ns == uscaled.ns && ns_frac < uscaled.ns_frac);
}

bool UScaledNs::operator<=(const UScaledNs& uscaled) const
{
    return ns < uscaled.ns || (ns == uscaled.ns && ns_frac <= uscaled.ns_frac);
}

