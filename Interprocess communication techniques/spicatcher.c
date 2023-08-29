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
#include "shared2.h"
#include <sys/sem.h>
#include <semaphore.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <sys/stat.h>
#include <math.h>
#include <sys/msg.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// Define variables
#define MAX_LENGTH 10000
#define MAX_MESSAGES 2000
int flag = 0;
int stop = 0;
int spy_pid;
int Delay;
int Total_Number_of_spies;
// Define Functions

int stringExistsInFile(const char *filename, const char *str);

void handle_sigusr_MasterSPY(int sig)
{
    srand(time(NULL));
    // For shared memory to catch data from the existing shared memory
    key_t key = 1234; // create unique key
    int shmid = shmget(key, sizeof(struct shared_data), 0666 | IPC_CREAT);
    struct shared_data *data = (struct shared_data *)shmat(shmid, NULL, 0);

    int maxColumns = data->number_of_columns;
    int Number_of_catches_per_spi = (maxColumns * 2) / Total_Number_of_spies;
    char Catched_Column[MAX_LENGTH];

    while (Number_of_catches_per_spi > 0)
    {
        int random_index = rand() % maxColumns; // Generate random column index 1
        usleep(100 * Total_Number_of_spies);
        //  read from share memory at random location
        strcpy(Catched_Column, data->value[random_index]);
        printf("------------------------------------------------------>>>> Master-SPY Catches data = [%s]", Catched_Column);
        char ch = Catched_Column[strlen(Catched_Column) - 1];
        char *charPtr = strchr(list_of_char, ch);
        if (charPtr != NULL) // Exist
        {
            printf(" Exists, Drop it!.\n");
        }
        else // New
        {
            printf(" New, Take it!.\n");
            char charStr[2] = {ch, '\0'};
            strcat(list_of_char, charStr);

            FILE *file = fopen("Unique_SPY_COLUMNS.txt", "a");
            if (file == NULL)
            {
                printf("Failed to open the file.\n");
            }

            fprintf(file, "%s\n", Catched_Column);
            fclose(file);
        }

        Number_of_catches_per_spi--;
    }

    if (Number_of_catches_per_spi <= 0)
    {
        printf(">>>>>>>>>>>> P[%d] Sending Message Queue to The Master-Spy.\n", spy_pid);
        kill(getppid(), SIGUSR2);
        exit(0);
    }
}

void handle_sigusr_MasterSPY_STOP(int sig)
{
    // exit(0);
}

int main(int argc, char *argv[])
{
    FILE *filex = fopen("Unique_SPY_COLUMNS.txt", "w");
    if (filex == NULL)
    {
        printf("Failed to open the file.\n");
        return -1;
    }
    fclose(filex);

    srand(time(NULL));
    spy_pid = atoi(argv[1]);
    Total_Number_of_spies = atoi(argv[2]);
    Delay = atoi(argv[3]);
    // struct sigaction sa_spy;
    // sa_spy.sa_handler = &handle_sigusr_MasterSPY;
    // sa_spy.sa_flags = SA_RESTART;
    // sigaction(SIGUSR2, &sa_spy, NULL);
    signal(SIGUSR2, handle_sigusr_MasterSPY);

    // struct sigaction sa_spy_stop;
    // sa_spy_stop.sa_handler = &handle_sigusr_MasterSPY_STOP;
    // sa_spy_stop.sa_flags = SA_RESTART;
    // sigaction(SIGUSR1, &sa_spy_stop, NULL);
    signal(SIGUSR1, handle_sigusr_MasterSPY_STOP);

    while (1)
    {
    }

    return 0;
}

//