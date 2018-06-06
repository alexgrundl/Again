#ifndef LICENSECHECKWINDOWS_H
#define LICENSECHECKWINDOWS_H

#include "licensecheck.h"

class LicenseCheckWindows : public LicenseCheck
{

public:

    LicenseCheckWindows();


    ~LicenseCheckWindows();


    bool LicenseValid();


    bool IsCTSSEnabled();


    bool IsPTSSEnabled();


    bool IsTimeRelayEnabled();


    void SetMacWithLicense(std::string mac) {}

private:

    bool m_ctssEnabled;


    bool m_ptssEnabled;


    bool m_timeRelayEnabled;
};

#endif // LICENSECHECKWINDOWS_H
