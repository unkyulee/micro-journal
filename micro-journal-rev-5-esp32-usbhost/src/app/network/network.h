#ifndef Network_h
#define Network_h

// #include "softAP/softAP.h"
// #include "wifi/wifi.h"

// by default ETHERNET is not enabled
// #define ENABLE_ETH
#ifdef ENABLE_ETH
#include "eth/eth.h"
#endif

// network control
void network_setup();
void network_loop();

//
void network_enable(bool enable);

#endif