
#include <ctype.h>

#include <stdbool.h>
#include <stdint.h>

#include <stdlib.h>
#include <stdio.h>

enum jType /*: char unsigned*/ {
    jNull,
    jBoolean,
    jNumber,
    jString,
    jCollection,
    jObject
};

typedef struct Jbasic {
    enum jType type;
    union {
        void*  empty;
        bool   boolean;
        double number;
        wchar_t*  string;
    };

}   Jbasic;

typedef struct Jcollection {
    uint16_t       size;
    enum jType*    type;
    struct Jvalue* value;

}   Jcollection;

typedef struct Jobject {
    uint16_t       size;
    char**         key;
    enum jType*    type;
    struct Jvalue* value;

}   Jobject;

struct Jvalue {
    enum jType type;
    union {
        Jbasic      basic;
        Jcollection collection;
        Jobject     object;
    };
};

struct JSON {
    char* file_name;
    struct Jvalue root;
};

struct ParseString {
    uint16_t size;
    fpos_t   byte;
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
    union ParseNumber_bit_flags flags;

    uint16_t base_size, fraction_size, expoent_size;
};

struct ParseString ParseString(FILE* const source, bool const rewind_stream)
{
    struct ParseString str = {.size = 0, .byte = ftell(source)};

    if ('"' != fgetc(source)) {
        str.byte = EOF;

        return str;
    }

    for (;;) {
        if ('"' == fgetc(source))
            break;

        ++str.size;
    }

    if (rewind_stream)
        fseek(source, str.byte - ftell(source), SEEK_CUR);

    return str;
}

struct ParseString ParseValue(FILE* const source)
{
    fpos_t const value_origin = ftell(source);

    uint8_t word_index = 0, expression_length = 0;

    switch (fgetc(source)) {
        static char const *const reserved_words[3] = {"null", "true", "false"};

        char const* expression;

    case 'f': ++word_index, expression_length  = 1;
    case 'n': ++word_index;
    case 't': ++word_index, expression_length += 4;

        expression = reserved_words[word_index];

        for (uint8_t c_cmp = 1; c_cmp < expression_length; ++c_cmp)
            if (expression[c_cmp] != fgetc(source))
                ; // ParseError()

        switch (fgetc(source)) {
        case ',':
        case ']':
        case '}': fseek(source, -1, SEEK_CUR); break;

        default:
            ; // ParseError()

            break;
        }

        return (struct ParseString){expression_length, value_origin};

    default:
        fseek(source, -1, SEEK_CUR);

        // -- Implement -- Numbers

        break;
    }

    return (struct ParseString){.size = 0, .byte = EOF};
}

struct ParseNumber implement__NumberParsing(FILE* const source)
{
    union ParseNumber_bit_flags pFlags = {.bits = 0};

    uint16_t 
        base_size     = 0,
        fraction_size = 0,
        expoent_size  = 0;

    for (;;)
    {
        char const character = fgetc(source);

        switch (character) {
        case ',':
        case ']':
        case '}':
            if (! pFlags.base_value)
                ; // ParseError()

            if (0134 ^ (0134 & pFlags.bits))
                ; // ParseError()

            fseek(source, -1, SEEK_CUR);

            return (struct ParseNumber){pFlags, base_size, fraction_size, expoent_size};
        case '+':
        case '-':
            if (! pFlags.base_value)
                pFlags.base_signal = true;

            else {
                if (!pFlags.expoent_notation || pFlags.expoent_value)
                    ; //ParseError()

                pFlags.expoent_signal = true;
            }

            break;
        case '.':
            if (false == pFlags.base_value || pFlags.fraction_notation)
                ; // ParseError()

            pFlags.fraction_notation = true;

            break;
        case 'e':
        case 'E':
            if (pFlags.expoent_notation)
                ; // ParseError()

            if (false == (pFlags.fraction_value || (pFlags.fraction_notation ^ pFlags.base_value)))
                ; // ParseError()

            pFlags.expoent_notation = true;

            break;

        default:
            if(isalpha(character))
                ; // ParseError()

            if (! isdigit(character))
                ; // ParseError()

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
}

void ParseNode(FILE* const source, bool const node_with_propertyes)
{
    do
    {
        union {
            bool bracket_type;
            bool property_actision;

        }   bool_share = {node_with_propertyes};

        string_attribute_parsing:

        switch (fgetc(source))
        {
            static char const close_bracket[2] = {']','}'};
        case '"':
            fseek(source, -1, SEEK_CUR);

            // = ParseString(source, false);

            if (bool_share.property_actision) {
                if (':' != fgetc(source))
                    ; // ParseError()

                bool_share.property_actision = false;

                goto string_attribute_parsing;
            }

            break;
        case '{':
            bool_share.bracket_type = true;
        case '[':
            // = ParseNode(source, bracket_type);

            if (close_bracket[bool_share.bracket_type] != fgetc(source))
                ; // ParseError()

            break;

        default:
            fseek(source, -1, SEEK_CUR);

            // = ParseValue(source);

            break;
        }
    }
    while (',' == fgetc(source));

    if (! feof(source))
       fseek(source, -1, SEEK_CUR);

    return ;
}


int main(int argc, char *argv[])
{
    exit(0);
}

