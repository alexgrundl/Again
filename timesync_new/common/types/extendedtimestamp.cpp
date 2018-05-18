#include "scaledns.h"
#include "uscaledns.h"
#include "timestamp.h"
#include "extendedtimestamp.h"

ExtendedTimestamp ExtendedTimestamp::operator=(const Timestamp& ts)
{
    sec = ts.sec;
    ns = ts.ns;
    ns_frac = 0;

    return *this;
}

ExtendedTimestamp ExtendedTimestamp::operator-(const ExtendedTimestamp& ts) const
{
    ExtendedTimestamp ts_new = *this;
    ts_new.sec -= ts.sec;

    uint32_t ns_new = ns >= ts.ns ? ns - ts.ns : NS_PER_SEC + ns - ts.ns;
    if(ns < ts.ns)
    {
        ts_new.sec--;
    }
    ts_new.ns = ns_new;

    uint16_t ns_frac_new = ns_frac - ts.ns_frac;
    if(ns_frac < ns_frac_new)
        ts_new.ns--;
    ts_new.ns_frac = ns_frac_new;

    return ts_new;
}

ExtendedTimestamp ExtendedTimestamp::operator+=(ScaledNs scaled)
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

double ExtendedTimestamp::operator/(const UScaledNs uscaled) const
{
//        double dTs = (double)sec + ((double)ns + (double)ns_frac * pow(2, -16)) / 1000000000.0;
//        double dScaled = ((double)uscaled.ns + (double)uscaled.ns_frac * pow(2, -16)) / 1000000000.0;
//        double remainder;

//        dTs /= dScaled;
//        sec = (uint64_t)dTs;
//        remainder = (dTs - sec) * 1000000000.0;
//        ns = (uint32_t)remainder;
//        ns_frac = (uint16_t)((remainder - (double)ns) * 65536.0);

    double dTs = (double)sec * NS_PER_SEC + (double)ns + (double)ns_frac * pow(2, -16);
    double dScaled = (double)uscaled.ns + (double)uscaled.ns_frac * pow(2, -16);

    return dTs / dScaled;
}


ExtendedTimestamp::operator ScaledNs() const
{
    ScaledNs scaled;
    scaled.ns = sec * NS_PER_SEC + ns;
    scaled.ns_frac = ns_frac;
    return scaled;
}

ExtendedTimestamp::operator UScaledNs() const
{
    UScaledNs uscaled;
    uscaled.ns = sec * NS_PER_SEC + ns;
    uscaled.ns_frac = ns_frac;
    return uscaled;
}

