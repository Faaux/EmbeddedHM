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

#define BILLION 1000000000L
#define ITERATIONS 90

sem_t p1_takt;
sem_t p2_takt;

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

void* pthread_1_worker(void* ignored) {

	int ticks = 0;
	while (1) {
		sem_wait(&p1_takt);
		printf("P1 TICK\n");
		waste_msec(2);
		if (ticks == 2) {
			sem_post(&p2_takt);
		}
		ticks = (ticks + 1) % 3;
	}

	return 0;
}

void* pthread_2_worker(void* ignored) {

	while (1) {
		sem_wait(&p2_takt);
		printf("P2 TICK\n");
		waste_msec(3);
	}
	return 0;
}

void signal_handler(_SIG_ARGS) {
	sem_post(&p1_takt);
}

int main(int argc, char *argv[]) {
	changeSystemTick(1000);

	calibrate_waste_scaling();

	//init semaphore
	sem_init(&p1_takt, 0, 0);
	sem_init(&p2_takt, 0, 0);

	pthread_t p1_worker;
	if (EOK != pthread_create(&p1_worker, NULL, &pthread_1_worker, NULL)) {
		perror("Error during thread creation");
	}

	pthread_t p2_worker;
	if (EOK != pthread_create(&p2_worker, NULL, &pthread_2_worker, NULL)) {
		perror("Error during thread creation");
	}

	// Which action on signal
	struct sigaction act;
	sigset_t set;
	sigemptyset(&set);

	act.sa_flags = 0;
	act.sa_mask = set;
	act.sa_handler = &signal_handler;

	// Set action on signal
	sigaction(SIGUSR1, &act, NULL);


	// Which event to use
	struct sigevent event;
	SIGEV_SIGNAL_INIT(&event, SIGUSR1);

	// Create timer on event
	timer_t timer;
	timer_create(CLOCK_REALTIME, &event, &timer);

	// Start timer in interval
	long timer_interval = 4000000;
	struct itimerspec itime;
	itime.it_value.tv_sec = 0;
	itime.it_value.tv_nsec = timer_interval;
	itime.it_interval.tv_sec = 0;
	itime.it_interval.tv_nsec = timer_interval;
	timer_settime(timer, 0, &itime, NULL);
	

	pthread_join(p1_worker, NULL);
	pthread_join(p2_worker, NULL);

	return EXIT_SUCCESS;
}
