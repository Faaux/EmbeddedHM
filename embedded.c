#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <math.h>
#include <time.h>
#include <signal.h>
#include <sys/siginfo.h>
#include <sys/neutrino.h>
#include <pthread.h>
#include <sched.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void toggle_led(int* led) {
	if (*led == 0)
		*led = 1;
	else
		*led = 0;
}

int main(int argc, char *argv[]) {
	int led0 = 0;
	int led1 = 0;

	int fid = open("/dev/leds", O_WRONLY);
	if (-1 == fid) {
		perror("Error during open /dev/leds");
		return EXIT_FAILURE;
	}

	printf("Enter '0' or '1' to toggle LED 0/1. Enter 'q' to quit.\n");
	int is_runnig = 1;
	while (is_runnig) {
		// Toggle LEDS
		unsigned char led_state = 0;

		int led2 = led0 ^ led1;
		int led3 = led0 | led1;

		led_state = (led3 << 3) | (led2 << 2) | (led1 << 1) | led0;

		write(fid, &led_state, 1);

		// Input

		while (1) {
			unsigned char input = getchar();
			if (input == '0') {
				toggle_led(&led0);
				break;
			} else if (input == '1') {
				toggle_led(&led1);
				break;
			} else if (input == 'q') {
				is_runnig = 0;
				break;
			}
		}
	}

	close(fid);

	return EXIT_SUCCESS;
}
