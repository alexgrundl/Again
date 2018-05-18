#include "licensechecklinux.h"

#ifdef __linux__

LicenseCheckLinux::LicenseCheckLinux()
{
    m_licenseIdentifier = '7';
}

LicenseCheckLinux::~LicenseCheckLinux()
{

}

bool LicenseCheckLinux::LicenseValid()
{
    const char invmBase[] = "/dev/invm_";
    char* invmPath;
    char mac[ETH_ALEN];
    char ifname[IFNAMSIZ];
    int fd;
    int errorCode = -1;

    if(GetMacOfInterfaceWithLicense(mac, ifname))
    {
        invmPath = (char*)malloc(sizeof(invmBase) + ETH_ALEN + 1);

        memcpy(invmPath, invmBase, sizeof(invmBase));
        strcat(invmPath, ifname);

        fd = open(invmPath, O_RDONLY);
        if(fd != -1)
        {
            uint32_t invm[64];
            read(fd, invm, sizeof(invm));
            close(fd);

            sha3_ctx ctx;
            rhash_sha3_256_init(&ctx);

            unsigned char hashdata[42] = { 0 };
            unsigned char result[32] = { 0 };

            //SET MAC to hashdata[0..5]
            hashdata[0] = mac[0];
            hashdata[1] = mac[1];
            hashdata[2] = mac[2];
            hashdata[3] = mac[3];
            hashdata[4] = mac[4];
            hashdata[5] = mac[5];

            invm[62] = __builtin_bswap32(invm[62]);
            memcpy(hashdata + 6, &invm[62], 4);

            invm[63] = __builtin_bswap32(invm[63]);
            memcpy(hashdata + 10, &invm[63], 4);

            invm[58] = __builtin_bswap32(invm[58]);
            memcpy(hashdata + 14, &invm[58], 4);

            invm[59] = __builtin_bswap32(invm[59]);
            memcpy(hashdata + 18, &invm[59], 4);

            invm[60] = __builtin_bswap32(invm[60]);
            memcpy(hashdata + 22, &invm[60], 4);

            invm[61] = __builtin_bswap32(invm[61]);
            memcpy(hashdata + 26, &invm[61], 4);

            invm[4] = __builtin_bswap32(invm[4]);
            memcpy(hashdata + 30, &invm[4], 4);

            //SET BPLUS SALT to hashdata[34..41]:
            hashdata[34] = 0x42;  //'B'
            hashdata[35] = 0x50;  //'P'
            hashdata[36] = 0x4C;  //'L'
            hashdata[37] = 0x55;  //'U'
            hashdata[38] = 0x53;  //'S'
            hashdata[39] = 0x43;  //'C'
            hashdata[40] = 0x42;  //'B'
            hashdata[41] = 0x4F;  //'O'

            rhash_sha3_update(&ctx, hashdata, 42);
            rhash_sha3_final(&ctx, result);


            uint32_t counter;

            if(invm[5] >= 0x20)
            {
                logerror("License count invalid.");
                errorCode = -10; // NO VALID LICENSE
            }
            else
            {
                counter = 6 - CountSetBits(invm[5]);

                //CHECK BPLUS LICENSE (Compare requested license with self calculated license):
                unsigned char license[32];
                for (int i = 0; i < 8; i++)
                {
                  invm[6+(counter*8) + i] = __builtin_bswap32(invm[6+(counter*8) + i]);
                  memcpy(license + i * 4, &invm[6+(counter*8) + i], 4);
                }

                if (memcmp(license, result, 32) != 0)
                {
                    logerror("License invalid.");
                    errorCode = -10; // NO VALID LICENSE
                }
                else
                {
                    lognotice("License good.");
                    errorCode = 0;
                }
            }
        }
        else
            logerror("License checking: invm driver not loaded.");

        free(invmPath);
    }
    else
        logerror("Network interface to check license not found.");
    return errorCode == 0;
}

bool LicenseCheckLinux::GetMacFromInterfaceName(char* name, uint8_t* mac)
{
    struct ifreq s;
    int fd = socket(PF_PACKET, SOCK_DGRAM, 0);
    bool success = false;

    strcpy(s.ifr_name, name);
    if(ioctl(fd, SIOCGIFHWADDR, &s) == 0)
    {
        success = true;
        memcpy(mac, s.ifr_addr.sa_data, ETH_ALEN);
    }

    return success;
}

bool LicenseCheckLinux::GetMacOfInterfaceWithLicense(char* mac, char* ifname)
{
    bool macFound = false;
    struct ifaddrs* addrs, *next;
    uint8_t currentMac[ETH_ALEN];
    char strLastMac[3];

    getifaddrs(&addrs);
    next = addrs;

    while(next)
    {
        if(next->ifa_addr != NULL && next->ifa_addr->sa_family == AF_PACKET && (next->ifa_flags & IFF_LOOPBACK) == 0)
        {
            if(GetMacFromInterfaceName(next->ifa_name, currentMac) &&
                    snprintf(strLastMac, 3, "%02x", currentMac[ETH_ALEN - 1]) > 0 && strLastMac[1] == m_licenseIdentifier)
            {
                memcpy(mac, currentMac, ETH_ALEN);
                memcpy(ifname, next->ifa_name, strlen(next->ifa_name) + 1);
                macFound = true;
                break;
            }
        }
        next = next->ifa_next;
    }
    freeifaddrs(addrs);

    return macFound;
}

uint32_t LicenseCheckLinux::CountSetBits(uint32_t number)
{
    //Hammering weight
     number = number - ((number >> 1) & 0x55555555);
     number = (number & 0x33333333) + ((number >> 2) & 0x33333333);
     return (((number + (number >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}

#endif //__linux__
