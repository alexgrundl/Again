#ifndef PTPCONFIG_H
#define PTPCONFIG_H

#include <string>

#include "types.h"

class PtpConfig
{

public:

    PtpConfig();


    bool ParseArgs(int argc, char** argv);


    bool IsDomain1Enabled();


    uint8_t GetPriority1Domain0();


    uint8_t GetPriority1Domain1();


    std::string GetLicenseMac();


    ClockTimeSource GetClockTimeSource();


    int32_t GetTxDelay(int speed);


    int32_t GetRxDelay(int speed);

private:


    bool m_domain1Enabled;


    uint8_t m_priority1Domain0;


    uint8_t m_priority1Domain1;


    std::string m_macLicense;


    ClockTimeSource m_clockTimeSource;


    int32_t m_rxDelays[3];


    int32_t m_txDelays[3];


    void PrintUsage(char *arg0);
};

#endif // PTPCONFIG_H
