#include <stdio.h>
#include <termios.h>
#include <stdlib.h>
#include <time.h>
#include <sys/timeb.h>
#include <unistd.h>

static struct termios old, new;


//#define DEBUG

#if defined(DEBUG)
	#define dbg printf
#else
void dbg(char *format, ...)
{
	
}
#endif

struct timeb timeSeed;
int get_random_digits(int mod)
{
	ftime(&timeSeed);
	//srand((unsigned)time(NULL));
	srand(timeSeed.time * 1000 + timeSeed.millitm);
	usleep(10000);
	return rand()%mod;
}

enum operation {ERR, ADD, SUB, MUX, DIV};
static char op_sym[5] = {'_','+','-','*','/'};
static struct question {
	int var1;
	int var2;
	enum operation op;
	int result;
}ques;

int double_digit_generator(void)
{
	printf("%d\n",  get_random_digits(100));
}

void generate_addition_question(struct question *q, int mod)
{
	while (1) {
		q->var1 = get_random_digits(mod);
		q->var2 = get_random_digits(mod);
		q->op=ADD;

		/* restrict result no more than max num */
		if (q->var1 + q->var2 < mod)
			break;
	}
	q->result = q->var1 + q->var2;
	dbg("\n%s(): %d %c %d = %d\n", __func__, q->var1, op_sym[ques.op],
		q->var2, q->result);
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
	generate_addition_question(&ques, 1000);

again:
	/* print question */
	printf("%d %c %d =____", ques.var1, op_sym[ques.op] ,ques.var2);

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
	dbg("\n%s(): input is: %d\n", __func__, input);

	/* judge */
	if (input == ques.result) {
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
	return 0;
}
