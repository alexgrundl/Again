#ifndef RECEIVEPACKAGE_H
#define RECEIVEPACKAGE_H

#include "types.h"

class ReceivePackage
{
  public:

    ReceivePackage(uint32_t buffersize);

    virtual ~ReceivePackage();

    bool IsValid();

    void SetValid();

    void SetRealSize(uint32_t size);

    void SetTimestamp(UScaledNs ts);

    virtual uint8_t const* GetBuffer();

    virtual uint32_t GetSize();

    virtual UScaledNs GetTimestamp();

  private:

    bool m_bValid;

    uint8_t* m_pBuffer;

    uint32_t m_Size;

    UScaledNs m_pTimestamp;
};

#endif // RECEIVEPACKAGE_H
