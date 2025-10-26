#include <stdio.h>
#include <setjmp.h>
#include <string.h>

jmp_buf envbuf;
int status;

int ERROR = 1;                  /* error code */

int char2int(char ch)
{
    if (isdigit(ch))
        return ch - '0';
    else
        longjmp(envbuf, ERROR);
}

void main()
{
    printf("Convert an character to an integer.\n");
    printf("please enter a digit:\n> ");
    char ch;
    while ((ch = getchar()) != EOF) {
        status = setjmp(envbuf);
        if (status == 0) {
            printf("the digit is: %d\n> ", char2int(ch));
        } else {
            /* handling exception */
            printf("error code: %d, not a digit!\n", status);
            printf("please enter a digit!\n> ");
        }
        /* consuming residual characters including '\n' */
        while (getchar() != '\n') ;
    }
}
