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




void* consumer(void* args) {
    while (1) {

        //remove from buffer
        sem_wait(&rptr->semFull); 
        pthread_mutex_lock(&rptr->mutexBuffer);
	    rptr->buffer[rptr->count - 1] = 0;
        rptr->count--;
	printf("Current Array After Consuming: ");
        for(int i = 0; i < 2; i++) {
                printf("%d ", rptr->buffer[i]);
        }
	printf("\n");
        pthread_mutex_unlock(&rptr->mutexBuffer);
        sem_post(&rptr->semEmpty);

        sleep(rand() % 5 + 1);
    }
}


int main(int argc, char* argv[]) {

    pthread_t thread;

    //create connection to shared memory, initial values set in producer
    fd = shm_open("/myregion", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1)
        printf("SHM OPEN FAILED\n");
    if (ftruncate(fd, sizeof(struct region)) == -1)
        printf("TRUNCATE FAILED\n");
    rptr = mmap(NULL, sizeof(struct region), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (rptr == MAP_FAILED)
        printf("MAP_FAILED\n");


    srand(time(NULL));


    //create thread
    if (pthread_create(&thread, NULL, &consumer, NULL) != 0) {
        perror("Failed to create thread");
    }
    if (pthread_join(thread, NULL) != 0) {
        perror("Failed to join thread");
    }
    

    return 0;
}