#include <Arduino.h>

typedef struct
{
  unsigned interval : 16;
  unsigned capability : 16;
  unsigned tag_number : 8;
  unsigned tag_length : 8;
  char ssid[0];
  uint8_t rates[1];
} WifiBeaconFrame;

typedef struct
{
  unsigned protocol : 2;
  unsigned type : 2;
  unsigned subtype : 4;
  unsigned to_ds : 1;
  unsigned from_ds : 1;
  unsigned more_frag : 1;
  unsigned retry : 1;
  unsigned pwr_mgmt : 1;
  unsigned more_data : 1;
  unsigned wep : 1;
  unsigned strict : 1;
} WifiFrameControl;

typedef struct
{
  WifiFrameControl control;
  uint8_t receiver_mac[6];
  uint8_t sender_mac[6];
  uint8_t filtering_mac[6];
  unsigned sequence_ctrl : 16;
  uint8_t optional_addr[6];
} WifiFrameHeader;

typedef struct
{
  WifiFrameHeader header;
  uint8_t payload[2]; /* network data ended with 4 bytes csum (CRC32) */
} WifiPacket;

typedef enum
{
  ASSOCIATION_REQ,
  ASSOCIATION_RES,
  REASSOCIATION_REQ,
  REASSOCIATION_RES,
  PROBE_REQ,
  PROBE_RES,
  NU1, /* ......................*/
  NU2, /* 0110, 0111 not used */
  BEACON,
  ATIM,
  DISASSOCIATION,
  AUTHENTICATION,
  DEAUTHENTICATION,
  ACTION,
  ACTION_NACK,
} wifi_mgmt_subtypes_t;