#include "esp_now_receiver.h"

// Global variable - Initialize with zeros
TelemetryData latestData = {0};
bool dataReceived = false;
std::vector<ESP_NOW_Peer_Class *> masters;

/* --- ESP-NOW Peer Class Implementation --- */
ESP_NOW_Peer_Class::ESP_NOW_Peer_Class(const uint8_t *mac_addr, uint8_t channel, wifi_interface_t iface, const uint8_t *lmk)
  : ESP_NOW_Peer(mac_addr, channel, iface, lmk) {}

bool ESP_NOW_Peer_Class::add_peer() {
  return add();
}

// This is the function that handles the data
void ESP_NOW_Peer_Class::onReceive(const uint8_t *data, size_t len, bool broadcast) {
  if (len == sizeof(TelemetryData)) {
    memcpy(&latestData, data, sizeof(TelemetryData));
    dataReceived = true;
  }
}

/* --- Callback for new masters --- */
void register_new_master(const esp_now_recv_info_t *info, const uint8_t *data, int len, void *arg) {
  if (memcmp(info->des_addr, ESP_NOW.BROADCAST_ADDR, 6) == 0) {
    ESP_NOW_Peer_Class *new_master = new ESP_NOW_Peer_Class(info->src_addr, ESPNOW_WIFI_CHANNEL, WIFI_IF_STA, nullptr);
    if (!new_master->add_peer()) {
      delete new_master;
      return;
    }
    masters.push_back(new_master);
    Serial.println("Master Registered");
  }
}

/* --- Initialize ESP-NOW Receiver --- */
void espnow_receiver_init() {
  // Initialize Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.setChannel(ESPNOW_WIFI_CHANNEL);
  while (!WiFi.STA.started()) { delay(100); }

  // Initialize ESP-NOW
  if (!ESP_NOW.begin()) {
    Serial.println("ESP-NOW Init Failed");
    ESP.restart();
  }

  ESP_NOW.onNewPeer(register_new_master, nullptr);
  Serial.println("Receiver System Ready");
}
