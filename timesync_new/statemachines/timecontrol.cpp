#include "timecontrol.h"

TimeControl::TimeControl()
{
    m_ptpClock = NULL;
    m_integral = 0.0003;
    m_proportional = 1.0;
}

void TimeControl::SetPtpClock(PtpClock* ptpClock)
{
    m_ptpClock = ptpClock;
}

void TimeControl::Syntonize(ScaledNs masterLocalOffset, double remoteLocalRate)
{
    static float ppm = 0;
    if(m_ptpClock != NULL)
    {
        if(abs(masterLocalOffset.ns) >= NS_PER_SEC / 1000)
        {
            m_ptpClock->AdjustPhase(masterLocalOffset.ns);

            m_ptpClock->StopPPS();
            m_ptpClock->StartPPS();
        }
        else
        {
            float syncPerSec = 1.0 / pow(2, -3);
            ppm += (m_integral * syncPerSec * masterLocalOffset.ns) + m_proportional * (remoteLocalRate - 1.0)*1000000;

            if(ppm < -250.0)
                ppm = -250.0;
            if(ppm > 250.0)
                ppm = 250.0;

//            printf("PPM: %f\n", ppm);
//            printf("remoteLocalRate: %f\n", remoteLocalRate);
            m_ptpClock->AdjustFrequency(ppm);
        }
    }
}
