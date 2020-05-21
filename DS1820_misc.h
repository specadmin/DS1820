#ifndef DS1820_MISC_H
#define DS1820_MISC_H
//-----------------------------------------------------------------------------
enum ROMCommand
{
    CMD_ROM_READ            = 0x33,
    CMD_ROM_MATCH           = 0x55,
    CMD_ROM_SKIP            = 0xCC,
    CMD_ROM_SEARCH          = 0xF0,
    CMD_ALARM_SEARCH        = 0xEC
};
//-----------------------------------------------------------------------------
enum FunctionCommand
{
    CMD_RAM_READ            = 0xBE,
    CMD_RAM_WRITE           = 0x4E,
    CMD_RAM_UPLOAD          = 0x48,
    CMD_START_CONVERTION    = 0x44,
    CMD_RECALL              = 0xB8,
    CMD_READ_POWER_SOURCE   = 0xB4
};
//-----------------------------------------------------------------------------
enum
{
    MODEL_DS1820            = 0x10,
    MODEL_DS18S20           = 0x10,
    MODEL_DS18B20           = 0x28,
    MODEL_DS1822            = 0x22
};
//-----------------------------------------------------------------------------
#endif
