#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <pthread.h>
#include <semaphore.h>
#include <iostream>

using namespace std;

struct thread_msg {
	int tid;
	string message;
};
sem_t bin_sem;
void *thread_function1(void *arg)
{
	thread_msg* data = (thread_msg*)arg;
	cout << data->tid << endl;
	//printf("%s\n",data->message);
	cout << data->message << endl;
	sem_post(&bin_sem);
	pthread_exit(NULL);
}

void *thread_function2(void *arg)
{
	thread_msg* data = (thread_msg*) arg;
	sem_wait(&bin_sem);
	cout << data->tid << endl;
	cout << data->message << endl;
	pthread_exit(NULL);

}


int main()
{
	int res;
	pthread_t a_thread,b_thread;
	
	thread_msg data[2];
	// data[0] = {0,"hello"};
	data[0].tid = 0;
	data[0].message = "hello!";
	data[1].tid = 1;
	data[1].message = "thread2!";
	res = sem_init(&bin_sem, 0, 0);
	if (res != 0)
	{
		perror("Semaphore initialization failed");
	}
	printf("sem_init\n");
	res = pthread_create(&a_thread, NULL, thread_function1, (void*)&data[0]);
	if (res != 0)
	{
		perror("Thread creation failure");
	}

	res = pthread_create(&b_thread, NULL, thread_function2, (void*)&data[1]);
	if (res != 0)
	{
		perror("Thread creation failure");
	}
	
		pthread_join(a_thread, NULL);
		pthread_join(b_thread, NULL);

	return 0;
}

