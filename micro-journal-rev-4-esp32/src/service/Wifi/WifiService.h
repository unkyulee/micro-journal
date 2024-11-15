#ifndef WifiService_h
#define WifiService_h

// 
void wifi_service_loop();

// service library to setup wifi and connect/disconnect to wifi
void wifi_config_load();
void wifi_config_save();

// 0 - starting
// 1 - waiting
// 2 - sending
#define SYNC_START 0
#define SYNC_ERROR -1
#define SYNC_SEND 1
#define SYNC_COMPLETED 2

void wifi_sync_init();

// SYNC PROCESS
void sync_start_request();
void sync_start();
void sync_stop_request();
void sync_stop();

//
void _sync_start(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f);
bool _connect_wifi(JsonDocument &app, const char *ssid, const char *password);
void _sync_send(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f);
void _network_stop();

#endif