#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <sys/neutrino.h>

void print_time(const struct timespec* time) {
	printf("Current Time %d s, %ld ms\n", time->tv_sec, time->tv_nsec);
}

void changeSystemTick(unsigned int microsecs) {

	struct _clockperiod period;
	period.nsec = microsecs * 1000;
	period.fract = 0;

	struct _clockperiod old_period;

	if (-1 == ClockPeriod(CLOCK_REALTIME, &period, &old_period, 0)) {
		printf("Clock failed, err: %i\n", errno);
		printf("Current clock: %ld us\n", old_period.nsec / 1000);
		return;
	}
	printf("old clock: %ld us\n", old_period.nsec / 1000);
	printf("new clock: %ld us\n", period.nsec / 1000);

}

int main(int argc, char *argv[]) {
	changeSystemTick(10); //min value for clockPeriod is 10us, otherwise Invalid Argument is thrown

	int i;
	struct timespec current;
	if (-1 == clock_gettime(CLOCK_REALTIME, &current)) {
		perror("clock gettime");
		return EXIT_FAILURE;
	}

	print_time(&current);
	for (i = 0; i < 100; i++) {
		current.tv_nsec += 1000000; // 1ms

		if (-1 == clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &current, NULL)) {
			perror("clock nanosleep");
			return EXIT_FAILURE;
		}

		if (-1 == clock_gettime(CLOCK_REALTIME, &current)) {
			perror("clock gettime");
			return EXIT_FAILURE;
		}

		print_time(&current); // Assumption: takes less than 1ms
	}

	return EXIT_SUCCESS;
}
