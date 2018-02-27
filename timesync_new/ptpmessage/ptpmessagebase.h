#ifndef PTPMESSAGEBASE_H
#define PTPMESSAGEBASE_H

#include <stdint.h>
#include <string.h>

#include "types.h"
#include "ptpmessagetype.h"

class PtpMessageBase
{
public:

    enum Flags
    {
        FLAG_LI61,
        FLAG_LI59,
        FLAG_UTC_REASONABLE,
        FLAG_TIMESCALE,
        FLAG_TIME_TRACABLE,
        FLAG_FREQUENCY_TRACABLE,
        FLAG_ALTERNATE_MASTER = 8,
        FLAG_TWO_STEP,
        FLAG_UNICAST,
        FLAG_PROFILE_SPECIFIC = 13,
        FLAG_SECURITY = 15
    };

    enum TlvType
    {
        TLV_TYPE_ORGANIZATION_EXTENSION = 3,
        TLV_TYPE_PATH_TRACE = 8
    };

    PtpMessageBase();


    virtual ~PtpMessageBase();


    static const uint8_t kMessageHeaderLength = 34;


    static const uint16_t kEtherType = 0x88F7;


    static const uint8_t DstMac[6];


    uint8_t GetVersionPTP();


    uint8_t GetTransportSpecific();


    PtpMessageType GetMessageType();


    uint16_t GetMessageLength();


    uint8_t GetDomainNumber();


    void SetDomainNumber(uint8_t value);


    uint16_t GetFlags();


    int64_t GetCorrectionField();


    void SetCorrectionField(int64_t value);


    PortIdentity GetSourcePortIdentity();


    void SetSourcePortIdentity(PortIdentity* value);


    uint16_t GetSequenceID();


    void SetSequenceID(uint16_t value);


    uint8_t GetControl();


    int8_t GetLogMessageInterval();


    void SetLogMessageInterval(int8_t value);


    UScaledNs GetReceiveTime();


    UScaledNs GetSendTime();


    void SetReceiveTime(UScaledNs value);


    void SetSendTime(UScaledNs value);


    virtual void GetMessage(uint8_t* bytes) = 0;


    void ParseHeader(const uint8_t* bytes);


    static PtpMessageType ParseMessageType(const uint8_t* bytes);


protected:

    uint8_t m_versionPTP;


    uint8_t m_transportSpecific;


    PtpMessageType m_messageType;


    uint16_t m_messageLength;


    uint8_t m_domainNumber;


    uint16_t m_flags;


    int64_t m_correctionField;


    PortIdentity m_sourcePortIdentity;


    uint16_t m_sequenceId;


    uint8_t m_control;


    int8_t m_logMessageInterval;


    UScaledNs m_receiveTime;


    UScaledNs m_sendTime;


    void GetHeader(uint8_t* bytes);
};

#endif // PTPMESSAGEBASE_H
