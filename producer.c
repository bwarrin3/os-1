#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>



struct region {        /* Defines shared memory */
    int count;
    char buffer[2];
    sem_t semFull;
    sem_t semEmpty;
    pthread_mutexattr_t mutexAttr;
    pthread_mutex_t mutexBuffer; 
};
struct region *rptr;
int fd;




void* producer(void* args) {
    while (1) {
        int x = 1;
	    sleep(rand() % 5 + 1);

        // Add to the buffer
        sem_wait(&rptr->semEmpty);
        pthread_mutex_lock(&rptr->mutexBuffer);
        rptr->buffer[rptr->count] = x;
        rptr->count++;
	printf("Current Array After Producing: ");
        for(int i = 0; i < 2; i++) {
                printf("%d ", rptr->buffer[i]);
        }
	printf("\n");
        pthread_mutex_unlock(&rptr->mutexBuffer);
        sem_post(&rptr->semFull);

    }
}



int main(int argc, char* argv[]) {
    pthread_t thread;

    //create and connect to shared memory
    fd = shm_open("/myregion", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1)
        printf("SHM OPEN FAILED\n");
    if (ftruncate(fd, sizeof(struct region)) == -1)
        printf("TRUNCATE FAILED\n");
    rptr = mmap(NULL, sizeof(struct region), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (rptr == MAP_FAILED)
        printf("MAP_FAILED\n");

    //set initial values of shared memory
    rptr->count = 0;
    rptr->buffer[0] = 0;
    rptr->buffer[1] = 0;

    sem_init(&rptr->semEmpty, 1, 2);
    sem_init(&rptr->semFull, 1, 0);
    pthread_mutexattr_setpshared(&rptr->mutexAttr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&rptr->mutexBuffer, &rptr->mutexAttr);


    srand(time(NULL));


    //create threads
    if (pthread_create(&thread, NULL, &producer, NULL) != 0) {
        perror("Failed to create thread");
    }
    if (pthread_join(thread, NULL) != 0) {
        perror("Failed to join thread");
    }
    

    //destroy semaphores and mutex lock
    sem_destroy(&rptr->semEmpty);
    sem_destroy(&rptr->semFull);
    pthread_mutex_destroy(&rptr->mutexBuffer);
    return 0;
}


