#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <math.h>
#include <time.h>
#include <sys/neutrino.h>
#include <pthread.h>
#include <sched.h>

#define BILLION 1000000000L
#define ITERATIONS 90

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

}

float waste_scaling = 1.f;

void waste_msec_internal() {
	volatile int first = 0, second = 1, next, c;
	for (c = 0; c < (int) (200000 * waste_scaling); c++) {
		if (c <= 1) {
			next = c;
		} else {
			next = first + second;
			first = second;
			second = next;
		}
	}
}

void waste_msec(unsigned int msecs) {

	int i;
	for (i = 0; i < msecs; i++)
		waste_msec_internal();
}

void calibrate_waste_scaling() {
	struct timespec current;
	struct timespec after;
	waste_scaling = 1.f;

	if (-1 == clock_gettime(CLOCK_REALTIME, &current)) {
		perror("clock_gettime");
	}
	
	waste_msec(ITERATIONS);

	if (-1 == clock_gettime(CLOCK_REALTIME, &after)) {
		perror("clock_gettime");
	}

	float time_slept_ms = 0;
	time_slept_ms += (after.tv_sec - current.tv_sec) * 1000.f;
	time_slept_ms += (after.tv_nsec - current.tv_nsec) / (1000.f * 1000.f);
	time_slept_ms /= ITERATIONS;
	if (fabsf(time_slept_ms) < 0.01f) {
		printf("calibration failed\n");
		return;
	}
	waste_scaling = 1.f / time_slept_ms;
	printf("Time Slept ms: %f\n", time_slept_ms);
	printf("Waste_scaling: %f\n", waste_scaling);
}

void do_waste_stess_test() {
	unsigned int time_to_sleep_ms = 10;

	struct timespec current;
	struct timespec after;
	if (-1 == clock_gettime(CLOCK_REALTIME, &current)) {
		perror("clock_gettime");
	}

	int i;
	for (i = 0; i < ITERATIONS; i++) {
		waste_msec(time_to_sleep_ms);
	}

	if (-1 == clock_gettime(CLOCK_REALTIME, &after)) {
		perror("clock_gettime");
	}

	float time_slept_ms = 0;
	time_slept_ms += (after.tv_sec - current.tv_sec) * 1000.f;
	time_slept_ms += (after.tv_nsec - current.tv_nsec) / (1000.f * 1000.f);

	printf("Time slept: %f (ms)\n", time_slept_ms);
	printf("Time error absolute: %f (ms)\n",
			fabsf(ITERATIONS * time_to_sleep_ms - time_slept_ms));
}

int get_thread_priority() {
	struct sched_param sched_param;

	if (EOK != pthread_getschedparam(pthread_self(), NULL, &sched_param)) {
		perror("pthread_getschedparam()");
	}

	return sched_param.sched_priority;
}

void* worker_thread(void* ignored) {

	printf("Thread priority: %i\n", get_thread_priority());

	calibrate_waste_scaling();
	do_waste_stess_test();

	return 0;
}

int main(int argc, char *argv[]) {
	changeSystemTick(1000);

	printf("process priority: %i\n", get_thread_priority());
	calibrate_waste_scaling();
	do_waste_stess_test();

	pthread_t worker;
	pthread_attr_t worker_attr;

	struct sched_param worker_sched_param;
	worker_sched_param.sched_priority = sched_get_priority_max(SCHED_RR);

	pthread_attr_init(&worker_attr);
	pthread_attr_setinheritsched(&worker_attr, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedparam(&worker_attr, &worker_sched_param);

	if (EOK != pthread_create(&worker, &worker_attr, &worker_thread, NULL)) {
		perror("Error during thread creation");
	}

	pthread_join(worker, NULL);

	return EXIT_SUCCESS;
}
