#include "receivepackage.h"

ReceivePackage::ReceivePackage(uint32_t buffersize)
{
    m_bValid = false;
    m_Size = buffersize;
    m_pBuffer = (uint8_t*)malloc(buffersize);
    m_pTimestamp.ns = 0;
    m_pTimestamp.ns_frac = 0;
}

ReceivePackage::~ReceivePackage()
{
    free(m_pBuffer);
}

bool ReceivePackage::IsValid()
{
    return m_bValid;
}

void ReceivePackage::SetValid()
{
    m_bValid = true;
}

void ReceivePackage::SetRealSize(uint32_t size)
{
    m_Size = size;
}

void ReceivePackage::SetTimestamp(UScaledNs ts)
{
    m_pTimestamp = ts;
}

uint8_t const* ReceivePackage::GetBuffer()
{
    return m_pBuffer;
}

uint32_t ReceivePackage::GetSize()
{
    return m_Size;
}

UScaledNs ReceivePackage::GetTimestamp()
{
    return m_pTimestamp;
}
