



char **breakToWords(char *string);
int hasLabel(char *line);
char *getCleanLabel(char *label);
int checkLabel(char *word);
int isRegister(char *str);
int isNumber(char *str);
int isOperation(char *op);
int labelPresents(char *p);
int getLabelNum(char * str);

int wordCount(char **array);

void zeroCommand(int index);
void zeroData(int index);
void zeroLabel(int index);

int addDataToData(char **line, int index);
void addStringToData(char **line, int index);
int buildCommand(char **line, int labeled);
int analizeOperand(char *op);