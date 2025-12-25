#ifndef ESP_NOW_RECEIVER_H
#define ESP_NOW_RECEIVER_H

#include "ESP32_NOW.h"
#include "WiFi.h"
#include "TelemetryData.h"
#include <vector>

#define ESPNOW_WIFI_CHANNEL 6

/* --- ESP-NOW Peer Class --- */
class ESP_NOW_Peer_Class : public ESP_NOW_Peer {
public:
  ESP_NOW_Peer_Class(const uint8_t *mac_addr, uint8_t channel, wifi_interface_t iface, const uint8_t *lmk);
  bool add_peer();
  void onReceive(const uint8_t *data, size_t len, bool broadcast);
};

// Global variables
extern TelemetryData latestData;
extern bool dataReceived;
extern std::vector<ESP_NOW_Peer_Class *> masters;

// Functions
void register_new_master(const esp_now_recv_info_t *info, const uint8_t *data, int len, void *arg);
void espnow_receiver_init();
void espnow_check_timeout();
TelemetryData espnow_get_data();

#endif // ESP_NOW_RECEIVER_H
