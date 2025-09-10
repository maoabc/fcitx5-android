
#include <cstddef>
#include <string_view>
#include <jni.h>


enum {
    SURROGATE_LEAD_FIRST = 0xD800
};
enum {
    SURROGATE_TRAIL_FIRST = 0xDC00
};

const unsigned char UTF8BytesOfLead[256] = {
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 00 - 0F
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 10 - 1F
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 20 - 2F
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 30 - 3F
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 40 - 4F
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 50 - 5F
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 60 - 6F
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 70 - 7F
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 80 - 8F
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 90 - 9F
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // A0 - AF
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // B0 - BF
        1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, // C0 - CF
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, // D0 - DF
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, // E0 - EF
        4, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // F0 - FF
};

inline constexpr unsigned char TrailByteValue0(char c) {
    // The top 2 bits are 0b10 to indicate a trail byte.
    // The lower 6 bits contain the value.
    return static_cast<unsigned char>(c & 0b0011'1111u);
}


size_t utf16_from_utf8(std::string_view sv, jchar *tbuf, size_t tlen) {
    size_t ui = 0;
    for (size_t i = 0; i < sv.length();) {
        unsigned char ch = (unsigned char) sv[i];
        const unsigned int byteCount = UTF8BytesOfLead[ch];
        unsigned int value;

        if (i + byteCount > sv.length()) {
            // Trying to read past end but still have space to write
            if (ui < tlen) {
                tbuf[ui] = ch;
                ui++;
            }
            break;
        }

        i++;
        switch (byteCount) {
            case 1:
                tbuf[ui] = ch;
                break;
            case 2:
                tbuf[ui] = static_cast<jchar>(
                        ((ch & 0x1Fu) << 6u) |
                        TrailByteValue0(sv[i++])
                );
                break;
            case 3:
                tbuf[ui] = static_cast<jchar>(((ch & 0xFu) << 12) |
                                              (TrailByteValue0(sv[i]) << 6u) |
                                              TrailByteValue0(sv[i + 1])
                );
                i += 2;
                break;
            default:
                // Outside the BMP so need two surrogates
                value = static_cast<unsigned int>((ch & 0x7) << 18);
                value |= TrailByteValue0(sv[i]) << 12;
                value |= TrailByteValue0(sv[i + 1]) << 6;
                value |= TrailByteValue0(sv[i + 2]);
                i += 3;
                tbuf[ui] = static_cast<jchar>(((value - 0x10000) >> 10u) + SURROGATE_LEAD_FIRST);
                ui++;
                tbuf[ui] = static_cast<jchar>((value & 0x3ff) + SURROGATE_TRAIL_FIRST);
                break;
        }
        ui++;
    }
    return ui;
}


