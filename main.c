#include <stdio.h>
#include <termios.h>
#include <stdlib.h>
#include <time.h>
#include <sys/timeb.h>
#include <unistd.h>
#include "io.h"
#include "number.h"

#define QUESTION_NUM    8
//#define RIGHT_INPUT_FIRST


#if 0
#define DEBUG
#endif

#if defined(DEBUG)
	#define dbg printf
#else
void dbg(char *format, ...)
{
	
}
#endif

static struct timeb timeSeed;
char get_input()
{
	ftime(&timeSeed);
    enqueue_seed->seed = timeSeed.time * 1000 + timeSeed.millitm;
    enqueue_seed = enqueue_seed->next;
    return getchar();
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
    void (*collect_input)(struct question *THIS);
}ques;

int double_digit_generator(void)
{
	printf("%d\n",  get_random_digits(100));
    return 0;
}

void general_collect_input(struct question *q)
{
	char ch, input_string[10];
	int input, i, size;

	input = -1;
	ch = 0;
    i = 0;
    disable_io_buffer();/* Change io buffer property to react for each cahr input */
	size = 0;
	input_string[size] = '\0';
	while ((ch = get_input()) != '\n') {
#ifdef RIGHT_INPUT_FIRST
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
#else
        input_string[i++] = ch;
        printf("%c", ch);
#endif
	}

    enable_io_buffer();
	input = atoi(input_string);;
	dbg("\n%s(): input is: %d\n", __func__, input);
    q->user_input = input;
}
void print_2_elements_question(struct question *q)
{
	dbg("%s() is called\n", __func__);
#ifdef RIGHT_INPUT_FIRST  
	printf("%d %c %d =____", q->var1, op_sym[q->op],
		   q->var2);
#else
	printf("%d %c %d = ", q->var1, op_sym[q->op],
		   q->var2);
#endif
}

void generate_mux_question(struct question *q, int mod)
{
    while(1) {
        q->var1 = get_random_digits(mod);
        if (q->var1 % 10 != 0)
            break;
    }
    q->var2 = get_random_digits(10);
    q->op = MUX;
    dbg("op:%d\n", q->op);

    /* restrict result is positive */
    q->correct_answer = q->var1 * q->var2;

	dbg("\n%s(): %d %c %d = %d\n",
        __func__, q->var1, op_sym[ques.op],
		q->var2, q->correct_answer);

	q->print_question = print_2_elements_question;
	q->collect_input = general_collect_input;
}

void generate_subtraction_question(struct question *q, int mod)
{
    int tmp;

    q->var1 = get_random_digits(mod);
    q->var2 = get_random_digits(mod);
    q->op=SUB;

    /* restrict result is positive */
    if (q->var1 > q->var2)
        q->correct_answer = q->var1 - q->var2;
    else {
        q->correct_answer = q->var2 - q->var1;
        tmp = q->var1;
        q->var1 = q->var2;
        q->var2 = tmp;
    }

	dbg("\n%s(): %d %c %d = %d\n",
        __func__, q->var1, op_sym[ques.op],
		q->var2, q->correct_answer);

	q->print_question = print_2_elements_question;
	q->collect_input = general_collect_input;
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
	q->collect_input = general_collect_input;
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
	int ret = 0;

    /* generate question */
    if (math == ADD)
        generate_addition_question(&ques, 1000);
    else if (math == SUB)
        generate_subtraction_question(&ques, 1000);
    else if (math == MUX)
        generate_mux_question(&ques, 100);
    else
        goto error;

again:
    /* print question */
	ques.print_question(&ques);

    /* collect user input */
    ques.collect_input(&ques);

	/* judge */
	if (ques.user_input == ques.correct_answer) {
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
                tr->user_input = ques.user_input;
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
    struct random_seed *p;
	int count, ret;
	char ch;
	int user_choice;
    int math_mode;

	ptr = test_result_10;

    initialize_random_seed_poll();
    disable_io_buffer();

    /* choose math mode */
	while(1) {
		printf("Do ADD(a) or SUB(s) or MUX(m)?\n");
		ch = get_input();
		if (ch == 'a') {
			math_mode = ADD;
			break;
		}
		if (ch == 's') {
			math_mode = SUB;
			break;
		}
		if (ch == 'm') {
			math_mode = MUX;
			break;
		}
		printf("invalid choice\n");
	}

    /* choose test mode */
	while(1) {
		printf("EXERCISE(x) or EXAM(m)?\n");
		ch = get_input();
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
    //print_random_seed_content();

	for (count = 0; count < QUESTION_NUM; count++) {
		ret = test(user_choice, ptr, math_mode);
		if ((ret == -1) && (user_choice == EXAM)) {
			printf("%d %c %d = %d: %d FAIL \n\n", ptr->ques.var1,
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
    //print_random_seed_content();

    /* Free memory for linked list items */
    struct random_seed *next;
    dbg("\nFree memory of linked list:\n");
    p = first_seed.next;
    while(p != &first_seed) {
        dbg("free p: %p\n", p);
        next = p->next;
        free(p);
        p = next;
        dbg("-------------\n");
    }
	return 0;
}
