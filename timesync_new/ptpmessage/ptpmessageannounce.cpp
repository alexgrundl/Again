#include "ptpmessageannounce.h"

PtpMessageAnnounce::PtpMessageAnnounce()
{
    m_messageType = PTP_MESSSAGE_TYPE_ANNOUNCE;
    m_messageLength = 68;

    m_currentUtcOffset = 0;
    m_grandmasterPriority1 = 255;
    m_grandmasterClockQuality.clockAccuracy = CLOCK_ACCURACY_UNKNOWN;
    m_grandmasterClockQuality.clockClass = CLOCK_CLASS_APPLICATION_LOST;
    m_grandmasterClockQuality.offsetScaledLogVariance = 0;
    m_grandmasterPriority2 = 255;
    memset(m_grandmasterIdentity, 0, sizeof(m_grandmasterIdentity));
    m_stepsRemoved = 0;
    m_timeSource = CLOCK_TIME_SOURCE_INTERNAL_OSCILLATOR;

    m_tlv.tlvType = TLV_TYPE_PATH_TRACE;
    m_tlv.lengthField = 0;
}

PtpMessageAnnounce::~PtpMessageAnnounce()
{
    for (std::vector<uint8_t*>::size_type i = 0; i < m_tlv.pathSequence.size(); ++i)
    {
        delete[] m_tlv.pathSequence[i];
    }
    m_tlv.pathSequence.clear();
}

int16_t PtpMessageAnnounce::GetCurrentUtcOffset()
{
    return m_currentUtcOffset;
}

void PtpMessageAnnounce::SetCurrentUtcOffset(int16_t offset)
{
    m_currentUtcOffset = offset;
}

uint8_t PtpMessageAnnounce::GetGrandmasterPriority1()
{
    return m_grandmasterPriority1;
}

void PtpMessageAnnounce::SetGrandmasterPriority1(uint8_t priority)
{
    m_grandmasterPriority1 = priority;
}

ClockQuality PtpMessageAnnounce::GetGrandmasterClockQuality()
{
    return m_grandmasterClockQuality;
}

void PtpMessageAnnounce::SetGrandmasterClockQuality(ClockQuality quality)
{
    m_grandmasterClockQuality = quality;
}

uint8_t PtpMessageAnnounce::GetGrandmasterPriority2()
{
    return m_grandmasterPriority2;
}

void PtpMessageAnnounce::SetGrandmasterPriority2(uint8_t priority)
{
    m_grandmasterPriority2 = priority;
}

void PtpMessageAnnounce::GetGrandmasterIdentity(uint8_t* identity)
{
    memcpy(identity, m_grandmasterIdentity, CLOCK_ID_LENGTH);
}

void PtpMessageAnnounce::SetGrandmasterIdentity(uint8_t* identity)
{
    memcpy(m_grandmasterIdentity, identity, CLOCK_ID_LENGTH);
}

uint16_t PtpMessageAnnounce::GetStepsRemoved()
{
    return m_stepsRemoved;
}

void PtpMessageAnnounce::SetStepsRemoved(uint16_t steps)
{
    m_stepsRemoved = steps;
}

ClockTimeSource PtpMessageAnnounce::GetTimeSource()
{
    return m_timeSource;
}

void PtpMessageAnnounce::SetTimeSource(ClockTimeSource timeSource)
{
    m_timeSource = timeSource;
}

PtpMessageAnnounce::AnnounceTLV PtpMessageAnnounce::GetTLV()
{
    return m_tlv;
}

std::vector<uint8_t*> PtpMessageAnnounce::GetPathSequence()
{
    return m_tlv.pathSequence;
}

void PtpMessageAnnounce::SetPathSequence(std::vector<uint8_t*> pathSequence)
{
    for (std::vector<uint8_t*>::size_type i = 0; i < m_tlv.pathSequence.size(); ++i)
    {
        delete[] m_tlv.pathSequence[i];
    }
    m_tlv.pathSequence.clear();
    m_messageLength = 68;

    m_tlv.lengthField = pathSequence.size() * 8;
    for (std::vector<uint8_t*>::size_type i = 0; i < pathSequence.size(); ++i)
    {
        uint8_t* clockIdentity = new uint8_t[CLOCK_ID_LENGTH];
        memcpy(clockIdentity, pathSequence[i], CLOCK_ID_LENGTH);
        m_tlv.pathSequence.push_back(clockIdentity);
        m_messageLength += 8;
    }
}

bool PtpMessageAnnounce::GetFlagLeap61()
{
    return (m_flags & (1 << FLAG_LI61)) == FLAG_LI61;
}

void PtpMessageAnnounce::SetFlagLeap61(bool enable)
{
    m_flags = enable ? m_flags | (1 << FLAG_LI61) : m_flags &~(1 << FLAG_LI61);
}

bool PtpMessageAnnounce::GetFlagLeap59()
{
    return (m_flags & (1 << FLAG_LI59)) == FLAG_LI59;
}

void PtpMessageAnnounce::SetFlagLeap59(bool enable)
{
    m_flags = enable ? m_flags | (1 << FLAG_LI59) : m_flags &~(1 << FLAG_LI59);
}

bool PtpMessageAnnounce::GetCurrentUtcOffsetValid()
{
    return (m_flags & (1 << FLAG_UTC_REASONABLE)) == FLAG_UTC_REASONABLE;
}

void PtpMessageAnnounce::SetCurrentUtcOffsetValid(bool enable)
{
    m_flags = enable ? m_flags | (1 << FLAG_UTC_REASONABLE) : m_flags &~(1 << FLAG_UTC_REASONABLE);
}

bool PtpMessageAnnounce::GetTimeTraceable()
{
    return (m_flags & (1 << FLAG_TIME_TRACABLE)) == FLAG_TIME_TRACABLE;
}

void PtpMessageAnnounce::SetTimeTraceable(bool enable)
{
    m_flags = enable ? m_flags | (1 << FLAG_TIME_TRACABLE) : m_flags &~(1 << FLAG_TIME_TRACABLE);
}

bool PtpMessageAnnounce::GetFrequencyTraceable()
{
    return (m_flags & (1 << FLAG_FREQUENCY_TRACABLE)) == FLAG_FREQUENCY_TRACABLE;
}

void PtpMessageAnnounce::SetFrequencyTraceable(bool enable)
{
    m_flags = enable ? m_flags | (1 << FLAG_FREQUENCY_TRACABLE) : m_flags &~(1 << FLAG_FREQUENCY_TRACABLE);
}

void PtpMessageAnnounce::GetMessage(uint8_t *bytes)
{
    GetHeader(bytes);

    bytes[kMessageHeaderLength + 10] = m_currentUtcOffset << 8;
    bytes[kMessageHeaderLength + 11] = m_currentUtcOffset;
    bytes[kMessageHeaderLength + 13] = m_grandmasterPriority1;
    bytes[kMessageHeaderLength + 14] = m_grandmasterClockQuality.clockClass;
    bytes[kMessageHeaderLength + 15] = m_grandmasterClockQuality.clockAccuracy;
    bytes[kMessageHeaderLength + 16] = m_grandmasterClockQuality.offsetScaledLogVariance << 8;
    bytes[kMessageHeaderLength + 17] = m_grandmasterClockQuality.offsetScaledLogVariance;
    bytes[kMessageHeaderLength + 18] = m_grandmasterPriority2;
    memcpy(bytes + kMessageHeaderLength + 19, m_grandmasterIdentity, CLOCK_ID_LENGTH);
    bytes[kMessageHeaderLength + 27] = m_stepsRemoved << 8;
    bytes[kMessageHeaderLength + 28] = m_stepsRemoved;
    bytes[kMessageHeaderLength + 29] = m_timeSource;
    bytes[kMessageHeaderLength + 30] = m_tlv.tlvType << 8;
    bytes[kMessageHeaderLength + 31] = m_tlv.tlvType;
    bytes[kMessageHeaderLength + 32] = m_tlv.lengthField << 8;
    bytes[kMessageHeaderLength + 33] = m_tlv.lengthField;
    for (std::vector<uint8_t*>::size_type i = 0; i < m_tlv.pathSequence.size(); ++i)
    {
       memcpy(bytes + kMessageHeaderLength + 34 + 8 * i, m_tlv.pathSequence[i], CLOCK_ID_LENGTH);
    }
}

void PtpMessageAnnounce::ParsePackage(const uint8_t* bytes)
{
    ParseHeader(bytes);

    m_currentUtcOffset = (bytes[kMessageHeaderLength + 10] << 8) + bytes[kMessageHeaderLength + 11];
    m_grandmasterPriority1 = bytes[kMessageHeaderLength + 13];
    m_grandmasterClockQuality.clockClass = (ClockClass)bytes[kMessageHeaderLength + 14];
    m_grandmasterClockQuality.clockAccuracy = (ClockAccuracy)bytes[kMessageHeaderLength + 15];
    m_grandmasterClockQuality.offsetScaledLogVariance = (bytes[kMessageHeaderLength + 16] << 8) + bytes[kMessageHeaderLength + 17];
    m_grandmasterPriority2 = bytes[kMessageHeaderLength + 18];
    memcpy(m_grandmasterIdentity, bytes + kMessageHeaderLength + 19, CLOCK_ID_LENGTH);
    m_stepsRemoved = (bytes[kMessageHeaderLength + 27] << 8) + bytes[kMessageHeaderLength + 28];
    m_timeSource = (ClockTimeSource)bytes[kMessageHeaderLength + 29];
    m_tlv.tlvType = (TlvType)((bytes[kMessageHeaderLength + 30] << 8) + bytes[kMessageHeaderLength + 31]);
    m_tlv.lengthField = (bytes[kMessageHeaderLength + 32] << 8) + bytes[kMessageHeaderLength + 33];

    for (std::vector<uint8_t*>::size_type i = 0; i < m_tlv.pathSequence.size(); ++i)
    {
        delete[] m_tlv.pathSequence[i];
    }
    m_tlv.pathSequence.clear();
    m_messageLength = 68;

    for (int i = 0; i < m_tlv.lengthField; i+=CLOCK_ID_LENGTH)
    {
        uint8_t* clockIdentity = new uint8_t[CLOCK_ID_LENGTH];
        memcpy(clockIdentity, bytes + kMessageHeaderLength + 34 + 8 * i, CLOCK_ID_LENGTH);
        m_tlv.pathSequence.push_back(clockIdentity);
        m_messageLength += CLOCK_ID_LENGTH;
    }
}
