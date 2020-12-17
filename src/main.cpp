#include <Arduino.h>
#include <WiFi.h>
#include "esp_wifi.h"
#include "wifi_structs.h"

const char ROUTER[] = "4c:09:d4:33:9b:9c";

void mac2str(const uint8_t* mac, char* output) {
  sprintf(output, "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  if (strcmp(output, ROUTER) == 0) {
    strncpy(output, "ROUTER", 17);
  }
}

void packet_handler(void *buf, wifi_promiscuous_pkt_type_t type)
{
  const wifi_promiscuous_pkt_t *packet = (wifi_promiscuous_pkt_t *) buf; // Cast bytes to a generic struct with a header and payload
  const wifi_pkt_rx_ctrl_t header = (wifi_pkt_rx_ctrl_t) packet->rx_ctrl;

  const WifiPacket *wifi_packet = (WifiPacket *) packet->payload;
  const WifiFrameHeader *frame_header = (WifiFrameHeader *) &wifi_packet->header;
  const WifiFrameControl *frame_control = (WifiFrameControl *) &frame_header->control;

  uint channel = header.channel;
  uint packet_length = header.sig_len;
  uint frame_type = frame_control->type;

  std::string packet_type = "----";
  if (frame_type == WIFI_PKT_MGMT) packet_type = "MGMT";
  if (frame_type == WIFI_PKT_CTRL) packet_type = "CTRL";
  if (frame_type == WIFI_PKT_DATA) packet_type = "DATA";
  if (frame_type == WIFI_PKT_MISC) packet_type = "MISC";

  if (frame_type == WIFI_PKT_MGMT && frame_control->subtype == BEACON) {
    const char *data = (char *) buf;
    Serial.print("Full message ");
    for (int i = 0; i < header.sig_len; i++)
    {
      Serial.print(data[i]);
    }
    Serial.println();

    const uint8_t *payload = wifi_packet->payload;

    Serial.print("Payload ");
    for (int i = 0; i < 100; i++)
    {
      Serial.printf("%d ", data[i]);
    }
    Serial.println();

    const WifiBeaconFrame *beacon_frame = (WifiBeaconFrame *) wifi_packet->payload;
    Serial.printf("Interval: %u, Capability: %u, tag: %d, tag_length: %d\n", beacon_frame->interval, beacon_frame->capability, beacon_frame->tag_number, beacon_frame->tag_length);

    char ssid[32] = {0};

    if (beacon_frame->tag_length >= 32)
    {
      strncpy(ssid, beacon_frame->ssid, 31);
    }
    else
    {
      strncpy(ssid, beacon_frame->ssid, beacon_frame->tag_length);
    }

    Serial.printf("%s\n", ssid);

  }

  char sender[] = "00:00:00:00:00:00\0";
  char receiver[] = "00:00:00:00:00:00\0";
  char filter[] = "00:00:00:00:00:00\0";

  mac2str(frame_header->sender_mac, sender);
  mac2str(frame_header->receiver_mac, receiver);
  mac2str(frame_header->filtering_mac, filter);

  Serial.printf("| %s | %s | %s | %d | %d | %d\n", packet_type.c_str(), sender, receiver, channel, packet_length, sizeof(wifi_packet->payload));
}

void setup_wifi_promiscuous_mode(int channel, wifi_promiscuous_cb_t handler)
{
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  esp_wifi_init(&cfg);
  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  esp_wifi_set_mode(WIFI_MODE_NULL);
  esp_wifi_start();
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_rx_cb(handler);
  esp_wifi_set_channel(4, WIFI_SECOND_CHAN_NONE);
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Setup");

  setup_wifi_promiscuous_mode(4, &packet_handler);
}

    void loop()
{
  Serial.println("Testing");
  delay(2000);
}