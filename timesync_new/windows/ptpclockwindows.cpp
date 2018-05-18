#include "ptpclockwindows.h"

#ifndef __linux__

PtpClockWindows::PtpClockWindows()
{
    m_ptssType = PTSS_TYPE_SLAVE;
}

PtpClockWindows::~PtpClockWindows()
{

}

PtpClock::PtssType PtpClockWindows::GetPtssType()
{
    return m_ptssType;
}

void PtpClockWindows::SetPtssType(PtssType type)
{
    m_ptssType = type;
}

#endif //__linux__
