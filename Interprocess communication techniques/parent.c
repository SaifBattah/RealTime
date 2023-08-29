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
#define MAX_LENGTH 1000
int looper = 0;
int flag = 0;
int flag_SPY_DONE = 0;
int flag_REC_DONE = 0;
int helper_counter = 0;
int times = 0;
int parent_counter = 0;
int rec = 0;
int spy = 0;
int rec_win = 0;
int spy_win = 0;
// Global variable to store the signal received from the child process
volatile sig_atomic_t received_signal = 0;

//  Defining Functions
void Create_Sender_file();

void handle_sigusr_sender(int sig)
{
    flag = 1;
}

void handle_sigusr_MASTER_SPY(int sig)
{
    if (parent_counter == 0)
    {
        rec = 2;
        // printf("\n\nSTOP SWAPPING ~~ MASTER-SPY ~~ SOLVE IT!\n");
        parent_counter = 1;
    }
}

void handle_sigusr_RECEIVER(int sig)
{
    if (parent_counter == 0)
    {
        rec = 1;
        // printf("\n\nSTOP SWAPPING ~~ MASTER-SPY ~~ SOLVE IT!\n");
        parent_counter = 1;
    }
}

int main()
{
    // struct sigaction sa_sender_and_receiver;
    // sa_sender_and_receiver.sa_handler = &handle_sigusr_sender_receiver;
    // sa_sender_and_receiver.sa_flags = SA_RESTART;
    // sigaction(SIGUSR2, &sa_sender_and_receiver, NULL);
    signal(SIGUSR2, handle_sigusr_sender);

    // struct sigaction sa_MASTERSPY;
    // sa_MASTERSPY.sa_handler = &handle_sigusr_MASTER_SPY;
    // sa_MASTERSPY.sa_flags = SA_RESTART;
    // sigaction(SIGUSR1, &sa_MASTERSPY, NULL);
    signal(SIGUSR1, handle_sigusr_MASTER_SPY);

    signal(SIGINT, handle_sigusr_RECEIVER);

    printf("Parent = %d.\n\n", getpid());
    int numHelpers, numSpies, Threshold;
    int Current_Round = 1;

    // Open the file
    FILE *filed = fopen("VALUES.txt", "r");
    if (filed == NULL)
    {
        printf("Failed to open the file.\n");
        return 1; // Exit the program with an error status
    }

    // Read the values from each line
    fscanf(filed, "%d", &numHelpers);
    fscanf(filed, "%d", &numSpies);
    fscanf(filed, "%d", &Threshold);
    // Close the file
    fclose(filed);

    // Print the values
    printf("Number of helpers: %d\n", numHelpers);
    printf("Number of spies: %d\n", numSpies);
    printf("Threshold: %d\n", Threshold);

    // Calculate Number Of Processes to be created by parent
    int Number_Of_Processes = 3 + numHelpers; // + numSpies;
    pid_t child_pids[Number_Of_Processes];

    // Create the Sender, Receiver, Spy and other Processes.
    for (int i = 0; i < Number_Of_Processes; i++)
    {
        srand(time(NULL));
        usleep(960000);
        child_pids[i] = fork();
        if (child_pids[i] == 0)
        {
            // Child process
            if (i == 0)
            {
                printf("---------------------------------------\n\n");
                printf("Sender Process Created.\n");
                execl("./sender", "sender", NULL);
                sleep(2);
            }
            else if (i == 1)
            {
                // usleep(i * 10000);
                char spies_number[10];
                sprintf(spies_number, "%d", numSpies);
                printf("Reciever Process Created.\n");
                execl("./rec", "rec", spies_number, NULL);
            }
            else if (i == 2)
            {
                // usleep(i * 10000);
                printf("Master Spy Process Created.\n");
                char number_of_spies[10];
                sprintf(number_of_spies, "%d", numSpies);
                execl("./spy", "spy", number_of_spies, NULL);
            }

            else if (i >= 3 && i < Number_Of_Processes)
            {
                // usleep(10000);
                printf("Helper Process %d Created.\n", i - 2);
                char index[10];
                sprintf(index, "%d", i - 2);
                execl("./helper", "helper", index, NULL);
            }
            // break;
        }
    }
    /*
    pid_t dis = fork();
    if (dis < 0)
    {
        perror("Fork failed\n");
        exit(1);
    }
    else if (dis == 0)
    {
        char *i1 = (char *)malloc(sizeof(int));
        sprintf(i1, "%d_%d_%d_%d", numHelpers, numSpies, rec_win, spy_win); // openGL stuffs
        if (execl("./display", "./display", i1, (char *)NULL) == -1)
        {
            perror("Exec Error");
            exit(-1);
        }
    }
    */

    while (flag != 1)
        ;
    // after parent receive signal from sender.
    // start data swapping and picking
    if (flag == 1)
    {
        sleep(3);
        while (spy_win < Threshold && rec_win < Threshold)
        {
            sleep(1);
            printf("----------------------------------------------------- ROUND %d Starts! -----------------------------------------------------\n", Current_Round);
            FILE *filetxry;
            filetxry = fopen("Unique_SPY_COLUMNS.txt", "w");
            if (filetxry != NULL)
            {
                fclose(filetxry);
            }

            // Create MASTER_SPY_TEMP.txt
            filetxry = fopen("MASTER_SPY_TEMP.txt", "w");
            if (filetxry != NULL)
            {
                fclose(filetxry);
            }

            // Create RECCOLLECTOR.txt
            filetxry = fopen("RECCOLLECTOR.txt", "w");
            if (filetxry != NULL)
            {
                fclose(filetxry);
            }

            // Create REC_TEMP.txt
            filetxry = fopen("REC_TEMP.txt", "w");
            if (filetxry != NULL)
            {
                fclose(filetxry);
            }

            filetxry = fopen("spy.txt", "w");
            if (filetxry != NULL)
            {
                fclose(filetxry);
            }

            filetxry = fopen("receiver.txt", "w");
            if (filetxry != NULL)
            {
                fclose(filetxry);
            }
            //  start swapping by signal the helpers

            for (int i = 3; i < Number_Of_Processes; i++)
            {
                usleep(10000);
                // sleep(1);
                kill(child_pids[i], SIGUSR1);
            }

            kill(child_pids[2], SIGUSR1); // signal for MASTER-SPY to start CATCHING
            kill(child_pids[1], SIGUSR2); // signal for RECEIVER to start CATCHING
            // sleep(1); // x try to remove  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

            sleep(1);

            while (parent_counter == 0) // keep swapping
                ;
            if (parent_counter == 1) // stop swapping
            {
                parent_counter = 0;
                for (int t = 3; t < Number_Of_Processes; t++)
                {
                    kill(child_pids[t], SIGUSR2); // to helper to stop swapping
                }

                // sleep(10000);
                //  kill(child_pids[2], SIGUSR1);
                //  kill(child_pids[1], SIGUSR2);
                // usleep(30000);
                //  remove("spy.txt");
                //  remove("receiver.txt");
                if (rec == 2)
                {
                    spy_win++;
                    rec = 0;
                    kill(child_pids[1], SIGINT);
                    printf("~~~~ MASTER-SPY WINS ~~~~\n");
                }
                else if (rec == 1)
                {
                    rec_win++;
                    rec = 0;
                    kill(child_pids[2], SIGINT);
                    printf("~~~~ RECEIVER WINS ~~~~\n");
                }

                // return 0;
            }
            // to be removed. used to stop swapping by using signal counter
            printf("----------------------------------------------------- ROUND %d Ends! ------------------------------------------------------\n\n", Current_Round);

            Current_Round++;
            sleep(2);
        }

        if (spy_win == Threshold || rec_win == Threshold)
        {
            printf("Final Result: MASTER-SPY [%d] | RECEIVER [%d].\n", spy_win, rec_win);
            printf("And The Winner is : ");
            if (spy_win > rec_win)
            {
                printf("MASTER-SPY!\n");
            }
            else
            {
                printf("RECEIVER!\n");
            }
        }
        return 0;
    }
}

void Create_Sender_file()
{
    FILE *file = fopen("Sender.txt", "w"); // Open file in write mode

    if (file == NULL)
    {
        printf("Error opening the file.\n");
    }

    printf("Sender.txt Has Created!\n");
    fclose(file); // Close the file
}

//
