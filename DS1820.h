#ifndef DS1820_H
#define DS1820_H
//-----------------------------------------------------------------------------
#include "../avr-misc/avr-misc.h"
//-----------------------------------------------------------------------------
#define BAD_TEMP            0x8000
//-----------------------------------------------------------------------------
class AllSensorsDS1820
{
public:
    void measure();
//  void setResolution(BYTE resolution);
};
//-----------------------------------------------------------------------------
class BusDS1820
{
public:
    BYTE searchDevices();
    const QWORD& getFirstDeviceAddress();
    const QWORD& getNextDeviceAddress();
    AllSensorsDS1820 allSensors;
private:
    BYTE m_nextID = 0;
    BYTE m_devicesCount = 0;
    QWORD* m_addresses = NULL;
};
//-----------------------------------------------------------------------------
class SensorDS1820
{
public:
    SensorDS1820(QWORD& address);
    bool available();
    void measure();
    int  readTempC();
    int  readTempF();
    int  readTempK();
    int  readRawTemp();
    BYTE getModelCode();
    QWORD& getAddress();
//  void setResolution(BYTE resolution);
    void* operator new(size_t count);
    void operator delete(void* ptr);
private:
    union
    {
        QWORD m_address;
        BYTE m_modelCode;
    };
};
//-----------------------------------------------------------------------------
#endif
