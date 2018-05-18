#ifndef LICENSECHECKWINDOWS_H
#define LICENSECHECKWINDOWS_H

#include "licensecheck.h"

class LicenseCheckWindows : public LicenseCheck
{

public:

    LicenseCheckWindows();


    ~LicenseCheckWindows();


    bool LicenseValid();
};

#endif // LICENSECHECKWINDOWS_H
