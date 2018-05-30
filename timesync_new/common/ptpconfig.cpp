#include "ptpconfig.h"

PtpConfig::PtpConfig(TimeSyncDaemon* timeSyncDaemon)
{
    m_timeSyncDaemon = timeSyncDaemon;
}

void PtpConfig::PrintUsage( char *arg0 )
{
    fprintf( stderr,
            "%s "
            "[-P <priority>] "
            "[-P0 <priority>] "
            "[-P1 <priority>] "
//            "[-D <10g_tx_delay,10g_rx_delay,gb_tx_delay,gb_rx_delay,mb_tx_delay,mb_rx_delay>] "
            "\n",
            arg0 );
    fprintf
        ( stderr,
          "\t-P <priority> priority value for all domains\n"
          "\t-P0 <priority> priority value for domain 0\n"
          "\t-P1 <priority> priority value for domain 1\n"
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
                            m_timeSyncDaemon->GetTimeAwareSystem(0)->SetSystemPriority1(tmp);
                        if(setPriority1)
                            m_timeSyncDaemon->GetTimeAwareSystem(1)->SetSystemPriority1(tmp);
                    }
                }
            }
            else if( strcmp(argv[i] + 1,  "H") == 0 )
            {
                PrintUsage( argv[0] );
                argsOk = false;
            }
        }
    }

    return argsOk;
}
