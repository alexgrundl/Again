#include "ptpconfig.h"

PtpConfig::PtpConfig()
{
    m_domain1Enabled = false;
    m_priority1Domain0 = 254;
    m_priority1Domain1 = 254;
    m_clockTimeSource = CLOCK_TIME_SOURCE_INTERNAL_OSCILLATOR;
    memset(m_rxDelays, -1, sizeof(m_rxDelays));
    memset(m_txDelays, -1, sizeof(m_txDelays));
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
            "[-D <10g_tx_delay,10g_rx_delay,gb_tx_delay,gb_rx_delay,mb_tx_delay,mb_rx_delay>]"
            "[-G]"
            "\n",
            arg0 );
    fprintf
        ( stderr,
          "\t-P <priority> priority value for all domains\n"
          "\t-P0 <priority> priority value for domain 0\n"
          "\t-P1 <priority> priority value for domain 1 (if enabled)\n"
          "\t-D1 enable domain 1\n"
          "\t-ML <mac address> Mac address of NIC containing the license\n"
          "\t-D Phy Delay <10g_tx_delay,10g_rx_delay,gb_tx_delay,gb_rx_delay,mb_tx_delay,mb_rx_delay>\n"
          "\t-G enable GPS time source of domain 0\n"
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
                    logwarning( "Priority value must be specified on "
                            "command line, using default value\n" );
                }
                else
                {
                    unsigned long tmp = strtoul( argv[i+1], NULL, 0 ); ++i;
                    if( tmp == 0 )
                    {
                        logwarning( "Invalid priority 1 value, using "
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
                    logerror( "Mac address has to be 17 characters (XX:XX:XX:XX:XX:XX)\n" );
                    argsOk = false;
                }
            }
            else if (strcmp(argv[i] + 1, "D") == 0)
            {
                uint32_t phy_delay[6];
                int delay_count=0;
                char *cli_inp_delay = strtok(argv[i+1],",");
                while (cli_inp_delay != NULL)
                {
                    if(delay_count>5)
                    {
                        logerror("Too many PHY delay values\n");
                        PrintUsage( argv[0] );
                        argsOk = false;
                        break;
                    }
                    phy_delay[delay_count]=atoi(cli_inp_delay);
                    delay_count++;
                    cli_inp_delay = strtok(NULL,",");
                }

                if (delay_count != 6)
                {
                    logerror("All six PHY delay values must be specified\n");
                    PrintUsage( argv[0] );
                    argsOk = false;
                    break;
                }

                for (int j = 0; j < 6; ++j)
                {
                    if(j % 2 == 0)
                        m_txDelays[j / 2] = phy_delay[j];
                    else
                        m_rxDelays[(j - 1) / 2] = phy_delay[j];
                }
            }
            else if( strcmp(argv[i] + 1,  "G") == 0 )
            {
                m_clockTimeSource = CLOCK_TIME_SOURCE_GPS;
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

int32_t PtpConfig::GetTxDelay(int speed)
{
    int32_t delay = -1;
    switch (speed)
    {
    case 10000:
        delay = m_txDelays[0];
        break;
    case 1000:
        delay = m_txDelays[1];
        break;
    case 100:
        delay = m_txDelays[2];
        break;
    default:
        logwarning("No Tx delay defined for speed %i\n", speed);
        break;
    }

    return delay;
}

int32_t PtpConfig::GetRxDelay(int speed)
{
    int32_t delay = -1;
    switch (speed)
    {
    case 10000:
        delay = m_rxDelays[0];
        break;
    case 1000:
        delay = m_rxDelays[1];
        break;
    case 100:
        delay = m_rxDelays[2];
        break;
    default:
        logwarning("No Tx delay defined for speed %i\n", speed);
        break;
    }

    return delay;
}

ClockTimeSource PtpConfig::GetClockTimeSource()
{
    return m_clockTimeSource;
}
