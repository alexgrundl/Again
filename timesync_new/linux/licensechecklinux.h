#ifndef LICENSECHECKLINUX_H
#define LICENSECHECKLINUX_H

#ifdef __linux__

#include "licensecheck.h"

#include <net/if.h>
#include <linux/if_ether.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <ifaddrs.h>

#include "sha3.h"

class LicenseCheckLinux : public LicenseCheck
{
public:

    LicenseCheckLinux();


    virtual ~LicenseCheckLinux();


    bool LicenseValid();


    bool GetMacOfInterfaceWithLicense(char* mac, char* ifname);

private:


    char m_licenseIdentifier;


    bool GetMacFromInterfaceName(char* name, uint8_t* mac);


    uint32_t CountSetBits(uint32_t number);
};

#endif //__linux__

#endif // LICENSECHECKLINUX_H
