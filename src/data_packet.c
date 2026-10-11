#include "hardware_connect.h"

#pragma pack(push, 1)
typedef struct
{
    uint8_t node_id;
    float latitude;
    float longitude;
    uint8_t hour;

    float current_water_level;


    uint16_t hourly_rain_count;
    float hourly_water_level; 

    uint8_t warning_status;
} transmit_packet;
#pragma pack(pop)
