
// DisplayConfig.h — panel selector and default pins
// You can override any of these with -D compiler flags or before including this header.

#ifndef PANEL_2P9
  // Select exactly one panel; default to 2.9"
  #define PANEL_2P9
  // #define PANEL_4P2
#endif

// Legacy width/height macros (still used by some sketches).
// Driver classes set their own width/height internally, so these are mostly informational.
#if defined(PANEL_2P9)
  #ifndef DISPLAY_WIDTH
    #define DISPLAY_WIDTH   168
  #endif
  #ifndef DISPLAY_HEIGHT
    #define DISPLAY_HEIGHT  384
  #endif
#elif defined(PANEL_4P2)
  #ifndef DISPLAY_WIDTH
    #define DISPLAY_WIDTH   300
  #endif
  #ifndef DISPLAY_HEIGHT
    #define DISPLAY_HEIGHT  400
  #endif
#endif

// ===== Default pin mapping (can be overridden) =====
#ifndef PIN_DC
  #define PIN_DC    4
#endif
#ifndef PIN_CS
  #define PIN_CS    3
#endif
#ifndef PIN_SCLK
  #define PIN_SCLK  2
#endif
#ifndef PIN_SDIN
  #define PIN_SDIN  1
#endif
#ifndef PIN_RST
  #define PIN_RST   0
#endif
