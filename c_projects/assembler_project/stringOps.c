#include "stringOps.h"
#include "binaryOps.h"
#include "utilities.h"

extern int IC;
extern int DC;
extern int LC;
extern int isError;
extern int asLineNum;
extern int numOfWords;
extern struct command commandArray[BINARY_ARRAY_MAX];
extern struct data dataArray[DATA_ARRAY_MAX];
extern struct label labelArray[LABELS_ARRAY_MAX];

struct opsList {
    char *name;
	int numOperands;
	int opcode;
	int func;
} operations[] = {
	{"mov", 2, 0, 0},
	{"cmp", 2, 1, 0},
	{"add", 2, 2, 1},
	{"sub", 2, 2, 2},
	{"lea", 2, 4, 0},
	{"clr", 1, 5, 1},
	{"not", 1, 5, 2},
	{"inc", 1, 5, 3},
	{"dec", 1, 5, 4},
	{"jmp", 1, 9, 1},
	{"bne", 1, 9, 2},
	{"jsr", 1, 9, 3},
	{"red", 1, 12, 0},
	{"prn", 1, 13, 0},
	{"rts", 0, 14, 0},
	{"stop", 0, 15, 0}
};

char **breakToWords(char *str)
{
	int i, j, k;
	int inWord = 1;

	char **arr = malloc(MAX_WORDS * sizeof(char *));
	for (j = 0; j < MAX_WORDS; j++)
		arr[j] = malloc(MAX_LETTERS * sizeof(char));

	i = j = k = 0;

	while (str[i] != '\n' && str[i] != '\0') 
	{
		
		if (str[i] != ' ' && str[i] != '\t')
		{
			
			if (k >= MAX_LETTERS)
				arr[j][--k] = '\0';

			
			if (j >= MAX_WORDS)
			{
				arr[--j][0] = '\n';
				arr[j][1] = '\0';
			}
			
			if (inWord && str[i] == ',')
			{
				arr[j][k] = '\0';
				j++;

				k = 0;
				arr[j][k] = str[i];

				inWord = 0;
			}
			
			else if (!inWord)
			{
				if (k != 0)
				{
					arr[j][k] = '\0';
					j++;
					k = 0;
				}
				inWord = 1;
			}
			arr[j][k] = str[i];
			k++;
		}

		else
			inWord = 0;

		i++;
	}

	/*last word "\n"*/
	j++;

	if (j >= MAX_WORDS)
		j--;
	
	arr[j][0] = '\n';
	arr[j][1] = '\0';
	
	return arr;
}

/* checks if first word is label */
int hasLabel(char *line)
{
	int i;
	for (i = 0; line[i] != '\0'; i++);
	return line[--i] == ':';
}

char *getCleanLabel(char *label)
{
	int i;
	char *cleanLabel = (char *)malloc(MAX_LABEL_LENGTH * sizeof(char));

	for (i = 0; label[i] != '\0'; i++)
		cleanLabel[i] = label[i];

	if (cleanLabel[--i] == ':')
		cleanLabel[i] = 0;

	if (!checkLabel(cleanLabel))
	{
		fprintf(stderr, "Error - %s is not a legal label. \n", cleanLabel);
		isError = TRUE;
	}

	return cleanLabel;
}

int checkLabel(char *wrd)/* 1 is legal label */
{
	int i = 0;
	int length = strlen(wrd);
	
	if (!length)
		return FALSE;

	if (wrd[i] < 'A' || wrd[i] > 'z' || (wrd[i] < 'a' && wrd[i] > 'Z'))
		return FALSE;

	if (wrd[i] == 'r' && length == 2)
		for (i = 0; i < 8; i++)
			if ((wrd[1] - '0') == i)
				return FALSE;

	for (i = 1; i < length; i++)
		if (wrd[i] < '0' || (wrd[i] > '9' && wrd[i] < 'A') || ((wrd[i] < 'a' && wrd[i] > 'Z')) || wrd[i] > 'z')
			return FALSE;

	for (i = 0; i < OPERATIONS_NUMBER; i++)
		if (strcmp(operations[i].name, wrd) == 0)
			return FALSE;
	return TRUE;
}

int isNumber(char *str) /* 1 == number*/
{
	if (*str == '+' || *str == '-') /*skip a sign*/
		str++;

	while (*str != '\0')
	{
		if (!(*str >= '0' && *str <= '9'))
			return FALSE;
		str++;
	}
	return TRUE;
}

int isRegister(char *str)
{
	if(str[0] == 'r' && strlen(str) == 2)
	{
		if(str[1] >= '0' && str[1] <= '7') return TRUE;
	}
	return FALSE;
}

int isOperation(char *op)
{
	int i;
	for (i = 0; i < OPERATIONS_NUMBER; i++)
		if (strcmp(op, operations[i].name) == 0)
			return TRUE;
	return FALSE;
}

int wordCount(char **array)
{
	int i;
	for (i = 0; strcmp(array[i], "\n") != 0; i++);
	return i;
}

void zeroCommand(int index)
{
	int i;
	commandArray[index].label = malloc(MAX_LABEL_LENGTH * sizeof(char));
	commandArray[index].address = 0;
    commandArray[index].label[0] = 0;
    commandArray[index].ARE = 0;
    for(i = 0; i < 24; i++) commandArray[index].binary[i] = '0';
}

void zeroData(int index)
{
	int i;
	dataArray[index].label = malloc(MAX_LABEL_LENGTH * sizeof(char));
	dataArray[index].address = 0;
	dataArray[index].label[0] = 0;
	for(i = 0; i < 24; i++) dataArray[index].binary[i] = '0';
}

void zeroLabel(int index)
{
	labelArray[index].name = malloc(MAX_LABEL_LENGTH * sizeof(char));
	labelArray[index].address = 0;
	labelArray[index].name[0] = 0;
	labelArray[index].kind = 5;
	labelArray[index].isEntry = 5;
	labelArray[index].isExt = 5;
}

int addDataToData(char **line, int index)
{
	int i;
	
		
	for(i = index; i < numOfWords; i++)
	{
		if(isNumber(line[i])){
			zeroData(DC);
			convertIntToBinary(dataArray[DC].binary, atoi(line[i]), 0);
			dataArray[DC].address = DC;
			DC++;
		}
		else if(line[i][0] != ',')
		{
			fprintf(stderr, "Warning in line %d: wrong type of .data\n", asLineNum);
		}		
	}
	return 0;
}

void addStringToData(char **line, int index)
{
	int i, j;
		
	for(i = index; i < numOfWords; i++)
	{
		if(line[i][0] != 34 || line[i][strlen(line[i])-1] != 34)
		{
			fprintf(stderr, "Warning in line %d: not string data\n", asLineNum);
			continue;
		}
		
		for(j = 1; j < strlen(line[i])-1; j++){
			zeroData(DC);
			convertIntToBinary(dataArray[DC].binary, line[i][j], 0);
			dataArray[DC].address = DC;
			DC++;
		}
		zeroData(DC);
		dataArray[DC].address = DC;
		DC++;
	}
}

int buildCommand(char **line, int index)		/*labeled=1 if there is label in line, 0 if there isnt*/
{
	int opNum, i, opLength, commandIC;
	
	opLength = numOfWords - index;
	commandIC = IC;
	
	for (i = 0; i < OPERATIONS_NUMBER; i++)
		if (strcmp(line[index], operations[i].name) == 0)
			opNum = i;
	switch (opNum)
	{
	case 0:

		if(opLength < 4)
		{
			fprintf(stderr, "Error in line %d: operation 'mov' - missing operands or commas\n", asLineNum);
			return 1;
		}
		if(opLength > 4)
		{
			fprintf(stderr, "Error in line %d: operation 'mov' - too much operands or commas\n", asLineNum);
			return 1;
		}
		convertUnsignedToBinary(commandArray[IC].binary, 4, 0, 2);

		IC++;
		if(analizeOperand(line[index + 1]) == 1)
		{
			convertUnsignedToBinary(commandArray[commandIC].binary, atoi(&line[index + 1][1]), 13, 15);
			convertUnsignedToBinary(commandArray[commandIC].binary, 3, 16, 17);
		}
		else if(analizeOperand(line[index + 1]) == 2)
		{
			zeroCommand(IC);
			commandArray[IC].address = IC;
			convertIntToBinary(commandArray[IC].binary, atoi(&line[index + 1][1]), 1);
			convertUnsignedToBinary(commandArray[IC].binary, 4, 0, 2);
			IC++;
		}
		else if(analizeOperand(line[index + 1]) == 3)
		{
			convertUnsignedToBinary(commandArray[commandIC].binary, 1, 16, 17);
			zeroCommand(IC);
			commandArray[IC].address = IC;
			strcpy(commandArray[IC].label, line[index + 1]);
			IC++;
		}
		else
		{
			fprintf(stderr, "Error in line %d: wrong parameter\n", asLineNum);
			return 1;
		}


		if(analizeOperand(line[index + 3]) == 1)
		{
			convertUnsignedToBinary(commandArray[commandIC].binary, atoi(&line[index + 3][1]), 8, 10);
			convertUnsignedToBinary(commandArray[commandIC].binary, 3, 11, 12);
		}
		else if(analizeOperand(line[index + 3]) == 3)
		{
			convertUnsignedToBinary(commandArray[commandIC].binary, 1, 11, 12);
			zeroCommand(IC);
			commandArray[IC].address = IC;
			strcpy(commandArray[IC].label, line[index + 3]);
			IC++;
		}
		else
		{
			fprintf(stderr, "Error in line %d: wrong second parameter\n", asLineNum);
			return 1;
		}
		return 0;
		break;
	case 1:

		if(opLength < 4)
		{
			fprintf(stderr, "Error in line %d: operation 'cmp' - missing operands or commas\n", asLineNum);
			return 1;
		}
		if(opLength > 4)
		{
			fprintf(stderr, "Error in line %d: operation 'cmp' - too much operands or commas\n", asLineNum);
			return 1;
		}
		convertUnsignedToBinary(commandArray[IC].binary, 4, 0, 2);
		convertUnsignedToBinary(commandArray[IC].binary, 1, 18, 23);

		IC++;
		if(analizeOperand(line[index + 1]) == 1)
		{
			convertUnsignedToBinary(commandArray[commandIC].binary, atoi(&line[index + 1][1]), 13, 15);
			convertUnsignedToBinary(commandArray[commandIC].binary, 3, 16, 17);
		}
		else if(analizeOperand(line[index + 1]) == 2)
		{
			zeroCommand(IC);
			commandArray[IC].address = IC;
			convertIntToBinary(commandArray[IC].binary, atoi(&line[index + 1][1]), 1);
			convertUnsignedToBinary(commandArray[IC].binary, 4, 0, 2);
			IC++;
		}
		else if(analizeOperand(line[index + 1]) == 3)
		{
			convertUnsignedToBinary(commandArray[commandIC].binary, 1, 16, 17);
			zeroCommand(IC);
			commandArray[IC].address = IC;
			strcpy(commandArray[IC].label, line[index + 1]);
			IC++;
		}
		else
		{
			fprintf(stderr, "Error in line %d: wrong parameter\n", asLineNum);
			return 1;
		}


		if(analizeOperand(line[index + 3]) == 1)
		{
			convertUnsignedToBinary(commandArray[commandIC].binary, atoi(&line[index + 3][1]), 8, 10);
			convertUnsignedToBinary(commandArray[commandIC].binary, 3, 11, 12);
		}
		else if(analizeOperand(line[index + 3]) == 2)
		{
			zeroCommand(IC);
			commandArray[IC].address = IC;
			convertIntToBinary(commandArray[IC].binary, atoi(&line[index + 3][1]), 1);
			convertUnsignedToBinary(commandArray[IC].binary, 4, 0, 2);
			IC++;
		}
		else if(analizeOperand(line[index + 3]) == 3)
		{
			convertUnsignedToBinary(commandArray[commandIC].binary, 1, 11, 12);
			zeroCommand(IC);
			commandArray[IC].address = IC;
			strcpy(commandArray[IC].label, line[index + 3]);
			IC++;
		}
		else
		{
			fprintf(stderr, "Error in line %d: wrong second parameter\n", asLineNum);
			return 1;
		}
		return 0;
		break;
	case 2:
		
		if(opLength < 4)
		{
			fprintf(stderr, "Error in line %d: operation 'add' - missing operands or commas\n", asLineNum);
			return 1;
		}
		if(opLength > 4)
		{
			fprintf(stderr, "Error in line %d: operation 'add' - too much operands or commas\n", asLineNum);
			return 1;
		}
		convertUnsignedToBinary(commandArray[commandIC].binary, 4, 0, 2);
		convertUnsignedToBinary(commandArray[commandIC].binary, 2, 18, 23);
		convertUnsignedToBinary(commandArray[commandIC].binary, 1, 3, 7);

		IC++;
		if(analizeOperand(line[index + 1]) == 1)
		{
			convertUnsignedToBinary(commandArray[commandIC].binary, atoi(&line[index + 1][1]), 13, 15);
			convertUnsignedToBinary(commandArray[commandIC].binary, 3, 16, 17);
		}
		else if(analizeOperand(line[index + 1]) == 2)
		{
			zeroCommand(IC);
			commandArray[IC].address = IC;
			convertIntToBinary(commandArray[IC].binary, atoi(&line[index + 1][1]), 1);
			convertUnsignedToBinary(commandArray[IC].binary, 4, 0, 2);
			IC++;
		}
		else if(analizeOperand(line[index + 1]) == 3)
		{
			convertUnsignedToBinary(commandArray[commandIC].binary, 1, 16, 17);
			zeroCommand(IC);
			commandArray[IC].address = IC;
			strcpy(commandArray[IC].label, line[index + 1]);
			IC++;
		}
		else
		{
			fprintf(stderr, "Error in line %d: wrong parameter\n", asLineNum);
			return 1;
		}


		if(analizeOperand(line[index + 3]) == 1)
		{
			convertUnsignedToBinary(commandArray[commandIC].binary, atoi(&line[index + 3][1]), 8, 10);
			convertUnsignedToBinary(commandArray[commandIC].binary, 3, 11, 12);
		}
		else if(analizeOperand(line[index + 3]) == 3)
		{
			convertUnsignedToBinary(commandArray[commandIC].binary, 1, 11, 12);
			zeroCommand(IC);
			commandArray[IC].address = IC;
			strcpy(commandArray[IC].label, line[index + 3]);
			IC++;
		}
		else
		{
			fprintf(stderr, "Error in line %d: wrong second parameter\n", asLineNum);
			return 1;
		}
		return 0;
		break;
	case 3:

		if(opLength < 4)
		{
			fprintf(stderr, "Error in line %d: operation 'sub' - missing operands or commas\n", asLineNum);
			return 1;
		}
		if(opLength > 4)
		{
			fprintf(stderr, "Error in line %d: operation 'sub' - too much operands or commas\n", asLineNum);
			return 1;
		}
		convertUnsignedToBinary(commandArray[IC].binary, 4, 0, 2);
		convertUnsignedToBinary(commandArray[IC].binary, 2, 18, 23);
		convertUnsignedToBinary(commandArray[IC].binary, 2, 3, 7);

		IC++;
		if(analizeOperand(line[index + 1]) == 1)
		{
			convertUnsignedToBinary(commandArray[commandIC].binary, atoi(&line[index + 1][1]), 13, 15);
			convertUnsignedToBinary(commandArray[commandIC].binary, 3, 16, 17);
		}
		else if(analizeOperand(line[index + 1]) == 2)
		{
			zeroCommand(IC);
			commandArray[IC].address = IC;
			convertIntToBinary(commandArray[IC].binary, atoi(&line[index + 1][1]), 1);
			convertUnsignedToBinary(commandArray[IC].binary, 4, 0, 2);
			IC++;
		}
		else if(analizeOperand(line[index + 1]) == 3)
		{
			convertUnsignedToBinary(commandArray[commandIC].binary, 1, 16, 17);
			zeroCommand(IC);
			commandArray[IC].address = IC;
			strcpy(commandArray[IC].label, line[index + 1]);
			IC++;
		}
		else
		{
			fprintf(stderr, "Error in line %d: wrong parameter\n", asLineNum);
			return 1;
		}


		if(analizeOperand(line[index + 3]) == 1)
		{
			convertUnsignedToBinary(commandArray[commandIC].binary, atoi(&line[index + 3][1]), 8, 10);
			convertUnsignedToBinary(commandArray[commandIC].binary, 3, 11, 12);
		}
		else if(analizeOperand(line[index + 3]) == 3)
		{
			convertUnsignedToBinary(commandArray[commandIC].binary, 1, 11, 12);
			zeroCommand(IC);
			commandArray[IC].address = IC;
			strcpy(commandArray[IC].label, line[index + 3]);
			IC++;
		}
		else
		{
			fprintf(stderr, "Error in line %d: wrong second parameter\n", asLineNum);
			return 1;
		}
		return 0;
		break;


	case 4:

		if(opLength < 4)
		{
			fprintf(stderr, "Error in line %d: operation 'lea' - missing operands or commas\n", asLineNum);
			return 1;
		}
		if(opLength > 4)
		{
			fprintf(stderr, "Error in line %d: operation 'lea' - too much operands or commas\n", asLineNum);
			return 1;
		}
		convertUnsignedToBinary(commandArray[IC].binary, 4, 0, 2);
		convertUnsignedToBinary(commandArray[IC].binary, 4, 18, 23);

		IC++;
		if(analizeOperand(line[index + 1]) == 3)
		{
			convertUnsignedToBinary(commandArray[commandIC].binary, 1, 16, 17);
			zeroCommand(IC);
			commandArray[IC].address = IC;
			strcpy(commandArray[IC].label, line[index + 1]);
			IC++;
		}
		else
		{
			fprintf(stderr, "Error in line %d: wrong parameter\n", asLineNum);
			return 1;
		}


		if(analizeOperand(line[index + 3]) == 1)
		{
			convertUnsignedToBinary(commandArray[commandIC].binary, atoi(&line[index + 3][1]), 8, 10);
			convertUnsignedToBinary(commandArray[commandIC].binary, 3, 11, 12);
		}
		else if(analizeOperand(line[index + 3]) == 3)
		{
			convertUnsignedToBinary(commandArray[commandIC].binary, 1, 11, 12);
			zeroCommand(IC);
			commandArray[IC].address = IC;
			strcpy(commandArray[IC].label, line[index + 3]);
			IC++;
		}
		else
		{
			fprintf(stderr, "Error in line %d: wrong second parameter\n", asLineNum);
			return 1;
		}
		return 0;
		break;


	case 5:
		if(opLength != 2)
		{
			fprintf(stderr, "Error in line %d: operation 'clr' must have one parameter\n", asLineNum);
			return 1;
		}
		if(analizeOperand(line[index + 1]) == 1)
		{
			convertOperandToBinary(commandArray[IC].binary, 4, 1, atoi(&line[index + 1][1]), 3, 0, 0, 5);
			commandArray[IC].address = IC;
			IC++;
			return 0;
		}
		else if(analizeOperand(line[index + 1]) == 3)
		{
			convertOperandToBinary(commandArray[IC].binary, 4, 1, 0, 1, 0, 0, 5);
			commandArray[IC].address = IC;
			IC++;
			zeroCommand(IC);
			commandArray[IC].address = IC;
			strcpy(commandArray[IC].label, line[index + 1]);
			IC++;
			return 0;
		}
		fprintf(stderr, "Error in line %d: wrong parameter\n", asLineNum);
		return 1;
		break;


	case 6:
		if(opLength != 2)
		{
			fprintf(stderr, "Error in line %d: operation 'not' must have one parameter\n", asLineNum);
			return 1;
		}
		if(analizeOperand(line[index + 1]) == 1)
		{
			convertOperandToBinary(commandArray[IC].binary, 4, 2, atoi(&line[index + 1][1]), 3, 0, 0, 5);
			commandArray[IC].address = IC;
			IC++;
			return 0;
		}
		else if(analizeOperand(line[index + 1]) == 3)
		{
			convertOperandToBinary(commandArray[IC].binary, 4, 2, 0, 1, 0, 0, 5);
			commandArray[IC].address = IC;
			IC++;
			zeroCommand(IC);
			commandArray[IC].address = IC;
			strcpy(commandArray[IC].label, line[index + 1]);
			IC++;
			return 0;
		}
		fprintf(stderr, "Error in line %d: wrong parameter\n", asLineNum);
		return 1;
		break;


	case 7:
		if(opLength != 2)
		{
			fprintf(stderr, "Error in line %d: operation 'inc' must have one parameter\n", asLineNum);
			return 1;
		}
		if(analizeOperand(line[index + 1]) == 1)
		{
			convertOperandToBinary(commandArray[IC].binary, 4, 3, atoi(&line[index + 1][1]), 3, 0, 0, 5);
			commandArray[IC].address = IC;
			IC++;
			return 0;
		}
		else if(analizeOperand(line[index + 1]) == 3)
		{
			convertOperandToBinary(commandArray[IC].binary, 4, 3, 0, 1, 0, 0, 5);
			commandArray[IC].address = IC;
			IC++;
			zeroCommand(IC);
			commandArray[IC].address = IC;
			strcpy(commandArray[IC].label, line[index + 1]);
			IC++;
			return 0;
		}
		fprintf(stderr, "Error in line %d: wrong parameter\n", asLineNum);
		return 1;
		break;


	case 8:
		if(opLength != 2)
		{
			fprintf(stderr, "Error in line %d: operation 'dec' must have one parameter\n", asLineNum);
			return 1;
		}
		if(analizeOperand(line[index + 1]) == 1)
		{
			convertOperandToBinary(commandArray[IC].binary, 4, 4, atoi(&line[index + 1][1]), 3, 0, 0, 5);
			commandArray[IC].address = IC;
			IC++;
			return 0;
		}
		else if(analizeOperand(line[index + 1]) == 3)
		{
			convertOperandToBinary(commandArray[IC].binary, 4, 4, 0, 1, 0, 0, 5);
			commandArray[IC].address = IC;
			IC++;
			zeroCommand(IC);
			commandArray[IC].address = IC;
			strcpy(commandArray[IC].label, line[index + 1]);
			IC++;
			return 0;
		}
		fprintf(stderr, "Error in line %d: wrong parameter\n", asLineNum);
		return 1;
		break;


	case 9:
		if(opLength != 2)
		{
			fprintf(stderr, "Error in line %d: operation 'bne' must have one parameter\n", asLineNum);
			return 1;
		}
		if(line[index + 1][0] == '&')
		{
			if(analizeOperand(&line[index + 1][1]) == 3)
			{
				convertOperandToBinary(commandArray[IC].binary, 4, 1, 0, 2, 0, 0, 9);
				commandArray[IC].address = IC;
				IC++;
				zeroCommand(IC);
				commandArray[IC].address = IC;
				strcpy(commandArray[IC].label, line[index + 1]);
				IC++;
				return 0;
			}
		}
		fprintf(stderr, "Error in line %d: wrong parameter\n", asLineNum);
		return 1;
		break;


	case 10:
		if(opLength != 2)
		{
			fprintf(stderr, "Error in line %d: operation 'bne' must have one parameter\n", asLineNum);
			return 1;
		}
		if(line[index + 1][0] == '&')
		{
			if(analizeOperand(&line[index + 1][1]) == 3)
			{
				convertOperandToBinary(commandArray[IC].binary, 4, 2, 0, 2, 0, 0, 9);
				commandArray[IC].address = IC;
				IC++;
				zeroCommand(IC);
				commandArray[IC].address = IC;
				strcpy(commandArray[IC].label, line[index + 1]);
				IC++;
				return 0;
			}
		}
		else if(analizeOperand(line[index + 1]) == 3)
		{
			convertOperandToBinary(commandArray[IC].binary, 4, 2, 0, 1, 0, 0, 9);
			commandArray[IC].address = IC;
			IC++;
			zeroCommand(IC);
			commandArray[IC].address = IC;
			strcpy(commandArray[IC].label, line[index + 1]);
			IC++;
			return 0;
		}
		fprintf(stderr, "Error in line %d: wrong parameter\n", asLineNum);
		return 1;
		break;


	case 11:
		if(opLength != 2)
		{
			fprintf(stderr, "Error in line %d: operation 'jsr' must have one parameter\n", asLineNum);
			return 1;
		}
		else if(analizeOperand(line[index + 1]) == 3)
		{
			convertOperandToBinary(commandArray[IC].binary, 4, 3, 0, 1, 0, 0, 9);
			commandArray[IC].address = IC;
			IC++;
			zeroCommand(IC);
			commandArray[IC].address = IC;
			strcpy(commandArray[IC].label, line[index + 1]);
			IC++;
			return 0;
		}
		fprintf(stderr, "Error in line %d: wrong parameter\n", asLineNum);
		return 1;
		break;


	case 12:
		if(opLength != 2)
		{
			fprintf(stderr, "Error in line %d: operation 'red' must have one parameter\n", asLineNum);
			return 1;
		}
		if(analizeOperand(line[index + 1]) == 1)
		{
			convertOperandToBinary(commandArray[IC].binary, 4, 0, atoi(&line[index + 1][1]), 3, 0, 0, 12);
			commandArray[IC].address = IC;
			IC++;
			return 0;
		}
		else if(analizeOperand(line[index + 1]) == 3)
		{
			convertOperandToBinary(commandArray[IC].binary, 4, 0, 0, 1, 0, 0, 12);
			commandArray[IC].address = IC;
			IC++;
			zeroCommand(IC);
			commandArray[IC].address = IC;
			strcpy(commandArray[IC].label, line[index + 1]);
			IC++;
			return 0;
		}
		fprintf(stderr, "Error in line %d: wrong parameter\n", asLineNum);
		return 1;
		break;
	
	
	case 13:
		if(opLength != 2)
		{
			fprintf(stderr, "Error in line %d: operation 'prn' must have one parameter\n", asLineNum);
			return 1;
		}
		if(analizeOperand(line[index + 1]) == 1)
		{
			convertOperandToBinary(commandArray[IC].binary, 4, 0, atoi(&line[index + 1][1]), 3, 0, 0, 13);
			commandArray[IC].address = IC;
			IC++;
			return 0;
		}
		else if(analizeOperand(line[index + 1]) == 2)
		{
			convertOperandToBinary(commandArray[IC].binary, 4, 0, 0, 0, 0, 0, 13);
			commandArray[IC].address = IC;
			IC++;
			zeroCommand(IC);
			commandArray[IC].address = IC;
			convertIntToBinary(commandArray[IC].binary, atoi(&line[index + 1][1]), 1);
			commandArray[IC].binary[0] = '0';
			commandArray[IC].binary[1] = '0';
			commandArray[IC].binary[2] = '1';
			IC++;
			return 0;
		}
		else if(analizeOperand(line[index + 1]) == 3)
		{
			convertOperandToBinary(commandArray[IC].binary, 4, 0, 0, 1, 0, 0, 13);
			commandArray[IC].address = IC;
			IC++;
			zeroCommand(IC);
			commandArray[IC].address = IC;
			strcpy(commandArray[IC].label, line[index + 1]);
			IC++;
			return 0;
		}
		fprintf(stderr, "Error in line %d: wrong parameter\n", asLineNum);
		return 1;
		break;
	
	
	case 14:
		if(opLength > 1)
		{
			fprintf(stderr, "Error in line %d: operation 'rts' must have no parameters\n", asLineNum);
			return 1;
		}
		convertOperandToBinary(commandArray[IC].binary, 4, 0, 0, 0, 0, 0, 14);
		commandArray[IC].address = IC;
		IC++;
		return 0;
		break;
	
	
	case 15:
		if(opLength > 1)
		{
			fprintf(stderr, "Error in line %d: operation 'stop' must have no parameters\n", asLineNum);
			return 1;
		}
		convertOperandToBinary(commandArray[IC].binary, 4, 0, 0, 0, 0, 0, 15);
		commandArray[IC].address = IC;
		IC++;
		return 0;
		break;
	}


	asLineNum++;
	return 0;
}

int labelPresents(char *p)
{
	int i;
	for(i = 0; i < LC; i++)
	{
        if(!strcmp(p, labelArray[i].name)) return TRUE;
    }
	return FALSE;
}

int analizeOperand(char *op)/* 0 is unknown, 1-register, 2-number, 3-label*/
{
	int i, flag;
	flag = 0;

	if(isRegister(op)) return 1;
	
	if(op[0] == '#' && isNumber(&op[1]))
	{
		for(i = 1; i < strlen(op); i++)
			if((op[i] < '0' || op[i] > '9')&& op[1] != '-') flag = 1;
		if(!flag)return 2;
	}
	
	if(checkLabel(op)) return 3;

	return 0;
}

int getLabelNum(char * str)
{
	int i;
	for(i = 0; i < LC; i++)
	{
        if(!strcmp(str, labelArray[i].name)) return i;
    }
	return 0;
}