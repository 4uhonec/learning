#include "utilities.h"
#include "binaryOps.h"

void firstPass(FILE * file);
void secondPass(void);
char **breakToWords(char *string);
void createObFile(FILE *file);
void createExtFile(FILE *file);
void createEntFile(FILE *file);

#ifndef OPSLIST
#define OPSLIST
struct opsList {
    char *name;
	int numOperands;
	int opcode;
	int func;
};
#endif