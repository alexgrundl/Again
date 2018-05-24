#include "licensecheckwindows.h"

LicenseCheckWindows::LicenseCheckWindows()
{
    m_ctssEnabled = false;
    m_ptssEnabled = false;
    m_timeRelayEnabled = false;
}

LicenseCheckWindows::~LicenseCheckWindows()
{

}

bool LicenseCheckWindows::LicenseValid()
{
    return true;
}

bool LicenseCheckWindows::IsCTSSEnabled()
{
    return m_ctssEnabled;
}

bool LicenseCheckWindows::IsPTSSEnabled()
{
    return m_ptssEnabled;
}

bool LicenseCheckWindows::IsTimeRelayEnabled()
{
    return m_timeRelayEnabled;
}
