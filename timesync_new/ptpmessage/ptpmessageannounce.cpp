#include "ptpmessageannounce.h"

PtpMessageAnnounce::PtpMessageAnnounce()
{
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
        delete m_tlv.pathSequence[i];
    }
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
    memcpy(identity, m_grandmasterIdentity, sizeof(m_grandmasterIdentity));
}

void PtpMessageAnnounce::SetGrandmasterIdentity(uint8_t* identity)
{
    memcpy(m_grandmasterIdentity, identity, sizeof(m_grandmasterIdentity));
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

void PtpMessageAnnounce::SetPathSequence(std::vector<uint8_t*> pathSequence)
{
    for (std::vector<uint8_t*>::size_type i = 0; i < m_tlv.pathSequence.size(); ++i)
    {
        delete m_tlv.pathSequence[i];
    }

    m_tlv.lengthField = pathSequence.size() * 8;
    for (std::vector<uint8_t*>::size_type i = 0; i < pathSequence.size(); ++i)
    {
        uint8_t* clockIdentity = new uint8_t[8];
        memcpy(clockIdentity, pathSequence[i], 8);
        m_tlv.pathSequence.push_back(clockIdentity);
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
    memcpy(m_grandmasterIdentity, bytes + kMessageHeaderLength + 19, sizeof(m_grandmasterIdentity));
    m_stepsRemoved = (bytes[kMessageHeaderLength + 27] << 8) + bytes[kMessageHeaderLength + 28];
    m_timeSource = (ClockTimeSource)bytes[kMessageHeaderLength + 29];
    m_tlv.tlvType = (TlvType)((bytes[kMessageHeaderLength + 30] << 8) + bytes[kMessageHeaderLength + 31]);
    m_tlv.lengthField = (bytes[kMessageHeaderLength + 32] << 8) + bytes[kMessageHeaderLength + 33];

    for (std::vector<uint8_t*>::size_type i = 0; i < m_tlv.pathSequence.size(); ++i)
    {
        delete m_tlv.pathSequence[i];
    }
    for (int i = 0; i < m_tlv.lengthField; i+=8)
    {
        uint8_t* clockIdentity = new uint8_t[8];
        memcpy(clockIdentity, bytes + kMessageHeaderLength + 34 + 8 * i, 8);
        m_tlv.pathSequence.push_back(clockIdentity);
    }
}
