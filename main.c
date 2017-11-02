#include <stdio.h>
#include <termios.h>
#include <stdlib.h>

static struct termios old, new;

int main(void)
{
	char ch, input_string[10];
	int input, i, size;
	input = -1;
	ch = 0;
	printf("Hello world..\n");
	tcgetattr(0, &old);
	new = old;
	new.c_lflag &= ~ICANON;
	new.c_lflag &= ~ECHO;
	tcsetattr(0, TCSANOW, &new);

	printf("1+1=___");

	size = 0;
	input_string[size] = '\0';
	while((ch = getchar()) != '\n') {
		size++;
		printf("\b%c\b",ch);
		for(i=size;i>0;i--) {
			input_string[i] = input_string[i-1];
		}
		input_string[0] = ch;
	}

	input = atoi(input_string);
	printf("\ninput is: %d\n", input);

	tcsetattr(0, TCSANOW, &old);


	return 0;
}
