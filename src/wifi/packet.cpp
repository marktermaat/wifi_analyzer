#include "wifi_structs.h"
#include "data_structs.h"
#include "esp_wifi.h"

#include <map>
#include <algorithm>

std::string get_packet_type(WifiFrameControl *frame_control);
bool is_beacon_packet(WifiFrameControl *frame_control);
void handle_beacon_packet(WifiPacket *wifi_packet);
PacketInformation process_packet(WifiPacket *wifi_packet, uint16_t packet_size);
std::string get_mac(uint8_t mac[6]);

std::map<std::string, std::string> ssids;

void process_packet(void *buf, wifi_promiscuous_pkt_type_t type)
{
  const wifi_promiscuous_pkt_t *packet = (wifi_promiscuous_pkt_t *) buf; // Cast bytes to a generic struct with a header and payload
  const wifi_pkt_rx_ctrl_t header = (wifi_pkt_rx_ctrl_t) packet->rx_ctrl;

  WifiPacket *wifi_packet = (WifiPacket *) packet->payload;
  const WifiFrameHeader *frame_header = (WifiFrameHeader *) &wifi_packet->header;
  WifiFrameControl *frame_control = (WifiFrameControl *) &frame_header->control;

  if(is_beacon_packet(frame_control)) {
    handle_beacon_packet(wifi_packet);
  }

  const PacketInformation packet_info = process_packet(wifi_packet, header.sig_len);

  Serial.printf("| %s | %s | %s | %d | %s |\n", packet_info.ssid_name.c_str(), packet_info.device_mac.c_str(), packet_info.direction.c_str(), packet_info.total_bytes, packet_info.packet_type.c_str());
}

std::string get_packet_type(WifiFrameControl *frame_control)
{
  switch (frame_control->type)
  {
  case WIFI_PKT_MGMT:
    return "MGMT";
  case WIFI_PKT_CTRL:
    return "CTRL";
  case WIFI_PKT_DATA:
    return "DATA";
  case WIFI_PKT_MISC:
    return "MISC";
  }
}

bool is_beacon_packet(WifiFrameControl *frame_control) {
  return frame_control->type == WIFI_PKT_MGMT && frame_control->subtype == BEACON;
}

void handle_beacon_packet(WifiPacket *wifi_packet) {
  const WifiBeaconFrame *beacon_frame = (WifiBeaconFrame *)wifi_packet->payload;
  WifiFrameHeader *frame_header = (WifiFrameHeader *) &wifi_packet->header;

  const std::string ssid = std::string(beacon_frame->ssid, std::min(beacon_frame->tag_length, 32u));
  const std::string mac = get_mac(frame_header->sender_mac);

  if (ssids.find(mac) == ssids.end()) {
    ssids.insert(std::make_pair(mac, ssid));
  }
}

PacketInformation process_packet(WifiPacket *wifi_packet, uint16_t packet_size)
{
  WifiFrameHeader *frame_header = (WifiFrameHeader *)&wifi_packet->header;
  WifiFrameControl *frame_control = (WifiFrameControl *)&frame_header->control;

  PacketInformation packet_info = PacketInformation();
  packet_info.total_bytes = packet_size;
  packet_info.packet_type = get_packet_type(frame_control);

  const std::string sender = get_mac(frame_header->sender_mac);
  const std::string receiver = get_mac(frame_header->receiver_mac);

  if(ssids.find(sender) != ssids.end()) {
    // Sender is Access Point
    packet_info.device_mac = receiver;
    packet_info.direction = "DOWN";
    packet_info.ssid_name = ssids[sender];
  } else if (ssids.find(receiver) != ssids.end()) {
    // Receiver is Access Point
    packet_info.device_mac = sender;
    packet_info.direction = "UP";
    packet_info.ssid_name = ssids[receiver];
  } else {
    // Unknown ssid
    packet_info.device_mac = "UNKNOWN";
    packet_info.direction = "UNKNOWN";
    packet_info.ssid_name = "UNKNOWN";
  }

  return packet_info;
}

std::string get_mac(uint8_t mac[6])
{
  char address[] = "00:00:00:00:00:00\0";
  sprintf(address, "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  return std::string(address);
}