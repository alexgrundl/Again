#include "scaledns.h"
#include "uscaledns.h"
#include "timestamp.h"
#include "extendedtimestamp.h"

ScaledNs ScaledNs::operator*(double d) const
{
    ScaledNs scaled_new = *this;
    double dScaled = (double)ns + (double)ns_frac * pow(2, -16);

    dScaled *= d;
    scaled_new.ns = dScaled;
    scaled_new.ns_frac = (uint16_t)((dScaled - (double)ns) * 65536.0);

    return scaled_new;
}

ScaledNs ScaledNs::operator+(const ScaledNs& scaled) const
{
    ScaledNs scaled_new = *this;

    scaled_new.ns += scaled.ns;
    uint16_t ns_frac_new = ns_frac + scaled.ns_frac;
    if(ns_frac > ns_frac_new)
        scaled_new.ns++;
    scaled_new.ns_frac = ns_frac_new;
    return scaled_new;
}

ScaledNs ScaledNs::operator-(const ScaledNs& scaled) const
{
    ScaledNs scaled_new = *this;

    scaled_new.ns -= scaled.ns;
    uint16_t ns_frac_new = ns_frac - scaled.ns_frac;
    if(ns_frac < ns_frac_new)
        scaled_new.ns--;
    scaled_new.ns_frac = ns_frac_new;
    return scaled_new;
}

ScaledNs ScaledNs::operator+=(ScaledNs scaled)
{
    ns += scaled.ns;
    uint16_t ns_frac_new = ns_frac + scaled.ns_frac;
    if(ns_frac > ns_frac_new)
        ns++;
    ns_frac = ns_frac_new;

    return *this;
}

ScaledNs ScaledNs::operator/(double d) const
{
    ScaledNs scaled_new = *this;
    double dScaled = (double)ns + (double)ns_frac * pow(2, -16);

    dScaled /= d;
    scaled_new.ns = dScaled;
    scaled_new.ns_frac = (uint16_t)((dScaled - (double)ns) * 65536.0);

    return scaled_new;
}
