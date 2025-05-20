
#pragma once

#include "json_structure.h"

#include <ctype.h>
#include <string.h>

struct ParseString {
    fpos_t   byte;
    uint16_t size;
};

union ParseNumber_bit_flags {
    uint8_t bits :7;

    struct {
        uint8_t 
            base_signal       :1,
            base_value        :1,
            fraction_notation :1,
            fraction_value    :1,
            expoent_notation  :1,
            expoent_signal    :1,
            expoent_value     :1;
    };
};

struct ParseNumber {
    fpos_t byte;
    union ParseNumber_bit_flags flags;

    uint8_t base_size, fraction_size, expoent_size;
};

struct ParseReservedWord {
    fpos_t byte;
    enum jType type;
    bool value;
};

struct ParseString preprocess_string(FILE* const source, bool const rewind_stream)
{
    if ('"' != fgetc(source)) {
        fseek(source, -1, SEEK_CUR);

        return (struct ParseString){EOF, 0};
    }

    struct ParseString str = {.byte = ftell(source), .size = 0};

    while (true) {
        if (feof(source)) {
            fseek(source, str.byte - ftell(source), SEEK_CUR);

            return (struct ParseString){EOF, 0};
        }

        if ('"' == fgetc(source))
            break;

        ++str.size;
    }

    if (rewind_stream)
        fseek(source, str.byte - ftell(source), SEEK_CUR);

    return str;
}

struct ParseNumber preprocess_number(FILE* const source)
{
    fpos_t const f_origin = { ftell(source) };

    union ParseNumber_bit_flags pFlags = {.bits = 0};

    uint8_t 
        base_size     = 0,
        fraction_size = 0,
        expoent_size  = 0;

    while (true)
    {
        char const character = fgetc(source);

        switch (character) {
        case ' ':
        case ',':
        case ']':
        case '}':
        case EOF:
            if (! pFlags.base_value)
                goto ParseError;

            switch (0136 & pFlags.bits) {
            case 0136:
            case 0122:
            case 0016:
            case 0002:
                fseek(source, -1, SEEK_CUR);

                return (struct ParseNumber){f_origin, pFlags, base_size, fraction_size, expoent_size};

            default:
                goto ParseError;
            }

            break;
        case '+':
        case '-':
            if (! pFlags.base_value)
                pFlags.base_signal = true;

            else if (false == pFlags.expoent_notation || pFlags.expoent_value)
                goto ParseError;

            pFlags.expoent_signal = true;

            break;
        case '.':
            if (false == pFlags.base_value || pFlags.fraction_notation)
                goto ParseError;

            pFlags.fraction_notation = true;

            break;
        case 'e':
        case 'E':
            if (pFlags.expoent_notation)
                goto ParseError;

            if (false == (pFlags.fraction_value || (pFlags.fraction_notation ^ pFlags.base_value)))
                goto ParseError;

            pFlags.expoent_notation = true;

            break;

        default:
            if(isalpha(character) || !isdigit(character))
                goto ParseError;

            switch (0136 & pFlags.bits) {
            case 0036:
            case 0022:
                pFlags.expoent_value = true;
            case 0136:
            case 0122:
                ++expoent_size;
                break;
            case 0006:
                pFlags.fraction_value = true;
            case 0016:
                ++fraction_size;
                break;
            case 0000:
                pFlags.base_value = true;
            case 0002:
                ++base_size;
                break;
            }
        }
    }

    ParseError:

    fseek(source, f_origin - ftell(source), SEEK_CUR);

    return (struct ParseNumber) {EOF, {0}, 0, 0, 0};
}

struct ParseReservedWord preprocess_reserved_word(FILE* const source)
{
    fpos_t const f_origin = ftell(source);

    uint8_t word_index = 0, expression_length = 0;

    switch (fgetc(source))
    {
        static char const* const reserved_words[3] = {"null", "true", "false"};

        char const* expression;

    case 'f': ++word_index, expression_length = 1;
    case 't': ++word_index;
    case 'n':
        expression_length += 3;

        expression = 1 + reserved_words[word_index];

        while (true) {
            if (*expression != fgetc(source) || !(--expression_length))
                break;

            ++expression;
        }

        switch (fgetc(source)) {
        case ' ':
        case ',':
        case ']':
        case '}':
            fseek(source, -1, SEEK_CUR);
        case EOF:
            if (! word_index)
                return (struct ParseReservedWord) {f_origin, jNull, false};

            return (struct ParseReservedWord) {f_origin, jBoolean, (1 == word_index) ? true : false};

        default: break;
        }

    default: break;
    }

    fseek(source, f_origin - ftell(source), SEEK_CUR);

    return (struct ParseReservedWord) {EOF, jError, false};
}

