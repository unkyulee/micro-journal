#include "HangulComposer.h"

//
// jamo classification and index tables
//
// Compatibility jamo consonants run 0x3131-0x314E, vowels 0x314F-0x3163.
// Precomposed syllable = 0xAC00 + (initial*21 + medial)*28 + final.
//

static bool isVowel(uint16_t jamo)
{
    return jamo >= 0x314F;
}

// initial (choseong) index of a compat consonant, -1 if it can't start
// a syllable (the compound consonants ㄳㄵ... never arrive as keystrokes)
static int8_t choseongIndex(uint16_t jamo)
{
    static const int8_t table[30] = {
        0, 1, -1, 2, -1, -1, 3, 4, 5, -1,      // ㄱㄲㄳㄴㄵㄶㄷㄸㄹㄺ
        -1, -1, -1, -1, -1, -1, 6, 7, 8, -1,   // ㄻㄼㄽㄾㄿㅀㅁㅂㅃㅄ
        9, 10, 11, 12, 13, 14, 15, 16, 17, 18, // ㅅㅆㅇㅈㅉㅊㅋㅌㅍㅎ
    };
    return table[jamo - 0x3131];
}

// final (jongseong) index of a compat consonant, -1 if it can't close
// a syllable (ㄸㅃㅉ)
static int8_t jongseongIndex(uint16_t jamo)
{
    static const int8_t table[30] = {
        1, 2, 3, 4, 5, 6, 7, -1, 8, 9,          // ㄱㄲㄳㄴㄵㄶㄷㄸㄹㄺ
        10, 11, 12, 13, 14, 15, 16, 17, -1, 18, // ㄻㄼㄽㄾㄿㅀㅁㅂㅃㅄ
        19, 20, 21, 22, -1, 23, 24, 25, 26, 27, // ㅅㅆㅇㅈㅉㅊㅋㅌㅍㅎ
    };
    return table[jamo - 0x3131];
}

// compound vowel of two keystrokes (ㅗ+ㅏ=ㅘ ...), 0 if they don't combine
static uint16_t combineVowels(uint16_t first, uint16_t second)
{
    if (first == 0x3157) // ㅗ
    {
        if (second == 0x314F)
            return 0x3158; // ㅘ
        if (second == 0x3150)
            return 0x3159; // ㅙ
        if (second == 0x3163)
            return 0x315A; // ㅚ
    }
    else if (first == 0x315C) // ㅜ
    {
        if (second == 0x3153)
            return 0x315D; // ㅝ
        if (second == 0x3154)
            return 0x315E; // ㅞ
        if (second == 0x3163)
            return 0x315F; // ㅟ
    }
    else if (first == 0x3161) // ㅡ
    {
        if (second == 0x3163)
            return 0x3162; // ㅢ
    }
    return 0;
}

// compound final of two keystrokes (ㄹ+ㄱ=ㄺ ...), 0 if they don't combine
static uint16_t combineFinals(uint16_t first, uint16_t second)
{
    if (first == 0x3131 && second == 0x3145)
        return 0x3133; // ㄱ+ㅅ=ㄳ
    if (first == 0x3134)
    {
        if (second == 0x3148)
            return 0x3135; // ㄴ+ㅈ=ㄵ
        if (second == 0x314E)
            return 0x3136; // ㄴ+ㅎ=ㄶ
    }
    if (first == 0x3139)
    {
        if (second == 0x3131)
            return 0x313A; // ㄹ+ㄱ=ㄺ
        if (second == 0x3141)
            return 0x313B; // ㄹ+ㅁ=ㄻ
        if (second == 0x3142)
            return 0x313C; // ㄹ+ㅂ=ㄼ
        if (second == 0x3145)
            return 0x313D; // ㄹ+ㅅ=ㄽ
        if (second == 0x314C)
            return 0x313E; // ㄹ+ㅌ=ㄾ
        if (second == 0x314D)
            return 0x313F; // ㄹ+ㅍ=ㄿ
        if (second == 0x314E)
            return 0x3140; // ㄹ+ㅎ=ㅀ
    }
    if (first == 0x3142 && second == 0x3145)
        return 0x3144; // ㅂ+ㅅ=ㅄ
    return 0;
}

// the character currently displayed for the composition state
uint32_t HangulComposer::build()
{
    if (L && V1)
    {
        uint16_t vowel = V2 ? combineVowels(V1, V2) : V1;
        uint16_t final_ = 0;
        if (T1)
            final_ = T2 ? combineFinals(T1, T2) : T1;

        int initial = choseongIndex(L);
        int medial = vowel - 0x314F;
        int final_index = final_ ? jongseongIndex(final_) : 0;
        return 0xAC00 + (initial * 21 + medial) * 28 + final_index;
    }

    if (L)
        return L; // lone consonant

    if (V1)
        return V2 ? combineVowels(V1, V2) : V1; // lone vowel

    return 0;
}

HangulComposer::Action HangulComposer::feed(uint32_t jamo)
{
    Action action = {0, 0};
    uint16_t j = (uint16_t)jamo;

    if (isVowel(j))
    {
        if (L && !V1)
        {
            // consonant + vowel: the syllable takes shape (ㄱ -> 가)
            V1 = j;
            action.replaceComposing = build();
        }
        else if (V1 && !T1 && !V2 && combineVowels(V1, j))
        {
            // compound vowel (고 -> 과)
            V2 = j;
            action.replaceComposing = build();
        }
        else if (L && V1 && T1)
        {
            // re-composition: the final consonant moves over to start the
            // next syllable (간 + ㅏ -> 가 + composing 나)
            uint16_t moved;
            if (T2)
            {
                moved = T2;
                T2 = 0; // compound final splits: first part stays (갃 -> 각)
            }
            else
            {
                moved = T1;
                T1 = 0; // simple final leaves entirely (간 -> 가)
            }
            action.replaceComposing = build();

            L = moved;
            V1 = j;
            V2 = T1 = T2 = 0;
            action.insert = build();
        }
        else if (!isComposing())
        {
            // lone vowel starts a composition (it may still grow into a
            // compound vowel)
            V1 = j;
            action.insert = build();
        }
        else
        {
            // nothing combines: current character is done, start over
            // with the lone vowel
            reset();
            V1 = j;
            action.insert = build();
        }
    }
    else // consonant
    {
        if (!isComposing())
        {
            L = j;
            action.insert = build();
        }
        else if (L && V1 && !T1 && jongseongIndex(j) >= 0)
        {
            // final consonant (가 -> 간)
            T1 = j;
            action.replaceComposing = build();
        }
        else if (L && V1 && T1 && !T2 && combineFinals(T1, j))
        {
            // compound final (갈 -> 갉)
            T2 = j;
            action.replaceComposing = build();
        }
        else
        {
            // nothing combines: current character is done, the consonant
            // starts the next syllable
            reset();
            L = j;
            action.insert = build();
        }
    }

    return action;
}

uint32_t HangulComposer::backspace()
{
    // remove the most recent jamo
    if (T2)
        T2 = 0;
    else if (T1)
        T1 = 0;
    else if (V2)
        V2 = 0;
    else if (V1)
        V1 = 0;
    else
        L = 0;

    return build();
}
