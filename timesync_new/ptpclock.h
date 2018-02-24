#ifndef PTPCLOCK_H
#define PTPCLOCK_H

#include <time.h>

#include "interfaces.h"

class PtpClock : public ClockSourceTime
{
public:
    PtpClock();


    void Invoke(ClockSourceTimeParams* params);
};

#endif // PTPCLOCK_H
