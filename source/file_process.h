
#pragma once

#include "preprocess.h"

#include <stdlib.h>

double file_process_number(FILE* const source, struct ParseNumber const parse)
// Need to be tested
{
    double ret;

    if (ftell(source) != parse.byte)
        fsetpos(source, &parse.byte);

    fscanf(source, "%lf", &ret);

    return ret;
}

char* file_process_string(FILE* const source, struct ParseString const parse, char* dest)
{
    if (NULL == dest || parse.size != strlen(dest))
        dest = realloc(dest, sizeof(char[1 + parse.size]));

    if (parse.byte != ftell(source))
        fsetpos(source, &parse.byte);

    fread(dest, 1, parse.size, source);

    dest[parse.size] = '\0';

    return dest;
}

struct Jbasic* file_process_reserved_word(FILE* const source, struct ParseReservedWord parse, struct Jbasic* dest)
{
    if (ftell(source) != parse.byte)
        fsetpos(source, &parse.byte);

    switch (parse.type) {
    default:
    case jError:
        // exit(1);
        break;

    case jNull:
        if (NULL == dest)
            dest = malloc(sizeof(sizeof(struct Jbasic)));

        dest->empty = NULL;

        break;
    case jBoolean:
        if (NULL == dest)
            dest = malloc(sizeof(sizeof(struct Jbasic)));

        if (false == (dest->boolean = parse.value))
            fseek(source, 1, SEEK_CUR);

        break;
    }

    fseek(source, 4, SEEK_CUR);

    return dest;
}

