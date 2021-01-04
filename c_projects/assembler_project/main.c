

/*Project done by Sergei Vilensky*/

#include "main.h"
#include "utilities.h"
#include "stringOps.h"
#include <stdio.h>

int isError;

int asLineNum;
int ICfinal;
int IC;
int DC;
int LC;/*label counter*/
int entrySize;
int externSize;
int numOfWords;
struct command commandArray[BINARY_ARRAY_MAX];
struct data dataArray[DATA_ARRAY_MAX];
struct label labelArray[LABELS_ARRAY_MAX];
extern struct opsList operations[16];


int main(int argc, char *argv[])
{
    char fileName[MAX_LETTERS];
    FILE *file;
    int i, len;
    if(argc == 1)
    {
        fprintf(stderr, "Error: no input file was entered \n");
    }

    for(i = 1; i<argc; i++)
    {
        
        int j,k;
        isError = 0;
        IC = IC_START_VALUE;
        DC = entrySize = externSize = 0;

        strcpy(fileName, argv[i]);
        printf("\n---------- file %s.as ---------\n\n", fileName);
        len = strlen(fileName);
        file = fopen(strcat(fileName, ".as"), "r");
        fileName[len] = '\0';
        if(!file)
        {
            fprintf(stderr, "Error: file %s does not exist \n", fileName);
            continue;
        }
        
        firstPass(file);

        /*some arrangements before sec. pass, adding .data to end of binary array*/        
        if(isError) continue;
        ICfinal = IC;
        for(j = 0; j < DC; j++)
        {
            zeroCommand(IC);
            commandArray[IC].address = IC;
            strcpy(commandArray[IC].binary, dataArray[j].binary);
            IC++;
        }
        for(j = 0; j <= LC; j++)
        {
            if(labelArray[j].kind == 0) labelArray[j].address += ICfinal;
        }
        
        for(j = 0; j <= LC; j++)
        {
            if(labelArray[j].isEntry == 1)
            {
                for(k = 1; k < LC; k++)
                {
                    if(j!=k)
                    if(!strcmp(labelArray[j].name, labelArray[k].name))
                        labelArray[j].address = labelArray[k].address;
                }
            }
        }
        
        secondPass();

        if(!isError)
        {
            file = fopen(strcat(fileName, ".ob"), "w");
			fileName[len] = '\0';
            createObFile(file);
            if(entrySize > 0)
            {
                file = fopen(strcat(fileName, ".ent"), "w");
                fileName[len] = '\0';
                createEntFile(file);
            }
            if(externSize > 0)
            {
                file = fopen(strcat(fileName, ".ext"), "w");
			    fileName[len] = '\0';
                createExtFile(file);
            }
        }
        fclose(file);
    }
    return 0;
}

void firstPass(FILE *file)
{
    char asLine[ASSEMBLY_LINE_MAX+1];
    char **line;
    asLineNum = 1;
    LC = 0;

    while(fgets(asLine, ASSEMBLY_LINE_MAX, file))
    {
        int i = 0;
        
            /*check if line is a comment or blank*/
        while(asLine[i] == ' ' || asLine[i] == '\t') i++;
        if(asLine[i] == ';' || asLine[i] == '\n')
        {
            asLineNum++;
            continue; 
        }

        line = breakToWords(asLine);
        numOfWords = wordCount(line);

        if(hasLabel(line[0]) && numOfWords < 2)
        {
            isError = TRUE;
            fprintf(stderr, "Error in line %d: blank label %s\n", asLineNum, line[0]);
            asLineNum++;
            continue;
        }
        
        /*first word is a label*/
        if(hasLabel(line[0]))
        {
            if(labelPresents(getCleanLabel(line[0])))
            {
                
                if(labelArray[getLabelNum(getCleanLabel(line[0]))].isEntry != 1 && labelArray[getLabelNum(getCleanLabel(line[0]))].isExt != 1)
                {
                    isError = 1;
                    fprintf(stderr, "Error in line %d: label %s already initiated\n", asLineNum++, getCleanLabel(line[0]));
                    continue;
                }
                
                
                
            }
            
            zeroLabel(LC);
            strcpy(labelArray[LC].name, getCleanLabel(line[0]));
            
            /*second word is .data*/
            if(!strcmp(line[1], ".data"))
            {
                int err;
                if(numOfWords < 3)
                {
                    isError = TRUE;
                    fprintf(stderr, "Error in line %d: empty .data\n", asLineNum++);
                    continue;
                }
                labelArray[LC].kind = 0;
                labelArray[LC].address = DC;
                strcpy(labelArray[LC].name, getCleanLabel(line[0]));
                err = addDataToData(line, 2);
                if(err)
                {
                    isError = TRUE;
                    asLineNum++;
                    continue;
                }
            }
                        
            /*second word is .string*/
            if(!strcmp(line[1], ".string"))
            {
                if(numOfWords < 3)
                {
                    fprintf(stderr, "Warning in line %d: no .string data\n", asLineNum);
                    continue;
                }
                if(line[2][0] != 34 && line[2][strlen(line[2])-1] != 34)
                {
                    isError = TRUE;
                    fprintf(stderr, "Error in line %d: data is not string type", asLineNum++);
                    continue;
                }
                labelArray[LC].kind = 0;
                labelArray[LC].address = DC;
                strcpy(labelArray[LC].name, getCleanLabel(line[0]));
                addStringToData(line, 2);
            }

            /*second word is operation*/
            if(isOperation(line[1]))
            {
                int err;
                zeroCommand(IC);
                commandArray[IC].address = IC;
                labelArray[LC].address = IC;
                labelArray[LC].kind = 1;
                err = buildCommand(line, 1);
                if(err)
                {
                    isError = TRUE;
                    asLineNum++;
                    continue;
                }
            }
            LC++;
            asLineNum++;
        }

        /*first word is command*/
        else if(isOperation(line[0]))
        {
            int err;
            zeroCommand(IC);
            commandArray[IC].address = IC;
            err = buildCommand(line, 0);
            if(err)
            {
                isError = TRUE;
                asLineNum++;
                continue;
            }
            asLineNum++;
        }

        /*first word is .data*/
        else if(!strcmp(line[0], ".data"))
        {
            int err;
            fprintf(stderr, "Warning in line %d: missing label, possible loss of data\n", asLineNum);
            if(numOfWords < 2)
            {
                fprintf(stderr, "Warning in line %d: empty .data entry\n", asLineNum++);
                continue;
            }
            err = addDataToData(line, 1);
            if(err)
            {
                isError = TRUE;
                asLineNum++;
                continue;
            }
            asLineNum++;
        }

        /*first word is .string*/
        else if(!strcmp(line[0], ".string"))
        {
            fprintf(stderr, "Warning in line %d: missing label, possible loss of string data\n", asLineNum);
            if(numOfWords < 2)
            {
                fprintf(stderr, "Warning in line %d: empty .string entry\n", asLineNum++);
                continue;
            }
            if(line[1][0] != 34 || line[1][strlen(line[1])-1] != 34)
            {
                isError = TRUE;
                fprintf(stderr, "Error in line %d: data is not string type\n", asLineNum++);
                continue;
            }
            addStringToData(line, 1);
            asLineNum++;
        }

        /*first word is .extern*/
        else if(!strcmp(line[0], ".extern"))
        {

            if(numOfWords < 2)
            {
                fprintf(stderr, "Warning in line %d: empty .extern label", asLineNum++);
                isError = TRUE;
                continue;
            }
            if(!checkLabel(line[1]))
            {
                isError = TRUE;
                fprintf(stderr, "Error in line %d: %s is not legal label\n", asLineNum++, line[1]);
                continue;
            }
            if(!labelPresents(line[1]))
            {    
                zeroLabel(LC);
                strcpy(labelArray[LC].name, line[1]);
                labelArray[LC].isExt = 1;
                externSize++;
                LC++;
                asLineNum++;
            }
            asLineNum++;
        }

        /*first word is .entry*/
        else if(!strcmp(line[0], ".entry"))
        {
            if(numOfWords < 2)
            {
                fprintf(stderr, "Warning in line %d: empty .entry label", asLineNum++);
                continue;
            }
            if(!checkLabel(line[1]))
            {
                isError = TRUE;
                fprintf(stderr, "Error in line %d: %s is not legal label\n", asLineNum++, line[1]);
                continue;
            }
            zeroLabel(LC);
                strcpy(labelArray[LC].name, line[1]);
                labelArray[LC].isEntry = 1;
                entrySize++;
                LC++;        
            if(numOfWords > 2)
            {
                fprintf(stderr, "Warning in line %d: too many arguments", asLineNum);
            }
            asLineNum++;
        }
    }

}

void secondPass()
{
    int i, labIndex;
    for(i = IC_START_VALUE; i < IC; i++)
    {
        if(strlen(commandArray[i].label) != 0)
        {
            if(commandArray[i].label[0] != '&')
            {
                if(!labelPresents(commandArray[i].label))
                {
                    isError = 1;
                    fprintf(stderr, "Argument error: label %s must be initiated before use\n", commandArray[i].label);
                    continue;
                }
                labIndex = getLabelNum(commandArray[i].label);
                convertUnsignedToBinary(commandArray[i].binary, labelArray[labIndex].address, 3, 23);
                if(labelArray[labIndex].isExt == 1) 
                    convertUnsignedToBinary(commandArray[i].binary, 1, 0, 2);
                else
                    convertUnsignedToBinary(commandArray[i].binary, 2, 0, 2);
            }
            else
            {
                if(!labelPresents(&commandArray[i].label[1]))
                {
                    isError = 1;
                    fprintf(stderr, "Argument error: label %s must be initiated before use\n", &commandArray[i].label[1]);
                    continue;
                }
                labIndex = getLabelNum(&commandArray[i].label[1]);
                convertIntToBinary(commandArray[i].binary, labelArray[labIndex].address - i + 1, 1);
                convertUnsignedToBinary(commandArray[i].binary, 4, 0, 2);
            }
            
        }
    }
}

void createObFile(FILE *file)
{
    int i;
    fprintf(file, "%7d %d\n", ICfinal - IC_START_VALUE, DC);

    for(i = 100; i < IC; i++)
    {
        fprintf(file, "%07d %07x\n", commandArray[i].address, convertBinaryToInt(commandArray[i].binary));
    }
}

void createEntFile(FILE *file)
{
    int i;
    for(i = 0; i <= LC; i++)
    {
        if(labelArray[i].isEntry == 1)
        {
            fprintf(file, "%s %07d\n", labelArray[i].name, labelArray[i].address);
        }
    }
}

void createExtFile(FILE * file)
{
    int i, j;
    for(i = 0; i <= LC; i++)
    {
        if(labelArray[i].isExt == 1)
        {
            for(j = 100; j < IC; j++)
            {
                if(!strcmp(labelArray[i].name, commandArray[j].label))
                {
                    fprintf(file, "%s %07d\n", labelArray[i].name, commandArray[j].address);
                }
            }
        }
    }
}