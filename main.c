#include <stdio.h>
#include <termios.h>
#include <stdlib.h>
#include <time.h>
#include <sys/timeb.h>
#include <unistd.h>

#define QUESTION_NUM    8
static struct termios old, new;

//#define DEBUG

#if defined(DEBUG)
	#define dbg printf
#else
void dbg(char *format, ...)
{
	
}
#endif

void initialize_buffer_property(void)
{
	tcgetattr(0, &old);
	new = old;
	new.c_lflag &= ~ICANON;
	new.c_lflag &= ~ECHO;
}

void disable_io_buffer()
{
	tcsetattr(0, TCSANOW, &new);
}

void enable_io_buffer()
{
	tcsetattr(0, TCSANOW, &old);
}

struct timeb timeSeed, last_question_timeb;
static int last_time_stamp;
int get_random_digits(int mod)
{
    int num;
	ftime(&timeSeed);
	srand(timeSeed.time * 1000 + timeSeed.millitm);
	usleep(10000);
    dbg("\n%d\n",timeSeed.millitm);
	//num = rand()%mod;
	//num = timeSeed.time * 1000 + timeSeed.millitm;
    //dbg("num: %d\n", num);
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
    void (*print_question)(struct question *THIS);
}ques;

int double_digit_generator(void)
{
	printf("%d\n",  get_random_digits(100));
    return 0;
}

void print_2_elements_question(struct question *q)
{
	dbg("%s() is called\n", __func__);
	printf("%d %c %d =____", q->var1, op_sym[q->op],
		   q->var2);
}

void generate_subtraction_question(struct question *q, int mod)
{

	while (1) {
        ftime(&timeSeed);
		//q->var1 = get_random_digits(mod);
		q->var1 = (timeSeed.millitm - last_question_timeb.millitm) % mod;
		q->var2 = get_random_digits(mod);
		q->op=SUB;

		/* restrict result no more than max num */
		if (q->var1 > q->var2)
			break;
	}
	q->correct_answer = q->var1 - q->var2;
	dbg("\n%s(): %d %c %d = %d\n",
        __func__, q->var1, op_sym[ques.op],
		q->var2, q->correct_answer);

	q->print_question = print_2_elements_question;
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
	dbg("\n%s(): %d %c %d = %d\n",
        __func__, q->var1, op_sym[ques.op],
		q->var2, q->correct_answer);

	q->print_question = print_2_elements_question;
}

/* record for jude result */
struct test_result {
	struct question ques;
	int user_input;
	int judge_result;
};

enum test_mode {EXERCISE, EXAM};
enum judge_result {PASS, FAIL};

/* main test function */
int test(int mode, struct test_result *tr, int math)
{
	char ch, input_string[10];
	int input, i, size;
	input = -1;
	ch = 0;
	int a1, a2, a3;
	int m1, m2, m3;
	int result = -1;
	int ret = 0;

    /* generate question */
    if (math == ADD)
        generate_addition_question(&ques, 1000);
    else if (math == SUB)
        generate_subtraction_question(&ques, 1000);
    else
        goto error;
again:
	/* print question */
	ques.print_question(&ques);
	//printf("%d %c %d =____", ques.var1, op_sym[ques.op],
	//	   ques.var2);

	/* Change io buffer property to react for each cahr input */
    disable_io_buffer();

	/* collect input */
	size = 0;
	input_string[size] = '\0';
	while ((ch = getchar()) != '\n') {
		if (ch == 127) {
			printf("_");
			if (size) {
				for(i=0;i<size;i++) {
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

    enable_io_buffer();

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

error:
    printf("%s(): mode invalid: %d!\n", __func__, math);
    return -2;
}

int main(void)
{
	struct test_result test_result_10[10], *ptr;
	int count, ret;
	char ch;
	int user_choice;
    int math_mode;

    /* prepare buffer settings */
    initialize_buffer_property();

	ftime(&last_question_timeb);

	ptr = test_result_10;

    disable_io_buffer();

    /* choose math mode */
	while(1) {
		printf("Do ADD(a) or SUB(s)?\n");
		ch = getchar();
		if (ch == 'a') {
			math_mode = ADD;
			break;
		}
		if (ch == 's') {
			math_mode = SUB;
			break;
		}
		printf("invalid choice\n");
	}

    /* choose test mode */
	while(1) {
		printf("EXERCISE(x) or EXAM(m)?\n");
		ch = getchar();
		if (ch == 'x') {
			user_choice = EXERCISE; 
			break;
		}
		if (ch == 'm') {
			user_choice = EXAM; 
			break;
		}
		printf("invalid choice\n");
	}

    enable_io_buffer();

	printf("Let's begin...\n\n");
	for (count = 0; count < QUESTION_NUM; count++) {
		ret = test(user_choice, ptr, math_mode);
		if ((ret == -1) && (user_choice == EXAM)) {
			printf("%d %c %d = %d: %d FAIL \n", ptr->ques.var1,
                    op_sym[ptr->ques.op] ,ptr->ques.var2,
                    ptr->user_input, ptr->ques.correct_answer);
			ptr++;
		}
        dbg("%s(): ret: %d\n", __func__,  ret);
        if (ret == -2) {
            printf("Fatal error: %d, return.\n", ret);
            return -1;
        }
	}

	return 0;
}
