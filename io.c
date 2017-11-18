#include <stdio.h>
#include <termios.h>

void disable_io_buffer(void)
{
    struct termios now;

	tcgetattr(0, &now);
	now.c_lflag &= ~ICANON;
	now.c_lflag &= ~ECHO;
	tcsetattr(0, TCSANOW, &now);
}

void enable_io_buffer(void)
{
	//tcsetattr(0, TCSANOW, &old);
    struct termios now;

	tcgetattr(0, &now);
	now.c_lflag |= ICANON;
	now.c_lflag |= ECHO;
	tcsetattr(0, TCSANOW, &now);
}
