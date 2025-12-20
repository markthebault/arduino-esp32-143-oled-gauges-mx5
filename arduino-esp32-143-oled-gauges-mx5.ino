#include "lcd_bsp.h"
#include "FT3168.h"
#include "mth_gauges.h"
#include "ESP32_NOW.h"
#include "WiFi.h"
#include <vector>

/* --- Telemetry Structure --- */
typedef struct __attribute__((packed)) {
  float oilTemp;
  float waterTemp;
  uint32_t engineRPM;
  float oilPressure;
  float brakePressure;
  int brakePercent;
  float throttlePos;
  float speed;
  float accelPos;
} TelemetryData;

// Global variable - Initialize with zeros
TelemetryData latestData = {0};
bool dataReceived = false; 

#define ESPNOW_WIFI_CHANNEL 6

/* --- ESP-NOW Peer Class --- */
class ESP_NOW_Peer_Class : public ESP_NOW_Peer {
public:
  ESP_NOW_Peer_Class(const uint8_t *mac_addr, uint8_t channel, wifi_interface_t iface, const uint8_t *lmk) 
    : ESP_NOW_Peer(mac_addr, channel, iface, lmk) {}

  bool add_peer() { return add(); }

  // This is the function that handles the data
  void onReceive(const uint8_t *data, size_t len, bool broadcast) {
    if (len == sizeof(TelemetryData)) {
      memcpy(&latestData, data, sizeof(TelemetryData));
      dataReceived = true; 
    }
  }
};

std::vector<ESP_NOW_Peer_Class *> masters;

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

void setup() {
  Serial.begin(115200);
  delay(2000); // Give serial time to start

  // 1. Initialize Display & LVGL
  Touch_Init();
  lcd_lvgl_Init();

  if (example_lvgl_lock(-1)) {   
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x000000), 0); 
    mth_gauge_oil_temp_init();
    example_lvgl_unlock();
  }

  // 2. Initialize Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.setChannel(ESPNOW_WIFI_CHANNEL);
  while (!WiFi.STA.started()) { delay(100); }

  // 3. Initialize ESP-NOW
  if (!ESP_NOW.begin()) {
    Serial.println("ESP-NOW Init Failed");
    ESP.restart();
  }

  ESP_NOW.onNewPeer(register_new_master, nullptr);
  Serial.println("Receiver System Ready");
}

void loop() {
  // Only update the gauge if we have received at least one packet
  if (dataReceived) {
    if (example_lvgl_lock(-1)) {
      // Cast the float to the expected int32_t for the gauge
      mth_gauge_set_temp((int32_t)latestData.oilTemp);
      example_lvgl_unlock();
    }
  }

  delay(30); 
}

/////////////// Test animation function when espnow is not used ///////////////
void set_oil_temp_animation() {
  const unsigned long period = 12000UL;
  unsigned long t = millis() % period;

  int32_t temp_val;

  if (t < 9000UL) {
    // Sweep Up (60 to 160)
    temp_val = 60 + (int32_t)((100UL * t) / 9000UL);
  } else {
    // Sweep Down (160 to 60)
    unsigned long t2 = t - 9000UL;
    temp_val = 160 - (int32_t)((100UL * t2) / 3000UL);
  }

  // Update oil temperature gauge
  mth_gauge_set_temp(temp_val);
}
/////////////////////////////////////////////////////