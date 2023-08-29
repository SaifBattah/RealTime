#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "shared.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <string.h>

#define BUFFER_SIZE 50

int read_flag = 0;
const char* MY_STRING;
void handle_sigusrx(int sig){
    read_flag = 1;
	printf("signal recieved for children 5 with -> PID [ %d ]\n", getpid());

    
    sleep(1);
}


int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("Error: Invalid number of arguments.\n");
        exit(1);
    }

    printf("Child [ %d ] has arrived to coPro.c!\n", getpid());
    float v1,v2,v3,v4;
    int f_des[2];
    static char message[BUFFER_SIZE];
    int pid;
    pid = getpid();
    struct sigaction sax;
    sax.sa_handler = &handle_sigusrx;
    sax.sa_flags = SA_RESTART;
    sleep(1);
    sigaction(SIGUSR1, &sax, NULL);
    sleep(1);


    while(read_flag != 1){
        sleep(1);
    }

    if(read_flag == 1){
        read_flag = 0;
        key_t key = ftok("shared_memory_key", 1234);  // Use the same key as in parent.c
        int shmid = shmget(key, sizeof(struct shared_data), 0666);  // Get access to the existing shared memory segment
        struct shared_data *shared_memory = (struct shared_data *)shmat(shmid, NULL, 0);  // Attach shared memory segment
        // Read the string value from shared memory and use it
        printf("Received value from parent in co-Processor.c: %s\n", shared_memory->value);

        sscanf(shared_memory->value, "%f-%f-%f-%f",&v1,&v2,&v3,&v4);





    //printf("Received %f\n", arr[i]);

    float sum1 = v1 + v2;
    float sum2 = v3 + v4;
    char msg[50] = {0};
    sprintf(msg, "%.3f-%.3f", sum1, sum2);
    //printf("%s\n", msg);
    strcpy(shared_memory->value, msg);
    kill(getppid(), SIGUSR1);
    }

    return 0;
}
    
