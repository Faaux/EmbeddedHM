#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <pthread.h>
#include <errno.h>


void* thread_work(void* time_to_sleep) {
	pthread_t thread_id = pthread_self();

	sleep((uint32_t) time_to_sleep);
	printf("Hello I'm a thread with the id %d!\n" , thread_id);

	return (void*) thread_id; // Behaves the same as pthread_exit()
}



int main(int argc, char *argv[]) {
	pthread_t threadId1;
	pthread_t threadId2;

	if(EOK != pthread_create(&threadId1, NULL, thread_work, (void*)1)) {
		printf("Error on first thread creation");
		return EXIT_FAILURE;
	}

	if(EOK != pthread_create(&threadId2, NULL, thread_work, (void*)2)) {
		printf("Error on second thread creation");
		return EXIT_FAILURE;
	}

	void *result1;
	void *result2;

	if(EOK != pthread_join(threadId1, &result1)) {
		printf("Thread joining failed");
		return EXIT_FAILURE;
	}
	printf("Thread 1 Ids %d, %d\n", threadId1, (int)result1);

	if(EOK != pthread_join(threadId2, &result2)) {
		printf("Thread joining failed");
		return EXIT_FAILURE;
	}
	printf("Thread 2 Ids %d, %d\n", threadId2, (int)result2);

	printf("EXIT PROGRAM\n");

	return EXIT_SUCCESS;
}
