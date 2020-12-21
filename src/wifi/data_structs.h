#include <Arduino.h>

typedef struct
{
  uint16_t sequence_number;
  uint16_t total_bytes;
  std::string device_mac;
  std::string ssid_name;
  std::string packet_type;
  std::string direction; // UP/DOWN
} PacketInformation;