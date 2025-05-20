
#pragma once

#include <stdbool.h>
#include <stdint.h>

#include <stdio.h>

enum jType {
    jError = -1,
    jNull,
    jBoolean,
    jNumber,
    jString,
    jCollection,
    jObject
};

typedef struct Jbasic {
    enum jType   type;
    union {
        void*    empty;
        bool     boolean;
        double   number;
        char*    string;
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

struct JSON_file {
    char* name;
    FILE* object;
};

struct JSON {
    struct JSON_file file;
    struct Jvalue    root;
};

