#include "binaryOps.h"
#include "utilities.h"



void convertOperandToBinary(char* bin, int ARE, int func, int destR, int destAddr, int srcR, int srcAddr, int opcode)
{
    convertUnsignedToBinary(bin, ARE, 0, 2);
	convertUnsignedToBinary(bin, func, 3, 7);
    convertUnsignedToBinary(bin, destR, 8, 10);
	convertUnsignedToBinary(bin, destAddr, 11, 12);
	convertUnsignedToBinary(bin, srcR, 13, 15);
	convertUnsignedToBinary(bin, srcAddr, 16, 17);
	convertUnsignedToBinary(bin, opcode, 18, 23);
}

void convertIntToBinary(char *bin, int num, int shift3left)
{
    int i, numlen;
    int size = BINARY_LENGTH;
    int arr[BINARY_LENGTH];

    int sign;
	sign = (num >= 0) ? 1 : -1;

	if(num == 0) return;

    num = sign * num;

    for (i = 0; num > 0 && i < size; i++)
	{
		arr[i] = num % 2;
		num = num / 2;
	}
    numlen = i;

	for (i = 0; i < 24; i++)
	{
		if(i >= numlen) bin[i] = '0';
		else bin[i] = (arr[i]>0)?'1':'0';
	}

	if (sign < 0)
	{
		i = 0;
		while (i <= 23 && bin[i] != '1')
			i++;
		i++;
		while (i <= 23)
		{
			if (bin[i] == '1')
				bin[i] = '0';

			else if (bin[i] == '0')
				bin[i] = '1';
			i++;
		}
	}

    if(shift3left)
    {
        for(i = 20; i >=0; i--)
        {
            bin[i+3] = bin[i];
        }
    }

}

void convertUnsignedToBinary(char *bin, int num, int start, int end)
{
	int i;
	int arr[BINARY_LENGTH];
	int numlen;
	
	for (i = 0; num > 0 && i < 24; i++)
	{
		arr[i] = num % 2;
		num = num / 2;
	}

	numlen = i;

	for(i = start; i <= end; i++)
	{
		if(i - start >= numlen) bin[i] = '0';
		else bin[i] = (arr[i-start]>0)?'1':'0';
	}
}

void reverse(char * str)
{
	int i;
	char tmp;
	for(i = 0; i < 12; i++)
	{
		tmp = str[i];
		str[i] = str[23 - i];
		str[23-i] = tmp;
	}
}

int convertBinaryToInt(char * str) /* converts binary string to integer*/
{
	int num, i, pow;
	num = 0;
	pow = 1;
	for(i = 0; i < 24; i++)
	{
		num += (str[i] - '0')*pow;
		pow*=2;
	}
	return num;
}