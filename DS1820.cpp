//-----------------------------------------------------------------------------
#include <stdlib.h>
#include <string.h>
#include "DS1820.h"
#include "DS1820_misc.h"
#include "../avr-debug/debug.h"
#include "../avr-oneWire/oneWire.h"
//-----------------------------------------------------------------------------
BYTE BusDS1820::searchDevices()
{
    BYTE A, B;
    QWORD address;
    char dIndex;
    BYTEstack dPos, dBit;

    // clear known devices list
    if(m_addresses)
    {
        free(m_addresses);
        m_addresses = NULL;
    }
    m_devicesCount = 0;

    while(m_devicesCount == 0 || dPos.size() > 0)
    {
        if(one_wire_reset())
        {
            // no devices found (no response from any)
            break;
        }
        address = 0;
        dIndex = -1;
        one_wire_send_byte(CMD_ROM_SEARCH);
        for(BYTE i = 0; i < 64; i++)
        {
            A = one_wire_read_bit();
            B = one_wire_read_bit();
            //DHEX8(2, A, B);
            if(A == 1 && B == 1)
            {
                // a strange combination - we should not be here
                return m_devicesCount;
            }
            address >>= 1;
            if(A != B)
            {
                if(A)
                {
                    address |= ((QWORD) 1 << 63);
                    one_wire_write_one();
                }
                else
                {
                    one_wire_write_zero();
                }
            }
            else
            {
                // discrepancy found
                dIndex++;
                if(dPos.size() == 0 || i > dPos.top())
                {
                    // first time found discrepancy
                    dPos.push(i);
                    dBit.push(0);
                    one_wire_write_zero();
                }
                else
                {
                    if(i == dPos.top())
                    {
                        // change route direction
                        dBit.pop();
                        dBit.push(1);
                        address |= ((QWORD) 1 << 63);
                        one_wire_write_one();
                    }
                    if(i < dPos.top())
                    {
                        // tracking the previous route
                        address |= ((QWORD) dBit[dIndex] << 63);
                        one_wire_write_bit(dBit[dIndex]);
                    }
                }
            }
        }
        while(dBit.size() && dBit.top())
        {
            // get rid of completely traced routes
            dPos.pop();
            dBit.pop();
        }

        // save the result
        m_addresses = (QWORD*) realloc(m_addresses, sizeof(QWORD) * (m_devicesCount + 1));
        m_addresses[m_devicesCount++] = address;
    }
    return m_devicesCount;
}
//-----------------------------------------------------------------------------
const QWORD& BusDS1820::getFirstDeviceAddress()
{
    if(m_devicesCount)
    {
        m_nextID = 1;
        return m_addresses[0];
    }
    return NULL;
}
//-----------------------------------------------------------------------------
const QWORD& BusDS1820::getNextDeviceAddress()
{
    if(m_nextID < m_devicesCount)
    {
        return m_addresses[m_nextID++];
    }
    return NULL;
}
//-----------------------------------------------------------------------------
void* SensorDS1820::operator new(size_t count)
{
    void* ptr = malloc(sizeof(SensorDS1820) * count);
    return ptr;
}
//-----------------------------------------------------------------------------
void SensorDS1820::operator delete(void* ptr)
{
    if(ptr)
    {
        free(ptr);
    }
}
//-----------------------------------------------------------------------------
SensorDS1820::SensorDS1820(QWORD& address)
{
    m_address = address;
}
//-----------------------------------------------------------------------------
QWORD& SensorDS1820::getAddress()
{
    return m_address;
}
//-----------------------------------------------------------------------------
BYTE SensorDS1820::getModelCode()
{
    return m_modelCode;
}
//-----------------------------------------------------------------------------
__inline void send_address(QWORD* address)
{
    if(address)
    {
        BYTE* tmp = (BYTE*) address;
        one_wire_send_byte(CMD_ROM_MATCH);
        for(BYTE i = 0; i < 8; i++)
        {
            one_wire_send_byte(*tmp++);
        }
    }
    else
    {
        one_wire_send_byte(CMD_ROM_SKIP);
    }
}
//-----------------------------------------------------------------------------
__inline void start_conversion(QWORD* address = NULL)
{
    if(one_wire_reset())
    {
        return;
    }
    send_address(address);
    one_wire_send_byte(CMD_START_CONVERTION);
}
//-----------------------------------------------------------------------------
bool SensorDS1820::available()
{
    if(one_wire_reset())
    {
        return false;
    }
    send_address(&m_address);
    one_wire_send_byte(CMD_RAM_READ);
    if(one_wire_read_byte() != 0xFF)
    {
        return true;
    }
    if(one_wire_read_byte() != 0xFF)
    {
        return true;
    }
    return false;
}
//-----------------------------------------------------------------------------
void AllSensorsDS1820::measure()
{
    start_conversion();
}
//-----------------------------------------------------------------------------
void SensorDS1820::measure()
{
    start_conversion(&m_address);
}
//-----------------------------------------------------------------------------
//__inline void set_resolution(__unused BYTE resolution, QWORD* address = NULL)
//{
//    if(one_wire_reset())
//    {
//        return;
//    }
//    send_address(address);
//    // TODO: set resolution command and data
//}
//-----------------------------------------------------------------------------
//void AllSensorsDS1820::setResolution(BYTE resolution)
//{
//    set_resolution(resolution);
//}
//-----------------------------------------------------------------------------
//void SensorDS1820::setResolution(BYTE resolution)
//{
//    set_resolution(resolution, &m_address);
//}
//-----------------------------------------------------------------------------
int SensorDS1820::readRawTemp()
{
    if(one_wire_reset())
    {
        return BAD_TEMP;
    }
    send_address(&m_address);
    one_wire_send_byte(CMD_RAM_READ);
    WORD result = one_wire_read_word();
    if(result == 0xFFFF)
    {
        return BAD_TEMP;
    }
    return (int) result;
}
//-----------------------------------------------------------------------------
int SensorDS1820::readTempC()
{
    int result = readRawTemp();
    if(result == (int) BAD_TEMP)
    {
        return result;
    }
    return ((result >> 3) + 1) / 2;  // round to nearest integer;
}
//-----------------------------------------------------------------------------
int SensorDS1820::readTempF()
{
    int result = readRawTemp();
    if(result == (int) BAD_TEMP)
    {
        return result;
    }
    return ((result * 18 / 10 >> 3) + 1) / 2 + 32;
}
//-----------------------------------------------------------------------------
int SensorDS1820::readTempK()
{
    return readTempC() + 273;
}
//-----------------------------------------------------------------------------
