#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <sys/neutrino.h>
#define BILLION 1000000000L

void print_time(unsigned int time_waited_ns, unsigned int tick_size_ns) {
	printf("%u;%u\n", time_waited_ns, tick_size_ns);
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

	if (-1 == ClockPeriod(CLOCK_REALTIME, NULL, &period, 0)) {
		printf("Clock failed, err: %i\n", errno);
		printf("Current clock: %ld us\n", old_period.nsec / 1000);
		return;
	}
	printf("clock before set: %ld us\n", old_period.nsec / 1000);
	printf("clock after set: %ld us\n", period.nsec / 1000);

}

int main(int argc, char *argv[]) {
	const unsigned int start_tick_us = 1000;
	const unsigned int end_tick_us = start_tick_us * 100;
	changeSystemTick(1000); //min value for clockPeriod is 10us, otherwise Invalid Argument is thrown

	int i;
	struct timespec current;
	struct timespec last;
	if (-1 == clock_gettime(CLOCK_REALTIME, &current)) {
		perror("clock gettime");
		return EXIT_FAILURE;
	}
	last = current;
	for (i = start_tick_us; i < end_tick_us; i += 1000) {
		changeSystemTick(i);
		current.tv_nsec += 1000000; // 1ms
		if (current.tv_nsec >= BILLION) {
			current.tv_nsec -= BILLION;
			current.tv_sec += 1;
		}

		if (-1
				== clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &current,
						NULL)) {
			perror("clock nanosleep");
			return EXIT_FAILURE;
		}

		struct timespec after_sleep;
		if (-1 == clock_gettime(CLOCK_REALTIME, &after_sleep)) {
			perror("clock gettime");
			return EXIT_FAILURE;
		}

		unsigned int time_slept_ns = 0;
		time_slept_ns += (after_sleep.tv_sec - last.tv_sec) * BILLION;
		time_slept_ns += (after_sleep.tv_nsec - last.tv_nsec);

		print_time(time_slept_ns, i * 1000); // Assumption: takes less than 1ms
		current = after_sleep;
		last = current;
	}

	return EXIT_SUCCESS;
}
