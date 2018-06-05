#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>
#define maxLength 30
#define maxLimit 300

//  This is the Lexical Analyzer for Tiny Language.
//  Definition of Tiny :: 

//  Keywords			  		   :  	WRITE READ IF ELSE RETURN BEGIN END MAIN STRING INT REAL
//  Single-character separators    :   	;  ,  (   )
//  Single-character operators     :   	+  -  *   /
//  Multi-character operators      :   	:=  ==   !=
//  Identifier: An identifier consists of a letter followed by any number of letters or digits. The following are examples of identifiers: x, x2, xx2, x2x, End, END2.Note that End is an identifier while END is a keyword. The following are not identifiers:

/*    IF, WRITE, READ, .... (keywords are not counted as identifiers)
      2x (identifier can not start with a digit)
      Strings in comments are not identifiers. */

//  Number is a sequence of digits, or a sequence of digits followed by a dot, and followed by digits.   

/*    Number -> Digits | Digits '.' Digits
      Digits -> Digit | Digit Digits
      Digit  -> '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9' */

//  Comments: string between /** and **/. Comments can be longer than one line.


int lineNum = 1;
char *keywords[] = { "WRITE","READ","IF","ELSE","RETURN","BEGIN","END","MAIN","STRING","INT","REAL"};
char *multiCharOps[] = { ":=","==","!="};
char delimiter[] = {';',',','(',')'};
char multiCharFirst[] = {':','=','!'};
char operators[] = {'+','-','*','/'};
char words[maxLimit][maxLength];
int wordNum[maxLimit];
char keys[maxLimit][maxLength];
int keyNum[maxLimit];
char ids[maxLimit][maxLength];
int idNum[maxLimit];
char quotes[maxLimit][200];
int quoteNum[maxLimit];
char charNums[maxLimit][maxLength];
int numberNum[maxLimit];
//long number[maxLimit];


enum Type { KEYWORD,
			IDENTIFIER,
			STRING_LITERAL,
			NUMBER,
			DELIMITER,
			OPERATOR,
			UNDEF,
			EOT } TokenType;

struct Tokens {
	enum Type t;
	int lNum;
	char instance[100];
};
struct Tokens tokens[500];

int isOperator(char c)
{
	int i=0;
	int flag = 0; //false
	for(i=0;i<4;i++)
		if(c == operators[i]) {
			flag = 1;
			break;
		}
	if(!flag) {
		for(i=0;i<3;i++)
			if(c == multiCharFirst[i]) {
				flag = 2;
				break;
			} 
	}
	return flag;
}

int isDelimiter(char c)
{
	int i=0;
	int flag = 0; //false
	for(i=0;i<4;i++)
		if( c == delimiter[i] ) {
			flag = 1;
			break;
		}
	return flag;
}

int isKeyword(char *str) {
	int i;
	int result = 0; // false
	for (i = 0; i < 11; i++) {
		if (!strcasecmp(keywords[i], str))
			result = 1;
	}
	return result;
}

int main(int argc,char* argv[])
{   
    FILE *filePtr;
	int i,result=0;
	int wordi=0,wordj=0;
	int numi=0,numj=0;
	int quotei=0,quotej=0;
	int keyi=0,keyj=0;
	int idi=0,idj=0;
	int tokeni=0;
	int num=0;
	char c;
	char temp[maxLength];
    switch (argc) {
		case 1: // No parameters
    		printf("\nNo File Specified!\n");
			exit(0);
    	    break;

    	case 2: // One parameter, use .tiny file supplied	
           	if ( (filePtr = fopen(strcat(argv[1],""), "r")) == NULL ) {
            	printf("Cannot open input file. \nMake sure to specify '.tiny' extension with the filename. \n");
                exit(1);
            }
            break;

        default:
            printf("Syntax: scanner [file] (.tiny is implicit)\n");
            exit(0);
    }
	while((c = fgetc(filePtr)) != EOF) {
		if(c == '/') {
			c = fgetc(filePtr);
			if( c == '*') {
				c = fgetc(filePtr);
				if ( c == '*') {
					while((c = fgetc(filePtr)) != EOF) {
						if(c == '*') {
							if((c = fgetc(filePtr)) == '*') {
								if((c = fgetc(filePtr)) == '/') {
									c = fgetc(filePtr);
									break;
								}
								else if(c == '\n')
									lineNum++;
							}
							else if(c == '\n')
								lineNum++;
						}
						else if(c == '\n')
							lineNum++;
					}
				}
				else {
					fseek(filePtr,-3,SEEK_CUR);
					c = fgetc(filePtr);
				}
			}
			else {
				fseek(filePtr,-2,SEEK_CUR);
				c = fgetc(filePtr);
			}
		}
		if(c == '\n')
			lineNum++;
		else if(isOperator(c) == 1) {
			tokens[tokeni].t = OPERATOR;
			tokens[tokeni].lNum = lineNum;
			temp[0] = c;
			temp[1] = '\0';
			strcpy(tokens[tokeni].instance,temp);
			tokeni++;
		}
		else if(isOperator(c) == 2) {
			tokens[tokeni].t = OPERATOR;
			tokens[tokeni].lNum = lineNum;
			temp[0] = c;
			c = fgetc(filePtr);
			temp[1] = c;
			temp[2] = '\0';
			strcpy(tokens[tokeni].instance,temp);
			tokeni++;
		}
		else if(isDelimiter(c)) {
			tokens[tokeni].t = DELIMITER;
			tokens[tokeni].lNum = lineNum;
			temp[0] = c;
			temp[1] = '\0';
			strcpy(tokens[tokeni].instance,temp);
			tokeni++;
		}
		else if(c == '"') {
			quotes[quotei][quotej++] = c;
			c = fgetc(filePtr);
			while(c != '"' && c != EOF) {
				if(quotei<maxLimit && quotej<200)
					quotes[quotei][quotej++] = c;
				c = fgetc(filePtr);
			}
			quotes[quotei][quotej++] = c;
			if(quotei<maxLimit && quotej<200)
				quotes[quotei][quotej] = '\0';
			quoteNum[quotei] = lineNum;
			tokens[tokeni].t = STRING_LITERAL;
			tokens[tokeni].lNum = lineNum;
			strcpy(tokens[tokeni].instance, quotes[quotei]);
			tokeni++;
			quotei++;
			quotej=0;
		} 
		else if(isalpha(c)) {
			words[wordi][wordj++] = c;
			c = fgetc(filePtr);
			while(isalpha(c) || isdigit(c)) {
				if(wordi<maxLimit && wordj<maxLength)
					words[wordi][wordj++] = c;
				c = fgetc(filePtr);
			}
			if(c == EOF)
				break;
			fseek(filePtr,-1,SEEK_CUR);
			//ungetc(filePtr);
			if(wordi<maxLimit && wordj<maxLength)		
				words[wordi][wordj] = '\0';
			wordNum[wordi] = lineNum;
			strcpy(tokens[tokeni].instance , words[wordi]);
			tokens[tokeni].lNum = lineNum;
			if(isKeyword(words[wordi])){
				strcpy(keys[keyi],words[wordi]);
				keyNum[keyi] = wordNum[wordi];
				tokens[tokeni].t = KEYWORD;
				keyi++;
			}
			else {
				strcpy(ids[idi],words[wordi]);
				idNum[idi] = wordNum[wordi];
				tokens[tokeni].t = IDENTIFIER;
				idi++;
			}
			tokeni++;
			wordi++;
			wordj=0;
		}
		else if(isdigit(c)) {
			charNums[numi][numj++] = c;
			while(isdigit(c = fgetc(filePtr)))
				charNums[numi][numj++] = c;
			if(c == EOF)
				break;
			fseek(filePtr,-1,SEEK_CUR);
			charNums[numi][numj] = '\0';
			numberNum[numi] = lineNum;
			strcpy(tokens[tokeni].instance, charNums[numi]);
			tokens[tokeni].lNum = lineNum;
			tokens[tokeni].t = NUMBER;
			tokeni++;
			numi++;
			numj=0;
		}
		else if( c != ' ' && c != '\t') {
			tokens[tokeni].t = UNDEF;
			tokens[tokeni].lNum = lineNum;
			temp[0] = c;
			temp[1] = '\0';
			strcpy(tokens[tokeni].instance,temp);
			tokeni++;
		}
	}
	
	tokens[tokeni].t = EOT;
	
	printf("\nTokens Are:: \n\nTokenID\tLine #\tTokenType\tLexeme\n\n");
	num = tokeni;
	for(tokeni=0;tokeni<num;tokeni++) {
		printf("  %d\t %d\t",tokeni+1,tokens[tokeni].lNum);
		switch(tokens[tokeni].t) {
			case KEYWORD :
				printf("KEYWORD   ");
				break;
			case IDENTIFIER :
				printf("IDENTIFIER");
				break;
			case STRING_LITERAL :
				printf("STRING LITERAL");
				break;
			case NUMBER :
				printf("NUMBER   ");
				break;
			case DELIMITER :
				printf("DELIMITER");
				break;
			case OPERATOR :
				printf("OPERATOR");
				break;
			case UNDEF :
				printf("UNDEFINED");
				break;
			case EOT :
				
				break;
		}
		printf("\t");
		for(i=0; tokens[tokeni].instance[i] != '\0' ;i++)
			printf("%c",tokens[tokeni].instance[i]);
		printf("\n");
	}
	return 0;
}
