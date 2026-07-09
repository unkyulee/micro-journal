#pragma once

#include <Arduino.h>

//
// Composes 두벌식 (dubeolsik) jamo key input into Hangul syllables.
//
// The keyboard locale sends Hangul compatibility jamo codepoints
// (0x3131-0x3163); this state machine assembles them into precomposed
// syllables (가..힣) one keystroke at a time. The Editor displays the
// syllable being composed as a normal character in the buffer and
// replaces it in place as it evolves: ㄱ -> 가 -> 간.
//
// Covered behavior:
//  - compound vowels (ㅗ+ㅏ=ㅘ ...) and compound finals (ㄹ+ㄱ=ㄺ ...)
//  - re-composition: a final consonant moves to the next syllable when a
//    vowel follows (간 + ㅏ -> 가 + composing 나)
//  - backspace removes one jamo, not the whole syllable (간 -> 가 -> ㄱ)
//
class HangulComposer
{
public:
    // What the Editor must do with the buffer after feeding a jamo:
    // first replace the current composing character (if replaceComposing
    // is nonzero), then insert a new composing character (if insert is
    // nonzero).
    struct Action
    {
        uint32_t replaceComposing;
        uint32_t insert;
    };

    static bool isJamo(uint32_t codepoint)
    {
        return codepoint >= 0x3131 && codepoint <= 0x3163;
    }

    // feed one jamo keystroke
    Action feed(uint32_t jamo);

    // backspace within a composition: removes the last jamo and returns
    // the new composing character, or 0 when the composing character is
    // gone entirely (the Editor then deletes it)
    uint32_t backspace();

    bool isComposing() { return L || V1; }

    // any event that ends the composition: cursor movement, paging,
    // file switch, non-jamo input ... the composed character simply
    // stays in the buffer as normal text
    void reset() { L = V1 = V2 = T1 = T2 = 0; }

private:
    // jamo of the current syllable, in keystroke order:
    // initial, vowel (up to 2 keys), final (up to 2 keys); 0 = unset
    uint16_t L = 0, V1 = 0, V2 = 0, T1 = 0, T2 = 0;

    uint32_t build();
};
