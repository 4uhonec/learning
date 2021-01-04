#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define IC_START_VALUE 100
#define BINARY_ARRAY_MAX 1000
#define DATA_ARRAY_MAX 200
#define LABELS_ARRAY_MAX 100
#define BINARY_LENGTH 24  /*length of binary word*/
#define ASSEMBLY_LINE_MAX 80  /*max length of assembly line*/
#define MAX_WORDS 20 /*max words in assembly line*/
#define MAX_LETTERS 30
#define MAX_LABEL_LENGTH 20

#define OPERATIONS_NUMBER 16



#ifndef STRUCTS
#define STRUCTS
enum BOOLEAN {FALSE, TRUE};
struct command {
    int address;
    char *label;
    int ARE;
    char binary[BINARY_LENGTH];
};

struct data{
    int address;
    char binary[BINARY_LENGTH];
    char *label;
};

struct label{
    char *name;
    int address;
    int kind; /*1-code, 0-data*/
    int isExt;
    int isEntry;
};

#endif