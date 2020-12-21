#include <Arduino.h>
#include <WiFi.h>
#include "esp_wifi.h"
#include "wifi/packet.h"

const char ROUTER[] = "4c:09:d4:33:9b:9c";

void mac2str(const uint8_t* mac, char* output) {
  sprintf(output, "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  if (strcmp(output, ROUTER) == 0) {
    strncpy(output, "ROUTER", 17);
  }
}

void packet_handler(void *buf, wifi_promiscuous_pkt_type_t type)
{
  process_packet(buf, type);
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

  setup_wifi_promiscuous_mode(4, &packet_handler);
}

    void loop()
{
  delay(2000);
}