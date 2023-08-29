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

// Define Functions
char *Encrypt(const char *str, int key);
char *Convert_Numbers(char *str);

// Semaphore union, needed for semctl
union semun
{
    int val;
    struct semid_ds *buf;
    ushort *array;
};

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        fprintf(stderr, "Insufficient arguments.\n");
        exit(1);
    }

    // Defining Variables and pass Values
    pid_t child_pid = getpid();
    char *token = argv[2];
    char *Number = argv[3];
    int key = atoi(Number) + 1; // key will be used for encryption
    int index = key - 1;

    // Shared Memory & semaphore variables
    key_t key_sm = 1234; // create unique key
    int shmid = shmget(key_sm, sizeof(struct shared_data), 0666 | IPC_CREAT);
    struct shared_data *data = (struct shared_data *)shmat(shmid, NULL, 0);

    // semaphore
    key_t key_sem = ftok("semfile", 75);
    int semid = semget(key_sem, 1, 0666 | IPC_CREAT);

    // Semaphore initialization
    union semun sem_union;
    sem_union.val = 1;
    if (semctl(semid, 0, SETVAL, sem_union) == -1)
    {
        perror("Semaphore initialization failed");
        return -1;
    }
    struct sembuf semaphore;

    //   Encrypt: 1- encrypt numbers. 2- encrypt characters.
    char *Encrypt_Numbers_first = Convert_Numbers(token);
    char *Encrypted_token = Encrypt(Encrypt_Numbers_first, key);
    printf("Column[%d] ->> [ %s] Encrypted -> ", index + 1, token);
    // End of encryption

    // Semaphore Acquire
    semaphore.sem_num = 0;
    semaphore.sem_op = -1; // this will wait for semaphore to be zero and then decrease it
    semaphore.sem_flg = SEM_UNDO;
    if (semop(semid, &semaphore, 1) == -1)
    {
        perror("Semaphore wait operation failed");
        return -1;
    }

    // Write data to shared memory
    if (strcpy(data->value[index], Encrypted_token))
    {
        printf("Added To Shared Memory at Location [%d]. | SIG The Sender\n", index + 1);

        // Semaphore Release / Up the semaphore after you're done with the critical section
        semaphore.sem_op = 1; // this will increase the semaphore
        if (semop(semid, &semaphore, 1) == -1)
        {
            perror("Semaphore signal operation failed");
            return -1;
        }
        // Detach from shared memory
        shmdt(data);
        // Free the dynamically allocated memory
        free(Encrypted_token);
        // Send signal to sender.c
        usleep(index * 10000);
        kill(getppid(), SIGUSR1);
    }
    return 0;
}

char *Encrypt(const char *str, int key)
{
    int length = strlen(str);
    char *Encrypted_string = (char *)malloc((10 * length) * sizeof(char)); // Allocate memory for modified string
    int i;
    for (i = 0; i < length; i++)
    {
        int mul = (i + 1) * key;
        // Add i+1 and take mod 26
        if (str[i] == ' ')
        {
            Encrypted_string[i] = ' ';
        }
        else if (str[i] == '!')
        {
            Encrypted_string[i] = '!';
        }
        else if (str[i] == '?')
        {
            Encrypted_string[i] = '?';
        }
        else if (str[i] == ',')
        {
            Encrypted_string[i] = ',';
        }
        else if (str[i] == ';')
        {
            Encrypted_string[i] = ';';
        }
        else if (str[i] == ':')
        {
            Encrypted_string[i] = ':';
        }
        else if (str[i] == '%')
        {
            Encrypted_string[i] = '%';
        }
        else if (str[i] == '.')
        {
            Encrypted_string[i] = '.';
        }
        else if (isdigit(str[i]))
        {
            Encrypted_string[i] = str[i];
        }
        else if (isalpha(str[i]))
        {
            char base = isupper(str[i]) ? 'A' : 'a';
            Encrypted_string[i] = ((str[i] - base + mul) % 26) + base;
        }
    }
    char suffix = 'a' + (key - 1);
    // printf("Suffix = %c\n", suffix);
    Encrypted_string[length] = suffix; // Add null terminator to the modified string

    return Encrypted_string;
}

char *Convert_Numbers(char *str)
{
    int length = strlen(str);
    char *newStr = (char *)calloc(length * 2, sizeof(char)); // allow more space
    char *currentNumber = (char *)calloc(length + 1, sizeof(char));
    int currentNumberIndex = 0;

    if (newStr == NULL || currentNumber == NULL)
    {
        printf("Memory allocation failed\n");
        return NULL;
    }

    for (int i = 0; i < length; i++)
    {
        if (str[i] >= '0' && str[i] <= '9')
        {
            currentNumber[currentNumberIndex++] = str[i];
        }
        else
        {
            if (currentNumberIndex != 0)
            {
                currentNumber[currentNumberIndex] = '\0';
                int number = atoi(currentNumber);
                number = 1000000 - number;
                char buffer[50]; // temporary buffer for new number
                sprintf(buffer, "%d", number);
                strcat(newStr, buffer);
                currentNumberIndex = 0;
            }
            strncat(newStr, &str[i], 1);
        }
    }

    if (currentNumberIndex != 0)
    {
        currentNumber[currentNumberIndex] = '\0';
        int number = atoi(currentNumber);
        number = 1000000 - number;
        char buffer[50]; // temporary buffer for new number
        sprintf(buffer, "%d", number);
        strcat(newStr, buffer);
    }

    free(currentNumber);
    return newStr;
}

//