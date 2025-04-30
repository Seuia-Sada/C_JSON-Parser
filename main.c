
#include <stdbool.h>
#include <stdint.h>

#include <stdlib.h>
#include <stdio.h>

enum jType /*: char unsigned*/ {
    jNULL,
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

        /*
        if (feof(source)) {
            str.size = 0;
            str.byte = EOF;

            return str;
        }
        */

        ++str.size;
    }

    if (rewind_stream)
        fseek(source, str.byte - ftell(source), SEEK_CUR);

    return str;
}

struct ParseString ParseValue(FILE* const source)
{
    uint8_t word_index = 0, expression_length = 0;

    struct ParseString value = {.size = 0, .byte = ftell(source)};

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

        value.size = expression_length;

        return value;

    default:
        fseek(source, -1, SEEK_CUR);
        break;
    }

    // -- Implement -- Numbers

    value.byte = EOF;

    return value;
}

void ParseNode(FILE* const source, bool const node_with_propertyes)
{
    union {
        bool bracket_type;
        bool property_actision;

    }   bool_share = {node_with_propertyes};

    do
    {
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

            bool_share.property_actision = node_with_propertyes;

            break;
        case '{':
            bool_share.bracket_type = true;
        case '[':
            // = ParseNode(source, bracket_type);

            if (close_bracket[bool_share.bracket_type] != fgetc(source))
                ; // ParseError()

            bool_share.property_actision = node_with_propertyes;

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

