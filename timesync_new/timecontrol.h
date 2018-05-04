#ifndef TIMECONTROL_H
#define TIMECONTROL_H

#include <stdlib.h>

#include "ptpclock.h"

class TimeControl
{
public:

    TimeControl();


    void SetPtpClock(PtpClock* ptpClock);


    void Syntonize(ScaledNs masterLocalOffset, double remoteLocalRate);

private:

    double m_integral;

    double m_proportional;

    PtpClock* m_ptpClock;
};

#endif // TIMECONTROL_H
