#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

double sleep_time = 0.25;
int flag = 0;

void handle_sigusr(int sig){
    flag = 1;
	printf("signal recieved for children with -> PID [ %d ]\n", getpid());
    sleep(1);
}

int main(int argc, char *argv[]) {

    // Check that the correct number of arguments is passed
    if (argc != 2) {
        printf("Error: Invalid number of arguments.\n");
        exit(1);
    }

    printf("child [ %d ] has arrived to child.c \n", getpid());
sleep(1);
    int pid;
    pid = getpid();
    struct sigaction sa;
    sa.sa_handler = &handle_sigusr;
    sa.sa_flags = SA_RESTART;
    sigaction(SIGUSR1, &sa, NULL);
sleep(1);
    while(flag != 1){
        sleep(1);
    }

    if(flag == 1){
        flag = 0;

        char pid_str[10];
        float min, max, rand_num;
sleep(1);
        if (pid <= 0) {
            printf("Error: Invalid process ID.\n");
            exit(1);
            }

        FILE *fp = fopen("range.txt", "r");

        if (fp == NULL) {
            printf("Error: Cannot open file.\n");
            exit(1);
            }

        fscanf(fp, "%f-%f", &min, &max);

        fclose(fp);


        srand(time(NULL) * pid); // Use process ID as seed
        rand_num = ((float) rand() / RAND_MAX) * (max - min) + min;

sleep(1);
        sprintf(pid_str, "%d", pid);

        fp = fopen(strcat(pid_str, ".txt"), "w");

        if (fp == NULL) {
            printf("Error: Cannot create file.\n");
            exit(1);
            }

        fprintf(fp, "%0.3f", rand_num);

        fclose(fp);
sleep(1);
        // Print a message to indicate that the file has been written
        printf("Child process with PID %d has written to file %s\n", pid, pid_str);

        sleep(sleep_time);
        sleep_time += 0.125;
        // signal to parent that the value is ready to be picked
        kill(getppid(), SIGUSR1);
        sleep(1);
        }
    
    //while(1);
    return 0;
}