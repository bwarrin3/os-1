# os-1

Brent Warring Assignment 1


Producer adds a 1 to shared memory array, consumer consumes the 1 and makes it a 0.


compile and run using
$ gcc producer.c -pthread -lrt -o producer
$ gcc consumer.c -pthread -lrt -o consumer
$ ./producer & ./consumer &

Included files
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


Online resources were a big help, biggest problem I had was with the shared memory semaphores and mutex lock. I first declared them as pointers in the shared memory struct so I would use sem_wait(rptr->semFull). This would not work however. The fix was to get rid of the pointer part, so just a normal semaphore, in shared memory and pass the address so sem_wait(&rptr->semFull). This then worked.

Project completed through vscode connecting to wasp through ssh so I saved files locally off that to my mac then uploaded to github.
