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
	srand(timeSeed.time * 1000 + timeSeed.millitm);
	usleep(10000);
	return rand()%mod;
}

enum operation {ADD, SUB, MUX, DIV, ERR};
static char op_sym[5] = {'+','-','*','/', '_'};
static struct question {
	int var1;
	int var2;
	enum operation op;
	int correct_answer;
	int user_input;
}ques;

int double_digit_generator(void)
{
	printf("%d\n",  get_random_digits(100));
    return 0;
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
	q->correct_answer = q->var1 + q->var2;
	dbg("\n%s(): %d %c %d = %d\n", __func__, q->var1, op_sym[ques.op],
		q->var2, q->correct_answer);
}

enum test_mode {EXERCISE, EXAM};
enum judge_result {PASS, FAIL};

struct test_result {
	struct question ques;
	int user_input;
	int judge_result;
};

int test(int mode, struct test_result *tr)
{
	char ch, input_string[10];
	int input, i, size;
	input = -1;
	ch = 0;
	int a1, a2, a3;
	int m1, m2, m3;
	int result = -1;
	int ret = 0;

	/* Change io buffer property to react for each cahr input */
	tcgetattr(0, &old);
	new = old;
	new.c_lflag &= ~ICANON;
	new.c_lflag &= ~ECHO;

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
		if (ch == 127) {
			printf("_");
			if (size) {
				//printf("\nsize:%d\n", size);
				for(i=0;i<size;i++) {
					//printf("i:%d\n", i);
					input_string[i] = input_string[i+1];
				}
				size--;
			}
			continue;
		}
		dbg("\n:%d\n", ch);
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
	if (input == ques.correct_answer) {
		dbg("\nResult: Correct!\n");
	} else {
		if (mode == EXERCISE) {
			printf("\nResult: Wrong! Do it again...\n\n");
			goto again;
		} else if (mode == EXAM) { /* record result */
			if (tr != NULL) {
				tr->ques.var1 = ques.var1;
				tr->ques.var2 = ques.var2;
				tr->ques.op = ques.op;
				tr->ques.correct_answer = ques.correct_answer;
				tr->user_input = input;
				tr->judge_result = FAIL;
			}
		}
		ret = -1;
	}


	printf("\n");
	return ret;
}

int main(void)
{
	struct test_result test_result_10[10], *ptr;
	int count, ret;
	char input[100];
	int user_choice;

	ptr = test_result_10;

	while(1) {
		printf("EXERCISE(x) or EXAM(m)?\n");
		scanf("%s", input);
		if (input[0] == 'x') {
			user_choice = EXERCISE; 
			getchar();
			break;
		}
		if (input[0] == 'm') {
			user_choice = EXAM; 
			getchar();
			break;
		}
		printf("invalid choice\n");
	}

	printf("Let's begin...\n\n");
	for (count = 0; count < 4; count++) {
		ret = test(user_choice, ptr);
		if ((ret < 0) && (user_choice == EXAM)) {
			printf("%d %c %d = %d: %d FAIL \n", ptr->ques.var1,
                    op_sym[ptr->ques.op] ,ptr->ques.var2,
                    ptr->user_input, ptr->ques.correct_answer);
			ptr++;
		}
	}

	return 0;
}
