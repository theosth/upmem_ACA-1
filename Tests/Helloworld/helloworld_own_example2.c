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

int main(){
    mutex_lock(my_mutex);
    shared_var += 1;
    mutex_unlock(my_mutex);
    barrier_wait(&my_barrier);
    printf("HELLOWORLD %u\n", me());
    if(!me()) {
        printf("\n%d\n", shared_var);
    }
}
