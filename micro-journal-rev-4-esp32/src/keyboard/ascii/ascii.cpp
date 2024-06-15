#include "ascii.h"

uint8_t ascii_international(uint8_t precursor, uint8_t ascii)
{
    // ~  a n o
    uint8_t found = 0;
    if (precursor == '~' && ascii == 'a')
    {
        found = 227; // ã
    }
    else if (precursor == '~' && ascii == 'A')
    {
        found = 195; // Ã
    }
    else if (precursor == '~' && ascii == 'n')
    {
        found = 241; // ñ
    }
    else if (precursor == '~' && ascii == 'N')
    {
        found = 209; // Ñ
    }
    else if (precursor == '~' && ascii == 'o')
    {
        found = 245; // õ
    }
    else if (precursor == '~' && ascii == 'O')
    {
        found = 213; // Õ
    }

    // ` a e i o u
    else if (precursor == '`' && ascii == 'a')
    {
        found = 224; // à
    }
    else if (precursor == '`' && ascii == 'A')
    {
        found = 192; // À
    }
    else if (precursor == '`' && ascii == 'e')
    {
        found = 232; // è
    }
    else if (precursor == '`' && ascii == 'E')
    {
        found = 200; // È
    }
    else if (precursor == '`' && ascii == 'i')
    {
        found = 236; // ì
    }
    else if (precursor == '`' && ascii == 'I')
    {
        found = 204; // Ì
    }
    else if (precursor == '`' && ascii == 'o')
    {
        found = 242; // ò
    }
    else if (precursor == '`' && ascii == 'O')
    {
        found = 210; // Ò
    }
    else if (precursor == '`' && ascii == 'u')
    {
        found = 249; // ù
    }
    else if (precursor == '`' && ascii == 'U')
    {
        found = 217; // Ù
    }

    // ' a e i o u
    else if (precursor == '\'' && ascii == 'a')
    {
        found = 225; // á
    }
    else if (precursor == '\'' && ascii == 'A')
    {
        found = 193; // Á
    }
    else if (precursor == '\'' && ascii == 'e')
    {
        found = 233; // é
    }
    else if (precursor == '\'' && ascii == 'E')
    {
        found = 201; // É
    }
    else if (precursor == '\'' && ascii == 'i')
    {
        found = 237; // í
    }
    else if (precursor == '\'' && ascii == 'I')
    {
        found = 205; // Í
    }
    else if (precursor == '\'' && ascii == 'o')
    {
        found = 243; // ó
    }
    else if (precursor == '\'' && ascii == 'O')
    {
        found = 211; // Ó
    }
    else if (precursor == '\'' && ascii == 'u')
    {
        found = 250; // ú
    }
    else if (precursor == '\'' && ascii == 'U')
    {
        found = 218; // Ú
    }

    // " a e i o u y
    else if (precursor == '\"' && ascii == 'a')
    {
        found = 228; // ä
    }
    else if (precursor == '\"' && ascii == 'A')
    {
        found = 196; // Ä
    }
    else if (precursor == '\"' && ascii == 'e')
    {
        found = 235; // ë
    }
    else if (precursor == '\"' && ascii == 'E')
    {
        found = 203; // Ë
    }
    else if (precursor == '\"' && ascii == 'i')
    {
        found = 239; // ï
    }
    else if (precursor == '\"' && ascii == 'I')
    {
        found = 207; // Ï
    }
    else if (precursor == '\"' && ascii == 'o')
    {
        found = 246; // ö
    }
    else if (precursor == '\"' && ascii == 'O')
    {
        found = 214; // Ö
    }
    else if (precursor == '\"' && ascii == 'u')
    {
        found = 252; // ü
    }
    else if (precursor == '\"' && ascii == 'U')
    {
        found = 220; // Ü
    }
    else if (precursor == '\"' && ascii == 'y')
    {
        found = 255; // ÿ
    }

    // ^ a e i o u
    else if (precursor == '^' && ascii == 'a')
    {
        found = 226; // â
    }
    else if (precursor == '^' && ascii == 'A')
    {
        found = 194; // Â
    }
    else if (precursor == '^' && ascii == 'e')
    {
        found = 234; // ê
    }
    else if (precursor == '^' && ascii == 'E')
    {
        found = 202; // Ê
    }
    else if (precursor == '^' && ascii == 'i')
    {
        found = 238; // î
    }
    else if (precursor == '^' && ascii == 'I')
    {
        found = 206; // Î
    }
    else if (precursor == '^' && ascii == 'o')
    {
        found = 244; // ô
    }
    else if (precursor == '^' && ascii == 'O')
    {
        found = 212; // Ô
    }
    else if (precursor == '^' && ascii == 'u')
    {
        found = 251; // û
    }
    else if (precursor == '^' && ascii == 'U')
    {
        found = 219; // Û
    }

    return found;
}

// https://www.ascii-code.com/
String ascii_convert_unicode(u_int8_t ascii)
{
    switch (ascii)
    {
    case 128:
        return "€";
    case 140:
        return "Œ";
    case 145:
        return "‘";
    case 146:
        return "’";
    case 147:
        return "“";
    case 148:
        return "”";
    case 162:
        return "¢";
    case 163:
        return "£";
    case 167:
        return "§";
    case 168:
        return "¨";
    case 169:
        return "©";
    case 170:
        return "ª";
    case 176:
        return "°";
    case 181:
        return "µ";
    case 191:
        return "¿";
    case 192:
        return "À";
    case 193:
        return "Á";
    case 194:
        return "Â";
    case 195:
        return "Ã";
    case 196:
        return "Ä";
    case 197:
        return "Å";
    case 198:
        return "Æ";
    case 199:
        return "Ç";
    case 200:
        return "È";
    case 201:
        return "É";
    case 202:
        return "Ê";
    case 203:
        return "Ë";
    case 204:
        return "Ì";
    case 205:
        return "Í";
    case 206:
        return "Î";
    case 207:
        return "Ï";
    case 208:
        return "Ð";
    case 209:
        return "Ñ";
    case 210:
        return "Ò";
    case 211:
        return "Ó";
    case 212:
        return "Ô";
    case 213:
        return "Õ";
    case 214:
        return "Ö";
    case 217:
        return "Ù";
    case 218:
        return "Ú";
    case 219:
        return "Û";
    case 220:
        return "Ü";
    case 223:
        return "ß";
    case 224:
        return "à";
    case 225:
        return "á";
    case 227:
        return "ã";
    case 226:
        return "â";
    case 228:
        return "ä";
    case 231:
        return "ç";
    case 232:
        return "è";
    case 233:
        return "é";
    case 234:
        return "ê";
    case 235:
        return "ë";
    case 236:
        return "ì";
    case 237:
        return "í";
    case 238:
        return "î";
    case 239:
        return "ï";
    case 241:
        return "ñ";
    case 242:
        return "ò";
    case 243:
        return "ó";
    case 244:
        return "ô";
    case 245:
        return "õ";
    case 246:
        return "ö";
    case 249:
        return "ù";
    case 252:
        return "ü";
    case 250:
        return "ú";
    case 251:
        return "û";
    case 255:
        return "ÿ";
    }

    return "";
}

// Map for Windows-1252 extended ASCII characters
uint8_t windows1252_map[128] = {
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
    0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
    0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
    0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
    0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7,
    0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
    0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7,
    0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
    0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7,
    0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
    0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7,
    0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
    0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7,
    0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
    0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7,
    0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF};

uint8_t unicode_convert_ascii(String key)
{
    if (key.length() != 1)
    {
        // If the input is not a single character, return 0 as an error value
        if(key == "ESC") {
            return 27;
        } else if(key == "BACKSPACE") {
            return 8;
        } else if(key == "SHIFT") {
            return 14;
        } else if(key == "ALT") {
            return 17;
        }
        return 0;
    }

    uint8_t ascii_value = key[0];
    if (ascii_value > 127)
    {
        // Extended ASCII range (128-255)
        ascii_value = windows1252_map[ascii_value - 128];
    }

    return ascii_value;
}