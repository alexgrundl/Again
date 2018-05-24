#include "timecontrol.h"

TimeControl::TimeControl()
{
    m_ptpClock = NULL;
    m_integral = 0.0002;
    m_proportional = 0.5;

    m_oldBestSource = 0;
    m_frequencyIntergral = 0;
    m_ppm = 0;
}

void TimeControl::SetPtpClock(PtpClock* ptpClock)
{
    m_ptpClock = ptpClock;
    if(m_ptpClock != NULL)
        m_ptpClock->AdjustFrequency(0);
}

void TimeControl::Syntonize(ScaledNs masterLocalOffset, double remoteLocalRate, int8_t logSyncPerSec)
{
    if(m_ptpClock != NULL)
    {
        if(abs(masterLocalOffset.ns) >= NS_PER_SEC / 1000)
        {
            m_ptpClock->AdjustPhase(masterLocalOffset.ns);

            //m_ptpClock->StopPPS();
            if(m_ptpClock->GetPtssType() == PtpClock::PTSS_TYPE_ROOT)
                m_ptpClock->StartPPS();
        }
        else
        {
            float syncPerSec = 1.0 / pow(2, logSyncPerSec);
            m_ppm += (m_integral * syncPerSec * masterLocalOffset.ns) + m_proportional * (remoteLocalRate - 1.0)*1000000;

            if(m_ppm < -250.0)
                m_ppm = -250.0;
            if(m_ppm > 250.0)
                m_ppm = 250.0;

            //printf("PPM: %f\n", ppm);
            //printf("remoteLocalRate: %f\n", remoteLocalRate);
            m_ptpClock->AdjustFrequency(m_ppm);
        }
    }
}

int64_t TimeControl::Syntonize(PtpClock *localClock, struct timespec& tsExtEvent, struct timespec& tsSystemOfEvent)
{
    struct timespec tsSystem, tsDevice;
    uint64_t nsExtEvent, nsDevice, nsOffsetSystem;
    int64_t nsOffset = -1;
    int64_t remainder;
    float periodInSec = 0.25;

    localClock->GetSystemAndDeviceTime(&tsSystem, &tsDevice);

    nsOffsetSystem = abs((int64_t)tsSystem.tv_sec * NS_PER_SEC + tsSystem.tv_nsec - tsSystemOfEvent.tv_sec * NS_PER_SEC - tsSystemOfEvent.tv_nsec);
    //printf("tsDevice:   %lu\n", (uint64_t)tsDevice.tv_sec * NS_PER_SEC + tsDevice.tv_nsec);

    //System time offset from now to the time the SDP external timestamp was signaled has to be less than 100 ms.
    if(nsOffsetSystem < (int)(0.1 * NS_PER_SEC))
    {
        remainder = tsDevice.tv_nsec % (int64_t)(NS_PER_SEC * periodInSec);

        //Set device time to a time divisible by "periodInSec" plus a small tolerance. This has to be the time (in the timescale of our PPS - root)
        //when the PPS signal arrived at the PPS slave. If the time is more than 100 ms away from a time divisible by "periodInSec" do nothing.
//        if(remainder < (int)(0.1 * NS_PER_SEC))
//        {
            uint64_t nsecNew = tsDevice.tv_nsec / (int64_t)(NS_PER_SEC * periodInSec);
            nsecNew = nsecNew * (int64_t)(NS_PER_SEC * periodInSec);// + 6;
            tsDevice.tv_nsec = nsecNew;
//        }
//        else
//            return nsOffset;

        nsExtEvent = (uint64_t)tsExtEvent.tv_sec * NS_PER_SEC + tsExtEvent.tv_nsec;
        nsDevice = (uint64_t)tsDevice.tv_sec * NS_PER_SEC + tsDevice.tv_nsec;
        //nsBestOffset = nsExtEvent > nsDevice ? nsExtEvent - nsDevice : nsDevice - nsExtEvent;
        nsOffset = nsExtEvent - nsDevice;

//        printf("tsExtEvent: %lu\n", (uint64_t)tsExtEvent.tv_sec * NS_PER_SEC + tsExtEvent.tv_nsec);
//        printf("tsDevice c: %lu\n", (uint64_t)tsDevice.tv_sec * NS_PER_SEC + tsDevice.tv_nsec);
        ControlTimePPS(nsOffset, (uint64_t)tsDevice.tv_sec * NS_PER_SEC + tsDevice.tv_nsec, (uint64_t)tsExtEvent.tv_sec * NS_PER_SEC + tsExtEvent.tv_nsec);
    }

    return nsOffset;
}

void TimeControl::ControlTimePPS(int64_t offset, uint64_t ppsSlaveTime, uint64_t ppsRootTime)
{
    if(m_oldBestSource > 0 && ppsRootTime - m_oldBestSource != 0)
    {
        float periodInSec = 0.25;
        float rate = periodInSec * NS_PER_SEC / (ppsRootTime - m_oldBestSource);
        Syntonize(ScaledNs({-offset, 0}), rate, log2(periodInSec));
    }
    m_oldBestSource = ppsRootTime;


//    if(bestOffset > 30000000)
//    {
//        struct timespec tsSystem, tsDevice, tsSystemAfter, tsDeviceNew;
//        uint64_t nsSystemDiff, nsDeviceNew;

//        m_oldBestSource = bestSource;
//        m_ptpClock->AdjustFrequency(0);
//        m_ptpClock->GetSystemAndDeviceTime(&tsSystem, &tsDevice);
//        clock_gettime(CLOCK_REALTIME, &tsSystemAfter);
//        nsSystemDiff = (uint64_t)tsSystemAfter.tv_sec * NS_PER_SEC + tsSystemAfter.tv_nsec - tsSystem.tv_sec * NS_PER_SEC - tsSystem.tv_nsec;

//        if(bestLocal > bestSource)
//            nsDeviceNew = (uint64_t)tsDevice.tv_sec * NS_PER_SEC + tsDevice.tv_nsec - bestOffset + nsSystemDiff;
//        else
//            nsDeviceNew = (uint64_t)tsDevice.tv_sec * NS_PER_SEC + tsDevice.tv_nsec + bestOffset + nsSystemDiff;

//        tsDeviceNew.tv_sec = nsDeviceNew / NS_PER_SEC;
//        tsDeviceNew.tv_nsec = nsDeviceNew % NS_PER_SEC;
//        m_ptpClock->SetTime(&tsDeviceNew);
//    }
//    else
//    {
//        if(bestSource - m_oldBestSource != 0)
//        {
//            printf("bestSource: %lu\n", bestSource);
//            int64_t offsetCorrection = (bestOffset << 32) * 8 / ((int64_t)bestSource - m_oldBestSource);
//            m_frequencyIntergral = bestLocal > bestSource ? m_frequencyIntergral - offsetCorrection : m_frequencyIntergral + offsetCorrection;
//            int64_t proportionalPart = bestLocal > bestSource ? -1 * offsetCorrection * 70 / 100 : offsetCorrection * 70 / 100;
//            int64_t intergralPart = m_frequencyIntergral * 30 / 100;

//            m_oldBestSource = bestSource;
//            printf("proportionalPart + intergralPart: %f\n", (double)(proportionalPart + intergralPart));
//            m_ptpClock->AdjustFrequency(proportionalPart + intergralPart);
//        }
//        else
//            printf("bestSource - m_oldBestSource is zero\n");
//    }
}
