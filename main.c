#include <stdio.h>
#include <termios.h>
#include <stdlib.h>

static struct termios old, new;

#define DEBUG

#if defined(DEBUG)
	#define dbg printf
#else
void dbg(char *format, ...)
{
	
}
#endif

int main(void)
{
	char ch, input_string[10];
	int input, i, size;
	input = -1;
	ch = 0;

	/* Change io buffer property to react for each cahr input */
	tcgetattr(0, &old);
	new = old;
	new.c_lflag &= ~ICANON;
	new.c_lflag &= ~ECHO;

	printf("Test begin...\n");

	/* print question */
	printf("1+1=___");

	/* collect input */
	tcsetattr(0, TCSANOW, &new);

	size = 0;
	input_string[size] = '\0';
	while ((ch = getchar()) != '\n') {
		size++;
		printf("\b%c\b",ch);
		for(i=size;i>0;i--) {
			input_string[i] = input_string[i-1];
		}
		input_string[0] = ch;
	}
	tcsetattr(0, TCSANOW, &old);

	input = atoi(input_string);;
	dbg("\niinput is: %d\n", input);

	/* judge */
	if (input == 2) {
		printf("\nResult: Correct!\n");
	} else {
		printf("\nResult: Wrong!\n");
	}

	
	/* print test result */


	printf("\n");
	return 0;
}
