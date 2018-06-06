#include "ptpconfig.h"

PtpConfig::PtpConfig()
{
    m_domain1Enabled = false;
    m_priority1Domain0 = 254;
    m_priority1Domain1 = 254;
}

void PtpConfig::PrintUsage( char *arg0 )
{
    fprintf( stderr,
            "%s "
            "[-P <priority>] "
            "[-P0 <priority>] "
            "[-P1 <priority>] "
            "[-D1]"
            "[-ML <mac address>]"
//            "[-D <10g_tx_delay,10g_rx_delay,gb_tx_delay,gb_rx_delay,mb_tx_delay,mb_rx_delay>] "
            "\n",
            arg0 );
    fprintf
        ( stderr,
          "\t-P <priority> priority value for all domains\n"
          "\t-P0 <priority> priority value for domain 0\n"
          "\t-P1 <priority> priority value for domain 1 (if enabled)\n"
          "\t-D1 enable domain 1\n"
          "\t-ML <mac address> Mac address of NIC containing the license\n"
//          "\t-D Phy Delay <10g_tx_delay,10g_rx_delay,gb_tx_delay,gb_rx_delay,mb_tx_delay,mb_rx_delay>\n"
        );
}

bool PtpConfig::ParseArgs(int argc, char** argv)
{
    bool argsOk = true;

    /* Process optional arguments */
    for(int i = 1; i < argc; ++i )
    {

        if( argv[i][0] == '-' )
        {
            if( strcmp(argv[i] + 1,  "P") == 0 ||
                strcmp(argv[i] + 1,  "P0") == 0 ||
                strcmp(argv[i] + 1,  "P1") == 0)
            {
                bool setPriority0 = strcmp(argv[i] + 1,  "P") == 0 || strcmp(argv[i] + 1,  "P0") == 0;
                bool setPriority1 = strcmp(argv[i] + 1,  "P") == 0 || strcmp(argv[i] + 1,  "P1") == 0;
                if( i+1 >= argc )
                {
                    printf( "Priority value must be specified on "
                            "command line, using default value\n" );
                }
                else
                {
                    unsigned long tmp = strtoul( argv[i+1], NULL, 0 ); ++i;
                    if( tmp == 0 )
                    {
                        printf( "Invalid priority 1 value, using "
                                "default value\n" );
                        argsOk = false;
                    }
                    else
                    {
                        if(setPriority0)
                            m_priority1Domain0 = tmp;
                        if(setPriority1)
                            m_priority1Domain1 = tmp;
                    }
                }
            }
            else if( strcmp(argv[i] + 1,  "D1") == 0 )
            {
                m_domain1Enabled = true;
            }
            else if( strcmp(argv[i] + 1,  "H") == 0 )
            {
                PrintUsage( argv[0] );
                argsOk = false;
            }
            else if( strcmp(argv[i] + 1,  "ML") == 0 )
            {
                m_macLicense = std::string(argv[i+1]);
                if(m_macLicense.size() != 17)
                {
                    printf( "Mac address has to be 17 characters (XX:XX:XX:XX:XX:XX)\n" );
                    argsOk = false;
                }
            }
        }
    }

    return argsOk;
}

bool PtpConfig::IsDomain1Enabled()
{
    return m_domain1Enabled;
}

uint8_t PtpConfig::GetPriority1Domain0()
{
    return m_priority1Domain0;
}

uint8_t PtpConfig::GetPriority1Domain1()
{
    return m_priority1Domain1;
}

std::string PtpConfig::GetLicenseMac()
{
    return m_macLicense;
}
