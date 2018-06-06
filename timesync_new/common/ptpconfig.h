#ifndef PTPCONFIG_H
#define PTPCONFIG_H

#include "types.h"

class PtpConfig
{

public:

    PtpConfig();


    bool ParseArgs(int argc, char** argv);


    bool IsDomain1Enabled();


    uint8_t GetPriority1Domain0();


    uint8_t GetPriority1Domain1();

private:


    bool m_domain1Enabled;


    uint8_t m_priority1Domain0;


    uint8_t m_priority1Domain1;


    void PrintUsage(char *arg0);
};

#endif // PTPCONFIG_H
