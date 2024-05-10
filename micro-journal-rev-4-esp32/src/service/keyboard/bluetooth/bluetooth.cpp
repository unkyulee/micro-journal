#include "usb.h"
#include "app/app.h"
#include "app/config/config.h"
#include "service/display/display.h"
#include "service/keyboard/ascii/ascii.h"

//
#include "screens/WordProcessor/WordProcessor.h"
#include "screens/ErrorScreen/ErrorScreen.h"
#include "screens/Menu/Menu.h"
#include "screens/WakeUp/WakeUp.h"


void bluetooth_setup();
void bluetooth_loop();
void bluetooth_key(char key);