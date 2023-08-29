#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <ctype.h>
#include "shared.h"
#include <sys/sem.h>
#include <semaphore.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <sys/stat.h>

// Define variables
#define MAX_LENGTH 10000
int flag = 0;
int breaker = 0;
int maxColumns;
key_t key;
int shmid;
// int index;

void handle_sigusr_parent(int sig)
{
    srand(time(NULL));
    key = 1234; // create unique key
    shmid = shmget(key, sizeof(struct shared_data), 0666 | IPC_CREAT);
    struct shared_data *data = (struct shared_data *)shmat(shmid, NULL, 0);
    maxColumns = data->number_of_columns;

    char temp[MAX_LENGTH];
    int col1 = rand() % maxColumns; // Generate random column index 1
    int col2;

    do
    {
        col2 = rand() % maxColumns; // Generate random column index 2
    } while (col1 == col2);         // Repeat until col2 is different from col1
    strcpy(temp, data->value[col1]);
    strcpy(data->value[col1], data->value[col2]);
    strcpy(data->value[col2], temp);
    printf("[ %d ]  -> <- [ %d ].\n", col1 + 1, col2 + 1);
}

void handle_sigusr_parent_2(int sig)
{
    // exit(0);
}

int main(int argc, char *argv[])
{

    srand(time(NULL));
    // index = atoi(argv[1]);

    // struct sigaction sa_parent;
    // sa_parent.sa_handler = &handle_sigusr_parent;
    // sa_parent.sa_flags = SA_RESTART;
    // sigaction(SIGUSR1, &sa_parent, NULL);
    signal(SIGUSR1, handle_sigusr_parent);

    // struct sigaction sa_parent_2;
    // sa_parent_2.sa_handler = &handle_sigusr_parent_2;
    // sa_parent_2.sa_flags = SA_RESTART;
    // sigaction(SIGUSR2, &sa_parent_2, NULL);
    signal(SIGUSR2, handle_sigusr_parent_2);

    while (1)
    {
    }

    return 0;
}

//