#include <stdio.h>
#include <defs.h>
#include <mutex.h>
#include <barrier.h>

# ifndef NR_TASKLETS
  # define NR_TASKLETS 1
# endif

int shared_var = -1;
MUTEX_INIT(my_mutex);
BARRIER_INIT(my_barrier, NR_TASKLETS);
BARRIER_INIT(my_barrier2, NR_TASKLETS);

#define BUFFERSIZE 32
__mram int buffer[BUFFERSIZE];
int checksums[NR_TASKLETS] = {0};

int main(){
    mutex_lock(my_mutex);
    shared_var += 1;
    mutex_unlock(my_mutex);
    barrier_wait(&my_barrier);

    // printf("HELLOWORLD %u\n", me());
    if(!me()) {
        printf("\n%d\n", shared_var);
    }

    // int checksum = 0;
    for(int i = me() * NR_TASKLETS; i < (me() + 1) * NR_TASKLETS; i++) {
        checksums[i] += buffer[i];
    }
    barrier_wait(&my_barrier2);

    
    if(!me()) {
        int checksum = 0;
        for(int i=1; i<NR_TASKLETS; i++){
            checksums[0] += checksums[i];
        }
        printf("\n%d\n", checksums[0]);

    }
}
