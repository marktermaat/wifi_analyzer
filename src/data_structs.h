#include <Arduino.h>

typedef struct
{
  uint16_t total_bytes;
  char device_mac[];
  char ssid_name[];
  char packet_type[4];
  char direction[4]; // UP/DOWN
} PacketInformation;