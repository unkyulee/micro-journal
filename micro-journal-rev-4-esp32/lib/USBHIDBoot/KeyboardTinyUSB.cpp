#include "KeyboardTinyUSB.h"

// Standard US keyboard layout. Ported verbatim from arduino-pico's
// HID_Keyboard/KeyboardLayout_en_US.cpp (Arduino LGPL) -- see the note in
// KeyboardTinyUSB.h for why it's duplicated here instead of included.
const uint8_t KeyboardLayout_en_US[128] =
{
    0x00,          // NUL
    0x00,          // SOH
    0x00,          // STX
    0x00,          // ETX
    0x00,          // EOT
    0x00,          // ENQ
    0x00,          // ACK
    0x00,          // BEL
    0x2a,          // BS  Backspace
    0x2b,          // TAB Tab
    0x28,          // LF  Enter
    0x00,          // VT
    0x00,          // FF
    0x00,          // CR
    0x00,          // SO
    0x00,          // SI
    0x00,          // DEL
    0x00,          // DC1
    0x00,          // DC2
    0x00,          // DC3
    0x00,          // DC4
    0x00,          // NAK
    0x00,          // SYN
    0x00,          // ETB
    0x00,          // CAN
    0x00,          // EM
    0x00,          // SUB
    0x00,          // ESC
    0x00,          // FS
    0x00,          // GS
    0x00,          // RS
    0x00,          // US

    0x2c,          // ' '
    0x1e|TINYUSB_KBD_SHIFT,    // !
    0x34|TINYUSB_KBD_SHIFT,    // "
    0x20|TINYUSB_KBD_SHIFT,    // #
    0x21|TINYUSB_KBD_SHIFT,    // $
    0x22|TINYUSB_KBD_SHIFT,    // %
    0x24|TINYUSB_KBD_SHIFT,    // &
    0x34,          // '
    0x26|TINYUSB_KBD_SHIFT,    // (
    0x27|TINYUSB_KBD_SHIFT,    // )
    0x25|TINYUSB_KBD_SHIFT,    // *
    0x2e|TINYUSB_KBD_SHIFT,    // +
    0x36,          // ,
    0x2d,          // -
    0x37,          // .
    0x38,          // /
    0x27,          // 0
    0x1e,          // 1
    0x1f,          // 2
    0x20,          // 3
    0x21,          // 4
    0x22,          // 5
    0x23,          // 6
    0x24,          // 7
    0x25,          // 8
    0x26,          // 9
    0x33|TINYUSB_KBD_SHIFT,    // :
    0x33,          // ;
    0x36|TINYUSB_KBD_SHIFT,    // <
    0x2e,          // =
    0x37|TINYUSB_KBD_SHIFT,    // >
    0x38|TINYUSB_KBD_SHIFT,    // ?
    0x1f|TINYUSB_KBD_SHIFT,    // @
    0x04|TINYUSB_KBD_SHIFT,    // A
    0x05|TINYUSB_KBD_SHIFT,    // B
    0x06|TINYUSB_KBD_SHIFT,    // C
    0x07|TINYUSB_KBD_SHIFT,    // D
    0x08|TINYUSB_KBD_SHIFT,    // E
    0x09|TINYUSB_KBD_SHIFT,    // F
    0x0a|TINYUSB_KBD_SHIFT,    // G
    0x0b|TINYUSB_KBD_SHIFT,    // H
    0x0c|TINYUSB_KBD_SHIFT,    // I
    0x0d|TINYUSB_KBD_SHIFT,    // J
    0x0e|TINYUSB_KBD_SHIFT,    // K
    0x0f|TINYUSB_KBD_SHIFT,    // L
    0x10|TINYUSB_KBD_SHIFT,    // M
    0x11|TINYUSB_KBD_SHIFT,    // N
    0x12|TINYUSB_KBD_SHIFT,    // O
    0x13|TINYUSB_KBD_SHIFT,    // P
    0x14|TINYUSB_KBD_SHIFT,    // Q
    0x15|TINYUSB_KBD_SHIFT,    // R
    0x16|TINYUSB_KBD_SHIFT,    // S
    0x17|TINYUSB_KBD_SHIFT,    // T
    0x18|TINYUSB_KBD_SHIFT,    // U
    0x19|TINYUSB_KBD_SHIFT,    // V
    0x1a|TINYUSB_KBD_SHIFT,    // W
    0x1b|TINYUSB_KBD_SHIFT,    // X
    0x1c|TINYUSB_KBD_SHIFT,    // Y
    0x1d|TINYUSB_KBD_SHIFT,    // Z
    0x2f,          // [
    0x31,          // bslash
    0x30,          // ]
    0x23|TINYUSB_KBD_SHIFT,    // ^
    0x2d|TINYUSB_KBD_SHIFT,    // _
    0x35,          // `
    0x04,          // a
    0x05,          // b
    0x06,          // c
    0x07,          // d
    0x08,          // e
    0x09,          // f
    0x0a,          // g
    0x0b,          // h
    0x0c,          // i
    0x0d,          // j
    0x0e,          // k
    0x0f,          // l
    0x10,          // m
    0x11,          // n
    0x12,          // o
    0x13,          // p
    0x14,          // q
    0x15,          // r
    0x16,          // s
    0x17,          // t
    0x18,          // u
    0x19,          // v
    0x1a,          // w
    0x1b,          // x
    0x1c,          // y
    0x1d,          // z
    0x2f|TINYUSB_KBD_SHIFT,    // {
    0x31|TINYUSB_KBD_SHIFT,    // |
    0x30|TINYUSB_KBD_SHIFT,    // }
    0x35|TINYUSB_KBD_SHIFT,    // ~
    0x00           // DEL
};

namespace
{
    // Boot-protocol compatible keyboard report descriptor: no Report ID,
    // matches the plain 8-byte boot report (modifier, reserved, keys[6])
    // required by BIOS/UEFI "Legacy USB Keyboard Support".
    uint8_t const _desc_hid_report[] = {
        TUD_HID_REPORT_DESC_KEYBOARD()};
}

KeyboardTinyUSB_::KeyboardTinyUSB_(void)
    : _hid(_desc_hid_report, sizeof(_desc_hid_report), HID_ITF_PROTOCOL_KEYBOARD, 4, false)
{
    memset(&_keyReport, 0, sizeof(_keyReport));
}

void KeyboardTinyUSB_::begin(void)
{
    _hid.setBootProtocol(HID_ITF_PROTOCOL_KEYBOARD);
    _hid.begin();
}

void KeyboardTinyUSB_::end(void)
{
}

void KeyboardTinyUSB_::sendReport(KeyReport *keys)
{
    // Wait (bounded) for the previous report to finish transmitting
    // instead of silently dropping this one when the endpoint is
    // momentarily still busy -- a single non-blocking ready() check was
    // losing keystrokes typed faster than the HID poll interval (e.g.
    // holding Shift then quickly tapping a letter fires two press() calls,
    // and hence two sendReport() calls, close enough together to land in
    // the same still-busy window). Mirrors the timeout/pump pattern the
    // native arduino-pico Keyboard library uses in __USBHIDReady().
    uint32_t start = millis();
    const uint32_t timeout = 500;
    while (((millis() - start) < timeout) && !_hid.ready())
    {
        tud_task();
    }

    if (_hid.ready())
    {
        // Report ID 0: no leading ID byte, keeps the wire format identical
        // to the USB HID boot keyboard report.
        _hid.keyboardReport(0, keys->modifiers, keys->keys);
    }
}

// press() adds the specified key (printing, non-printing, or modifier)
// to the persistent key report and sends the report. Because of the way
// USB HID works, the host acts like the key remains pressed until we
// call release(), releaseAll(), or otherwise clear the report and resend.
// Ported from arduino-pico's HID_Keyboard::press().
size_t KeyboardTinyUSB_::press(uint8_t k)
{
    uint8_t i;
    if (k >= 136) // it's a non-printing key (not a modifier)
    {
        k = k - 136;
    }
    else if (k >= 128) // it's a modifier key
    {
        _keyReport.modifiers |= (1 << (k - 128));
        k = 0;
    }
    else // it's a printing key
    {
        k = KeyboardLayout_en_US[k];
        if (!k)
        {
            setWriteError();
            return 0;
        }
        if ((k & TINYUSB_KBD_ALT_GR) == TINYUSB_KBD_ALT_GR)
        {
            _keyReport.modifiers |= 0x40; // AltGr = right Alt
            k &= 0x3F;
        }
        else if ((k & TINYUSB_KBD_SHIFT) == TINYUSB_KBD_SHIFT)
        {
            _keyReport.modifiers |= 0x02; // the left shift modifier
            k &= 0x7F;
        }
        if (k == TINYUSB_KBD_ISO_KEY)
        {
            k = TINYUSB_KBD_ISO_REPLACEMENT;
        }
    }

    // Add k to the key report only if it's not already present
    // and if there is an empty slot.
    if (_keyReport.keys[0] != k && _keyReport.keys[1] != k &&
        _keyReport.keys[2] != k && _keyReport.keys[3] != k &&
        _keyReport.keys[4] != k && _keyReport.keys[5] != k)
    {
        for (i = 0; i < 6; i++)
        {
            if (_keyReport.keys[i] == 0x00)
            {
                _keyReport.keys[i] = k;
                break;
            }
        }
        if (i == 6)
        {
            setWriteError();
            return 0;
        }
    }
    sendReport(&_keyReport);
    return 1;
}

// release() takes the specified key out of the persistent key report and
// sends the report. Ported from arduino-pico's HID_Keyboard::release().
size_t KeyboardTinyUSB_::release(uint8_t k)
{
    uint8_t i;
    if (k >= 136) // it's a non-printing key (not a modifier)
    {
        k = k - 136;
    }
    else if (k >= 128) // it's a modifier key
    {
        _keyReport.modifiers &= ~(1 << (k - 128));
        k = 0;
    }
    else // it's a printing key
    {
        k = KeyboardLayout_en_US[k];
        if (!k)
        {
            return 0;
        }
        if ((k & TINYUSB_KBD_ALT_GR) == TINYUSB_KBD_ALT_GR)
        {
            _keyReport.modifiers &= ~(0x40); // AltGr = right Alt
            k &= 0x3F;
        }
        else if ((k & TINYUSB_KBD_SHIFT) == TINYUSB_KBD_SHIFT)
        {
            _keyReport.modifiers &= ~(0x02); // the left shift modifier
            k &= 0x7F;
        }
        if (k == TINYUSB_KBD_ISO_KEY)
        {
            k = TINYUSB_KBD_ISO_REPLACEMENT;
        }
    }

    // Test the key report to see if k is present. Clear it if it exists.
    // Check all positions in case the key is present more than once.
    for (i = 0; i < 6; i++)
    {
        if (0 != k && _keyReport.keys[i] == k)
        {
            _keyReport.keys[i] = 0x00;
        }
    }

    sendReport(&_keyReport);
    return 1;
}

void KeyboardTinyUSB_::releaseAll(void)
{
    _keyReport.keys[0] = 0;
    _keyReport.keys[1] = 0;
    _keyReport.keys[2] = 0;
    _keyReport.keys[3] = 0;
    _keyReport.keys[4] = 0;
    _keyReport.keys[5] = 0;
    _keyReport.modifiers = 0;
    sendReport(&_keyReport);
}

size_t KeyboardTinyUSB_::write(uint8_t c)
{
    uint8_t p = press(c); // Keydown
    delay(10);
    release(c); // Keyup
    delay(10);
    return p; // just return the result of press() since release() almost always returns 1
}

size_t KeyboardTinyUSB_::write(const uint8_t *buffer, size_t size)
{
    size_t n = 0;
    while (size--)
    {
        if (*buffer != '\r')
        {
            if (write(*buffer))
            {
                n++;
            }
            else
            {
                break;
            }
        }
        buffer++;
    }
    return n;
}

KeyboardTinyUSB_ Keyboard;
