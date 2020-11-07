/************  PL0.c  *************/

// pl0 compiler source code

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "pl0.h"

//////////////////////////////////////////////////////////////////////
// print error message.
void error(n)
{
	int i;

	printf("      ");
	for (i = 1; i <= cc - 1; i++)
		printf(" ");
	printf("^\n");
	printf("Error %3d: %s\n", n, err_msg[n]);
	err++;
} // error

//////////////////////////////////////////////////////////////////////
void getch(void)
{
	if (cc == ll)
	{
		if (feof(infile))
		{
			printf("\nPROGRAM INCOMPLETE\n");  
			exit(1);
		}
		ll = cc = 0;
		printf("\n");
		printf("%5d  ", cx);
		while (!feof(infile) && (ch = getc(infile))!='\n')
		{
			printf("%c", ch);
			/* TO BE MODIFIED */
			line[++ll] = ch;
		} // while
		printf("\n");
		line[++ll] = ' ';
	}
	ch = line[++cc];
} // getch

//////////////////////////////////////////////////////////////////////
// gets a symbol from input stream.
void getsym(void)
{
	int i, k;
	char a[MAXIDLEN + 1];

	while (ch == ' ' || ch=='\t')
		getch();

	if (isalpha(ch))
	{ // symbol is a reserved word or an identifier.
		k = 0;
		do
		{
			if (k < MAXIDLEN)
				a[k++] = ch;
			getch();
		}
		while (isalpha(ch) || isdigit(ch));
		a[k] = 0;
		strcpy_s(id, 11, a);  //strcpy 改为了 strcpy_s，增加了参数 11(数组长度)
		word[0] = id;
		i = NRW;
		while (strcmp(id, word[i--]));
		if (++i)
			sym = wsym[i]; // symbol is a reserved word
		else
			sym = SYM_IDENTIFIER;   // symbol is an identifier
		
	}
	else if (isdigit(ch))
	{ // symbol is a number.
		k = num = 0;
		sym = SYM_NUMBER;
		do
		{
			num = num * 10 + ch - '0';
			k++;
			getch();
		}
		while (isdigit(ch));
		if (k > MAXNUMLEN)
			error(25);     // The number is too great.
	}
	else if (ch == ':')
	{
		getch();
		if (ch == '=')
		{
			sym = SYM_BECOMES; // :=
			getch();	
		}
		else
		{
			sym = SYM_NULL;       // illegal?
		}
	}
	else if (ch == '>')
	{
		getch();
		if (ch == '=')
		{
			sym = SYM_GEQ;     // >=
			getch();
		}
		else
		{
			sym = SYM_GTR;     // >
		}
	}
	else if (ch == '<')
	{
		/* TO BE MODIFIED */
		getch();
		if (ch == '=')  // <=
		{
			sym = SYM_LEQ;
			getch();
		}
		else if (ch == '>') //<>
		{
			sym = SYM_NEQ;
			getch();
		}
		else
		{
			sym = SYM_LES;  //<
		}
	}
	
	/* TO BE MODIFIED */
	/* Skip  Notes*/
	else if (ch == '(' && line[cc+1] == '*' && comment_flag == 0)  //（*
	{
		comment_flag = 1;
		getch();
		getch();
		int flag = (ch == '*' && line[cc + 1] == ')');
		while (!flag)      //try finding  *)
		{
			getch();
			flag = (ch == '*' && line[cc + 1] == ')');
			if (ch == '(' && line[cc + 1] == '*' && comment_flag == 1)  //Comment nesting
			{
				error(26);
				exit(1);
			}
		}
		comment_flag = 0;   //find *)
		getch();
		getch();
	}
	else if (ch == '*' && line[cc + 1] == ')' && comment_flag == 0)  //Left comment(  (*  ) missing
	{
		error(27);
		exit(1);
	}
	else
	{ // other tokens
		i = NSYM;
		csym[0] = ch;
		while (csym[i--] != ch);
		if (++i)
		{
			sym = ssym[i];
			if(sym != 18)
				getch();
		}
		else
		{
			printf("Fatal Error: Unknown character.\n");
			exit(1);
		}
	}
} // getsym

//////////////////////////////////////////////////////////////////////

void main ()
{
	char s[80];
	printf("Please input source file name: "); // get file name to be compiled
	scanf_s("%s", s, 80); //scanf改为了scanf_s,增加参数 80

	errno_t err;
	if ((err = fopen_s(&infile, s, "r")) != 0) //fopen改为了fopen_s,增加了参数 &infile与变量err，同时修改判断条件
	{
		printf("File %s can't be opened.\n", s);
		exit(1);
	}
	
	err = cc = cx = ll = 0; // initialize global variables
	ch = ' ';
	kk = MAXIDLEN;

	while(ch != '.')
	{   
		getsym();  
		
		/* TO BE MODIFIED */
	    /* Please output types of words in every line */  
		if (sym != 0)
		{
			printf("%d  ", sym);
			if (sym != 18)
				sym = 0;
		}
    }

	if (sym != SYM_PERIOD)
		error(9); // '.' expected.

    /* TO BE MODIFIED */
    /* Please output the number of “:=”，“>”，“<”，“<>”  to file "test.out"*/
	fclose(infile);
	if ((err = fopen_s(&infile, s, "r")) != 0)   //重新打开文件
	{
		printf("File %s can't be opened.\n", s);
		exit(1);
	}
	int number1, number2, number3, number4;
	number1 = number2 = number3 = number4 = 0;
	char tmp;
	while ((tmp = getc(infile)) != EOF)   //从头扫描
	{
		if (tmp == '(')      //判断是否为注释中的内容
		{
			tmp = getc(infile);
			if (tmp == '*')  //是注释中的内容，找注释的终点
			{
				int flag = 0;
				do
				{
					tmp = getc(infile);
					if (tmp == '*')
					{
						tmp = getc(infile);
						if (tmp == ')')
							flag = 1;
					}
				} while (!flag);
			}
			else if (tmp == ':' || tmp == '>' || tmp == '<')
			{
				goto count_character;  //如果不是注释的内容，转到计数部分
			}
			else
			{
				continue;
			}
		}
		count_character:   //计数部分
			if (tmp == ':')
			{
				tmp = getc(infile);
				if (tmp == '=')
					number1++;
			}	
			else if (tmp == '>')
			{
				tmp = getc(infile);
				if (tmp != '=')
					number2++;
			}
			else if(tmp == '<')
			{
				tmp = getc(infile);
				if (tmp == '>')
				{
					number4++;
				}
				else if (tmp == '=')
				{}
				else
				{
					number3++;
				}
			}
	}
	


	fclose(infile);

	FILE* testout;    
	char name[80];    //得到文件名
	for (int i = 0; i < 75; i++)
	{
		if (s[i] == '.')
		{
			name[i] = '.';
			name[i + 1] = 'o';
			name[i + 2] = 'u';
			name[i + 3] = 't';
			name[i + 4] = '\0';
			break;
		}
		else
		{
			name[i] = s[i];
		}

	}
	if ((err = fopen_s(&testout , name, "w+")) != 0)  //打开文件，输出计数内容
	{
		printf("File %s can't be opened.\n", name);
		exit(1);
	}
	fprintf(testout, "%s :", ":=");
	fprintf(testout, "  %d\n", number1);
	fprintf(testout, "%s :", ">");
	fprintf(testout, "  %d\n", number2);
	fprintf(testout, "%s :", "<");
	fprintf(testout, "  %d\n", number3);
	fprintf(testout, "%s :", "<>");
	fprintf(testout, "  %d\n", number4);

	fclose(testout);
} // main    END OF PL0.c
