#include "scaledns.h"
#include "uscaledns.h"
#include "timestamp.h"
#include "extendedtimestamp.h"

Timestamp Timestamp::operator+(const ScaledNs& scaled) const
{
    Timestamp ts_new = *this;

    ts_new.sec += scaled.ns / NS_PER_SEC;
    uint32_t ns_new = ns + (scaled.ns % NS_PER_SEC);
    if(ns_new >= NS_PER_SEC)
    {
        ts_new.sec++;
        ns_new -= NS_PER_SEC;
    }
    ts_new.ns = ns_new;
    return ts_new;
}

Timestamp Timestamp::operator-(const Timestamp& ts) const
{
    Timestamp ts_new = *this;

    ts_new.sec -= ts.sec;
    uint32_t ns_new = ns >= ts.ns ? ns - ts.ns : NS_PER_SEC + ns - ts.ns;
    if(ns < ts.ns)
    {
        ts_new.sec--;
    }
    ts_new.ns = ns_new;
    return ts_new;
}

Timestamp Timestamp::operator/(int value) const
{
    Timestamp ts_new = *this;
    ts_new.sec /= value;
    ts_new.ns /= value;

    return ts_new;
}

Timestamp Timestamp::operator*=(double d)
{
    sec *= d;
    ns *= d;

    uint64_t secs_to_add = ns / NS_PER_SEC;
    ns -= secs_to_add * NS_PER_SEC;
    sec += secs_to_add;

    return *this;
}

Timestamp::operator ExtendedTimestamp() const
{
    ExtendedTimestamp timestamp;
    timestamp.sec = ns / NS_PER_SEC;
    timestamp.ns = ns % NS_PER_SEC;

    return timestamp;
}

bool Timestamp::operator>(const Timestamp& ts) const
{
    return sec > ts.sec || (sec == ts.sec && ns > ts.ns);
}

bool Timestamp::operator>=(const Timestamp& ts) const
{
    return sec > ts.sec || (sec == ts.sec && ns >= ts.ns);
}

bool Timestamp::operator<(const Timestamp& ts) const
{
    return sec < ts.sec || (sec == ts.sec && ns < ts.ns);
}

bool Timestamp::operator<=(const Timestamp& ts) const
{
    return sec < ts.sec || (sec == ts.sec && ns <= ts.ns);
}

