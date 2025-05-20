
#pragma once

#include "file_process.h"

struct Jvalue* extract_number(FILE* const source, struct ParseNumber const parse, struct Jvalue* dest)
{
    if (EOF == parse.byte)
        ; // exit(1);

    switch (dest->type) {
    case jString:
        free(dest->basic.string);
    default:
        dest->type       = jNumber;
        dest->basic.type = jNumber;

    case jNumber:
        break;
    }

    dest->basic.number = file_process_number(source, parse);

    return dest;
}

struct Jvalue* extract_string(FILE* const source, struct ParseString const parse, struct Jvalue* dest)
{
    if (EOF == parse.byte)
        ; // exit(1);

    char* memory;

    switch (dest->type) {
    case jString:
        memory = dest->basic.string;

        break;

    default:
        dest->type       = jString;
        dest->basic.type = jString;

        memory = malloc(sizeof(char[1 + parse.size]));

        break;
    }

    dest->basic.string = file_process_string(source, parse, memory);

    return dest;
}

struct Jvalue* extract_reserved_word(FILE* const source, struct ParseReservedWord const parse, struct Jvalue* dest)
{
    if (EOF == parse.byte)
        ; // exit(1);

    dest->type       = parse.type;
    dest->basic.type = parse.type;

    file_process_reserved_word(source, parse, &dest->basic);

    return dest;
}

struct Jvalue* extract(FILE* const source, enum jType const type, void const* const structure, struct Jvalue* dest)
{
    if (EOF == *(fpos_t*) structure)
        ; // exit(1);

    switch (type) {
    case jError:
        // exit(1);

        break;
    case jNull:
    case jBoolean:
        return extract_reserved_word(source, *(struct ParseReservedWord*) structure, dest);

    case jNumber:
        return extract_number(source, *(struct ParseNumber*) structure, dest);

    case jString:
        return extract_string(source, *(struct ParseString*) structure, dest);

    case jCollection:
    case jObject:
        break;
    }

    return dest;
}

