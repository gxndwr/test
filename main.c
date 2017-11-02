#include <stdio.h>
#include <termios.h>
#include <stdlib.h>
#include <time.h>

static struct termios old, new;


//#define DEBUG

#if defined(DEBUG)
	#define dbg printf
#else
void dbg(char *format, ...)
{
	
}
#endif

int get_random_digits(int mod)
{
	srand((unsigned)time(NULL));
	return rand()%mod;
}

int double_digit_generator(void)
{
	printf("%d\n",  get_random_digits(100));
}

void test(void)
{
	char ch, input_string[10];
	int input, i, size;
	input = -1;
	ch = 0;
	int a1, a2, a3;
	int m1, m2, m3;
	int result = -1;

	/* Change io buffer property to react for each cahr input */
	tcgetattr(0, &old);
	new = old;
	new.c_lflag &= ~ICANON;
	new.c_lflag &= ~ECHO;

	printf("Test begin...\n\n");
	/* generate question */
	a1 = 234;
	a2 = 34;
	result = a1 + a2;
again:
	/* print question */
	printf("%d + %d =____", a1, a2);

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
	dbg("\ninput is: %d\n", input);

	/* judge */
	if (input == result) {
		printf("\nResult: Correct!\n");
	} else {
		printf("\nResult: Wrong! Do it again...\n\n");
		goto again;
	}

	
	/* print test result */


	printf("\n");
}

int main(void)
{
	test();
	//double_digit_generator();
	return 0;
}
