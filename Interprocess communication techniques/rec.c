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
#include <wctype.h>

// Define variables
#define MAX_LENGTH 10000
#define MAX_LINES 1000
int flag = 0;
int stop = 0;
int Delay_t;
// Define Functions
void Decoding(int maxColumns);
char *Decode_Numbers(char *str);
char *Decrypt(const char *str, int key);

void handle_sigusr_parent(int sig)
{
    printf("------------------------------------------------- >>> RECIEVER SIGNAL REACHED\n");
    key_t key = 1234; // create unique key
    int shmid = shmget(key, sizeof(struct shared_data), 0666 | IPC_CREAT);
    struct shared_data *data = (struct shared_data *)shmat(shmid, NULL, 0);

    int maxColumns = data->number_of_columns;
    int Number_Of_Required_Catches = data->number_of_columns;
    char Catched_Column[MAX_LENGTH];
    char Checklist[maxColumns];

    FILE *file1 = fopen("RECCOLLECTOR.txt", "a");
    if (file1 == NULL)
    {
        printf("Failed to open the file.\n");
        exit(1);
    }
    int found = 0;
    int index = 0;
    usleep(5000);

    while (Number_Of_Required_Catches > 0)
    {

        // usleep(300000);
        int random_index = rand() % maxColumns; // Generate random column index 1
        // read from shared memory at random location
        if (strcpy(Catched_Column, data->value[random_index]))
        {
            // usleep(20000);
            for (int i = 0; i < index; i++)
            {
                usleep(200 * Delay_t);
                if (Catched_Column[strlen(Catched_Column) - 1] == Checklist[i])
                {
                    found = 1;
                }
            }
            if (found == 1)
            {
                printf("------------------------------------------------------>>>> Receiver Catches data = [%s] Exists, Drop it!.\n", Catched_Column);
                found = 0;
            }
            else if (found == 0)
            {
                printf("------------------------------------------------------>>>> Receiver Catches data = [%s] New, Take it!.\n", Catched_Column);
                Checklist[index] = Catched_Column[strlen(Catched_Column) - 1];
                index++;
                fprintf(file1, "%s\n", Catched_Column);
                Number_Of_Required_Catches--;
            }
        }
    }

    if (Number_Of_Required_Catches == 0)
    {
        printf(">>>> Reciver Catch All He Needs <<<<\n");
        fclose(file1);
        // Decode.
        Decoding(maxColumns);
        // Signal to parent that reciver finish
        kill(getppid(), SIGINT);
    }
}

void handle_sigusr_parent_stop(int sig)
{
    while (1)
        ;
}

int main(int argc, char *argv[])
{
    printf("You are in Reciever.c \n");
    Delay_t = atoi(argv[1]);
    // For shared memory to catch data from the existing shared memory

    srand(time(NULL));

    // struct sigaction sa_parent;
    // sa_parent.sa_handler = &handle_sigusr_parent;
    // sa_parent.sa_flags = SA_RESTART;
    // sigaction(SIGUSR2, &sa_parent, NULL);
    signal(SIGUSR2, handle_sigusr_parent);
    signal(SIGINT, handle_sigusr_parent_stop);
    while (1)
    {
    }

    return 0;
}

void Decoding(int maxColumns)
{
    printf("->>>>>>>>>>>>>>>>>>>>>>>>>| DECODING For RECEIVER |<<<<<<<<<<<<<<<<<<<<<<<<<-\n");

    // Ordering Lines Correctly!
    FILE *file2 = fopen("RECCOLLECTOR.txt", "r");
    if (file2 == NULL)
    {
        printf("Failed to open the file.\n");
    }

    char line[MAX_LENGTH];
    char True_Order[maxColumns][MAX_LENGTH];
    while (fgets(line, sizeof(line), file2))
    {
        int lineLength = strlen(line);
        if (lineLength > 0)
        {
            char lastChar = line[lineLength - 2];
            int number = lastChar - 'a' + 1; // charToNumber(lastChar);
            line[lineLength - 2] = '\0';
            strcpy(True_Order[number - 1], line);
        }
    }

    fclose(file2);
    remove("RECCOLLECTOR.txt");

    FILE *file3 = fopen("REC_TEMP.txt", "a");
    if (file3 == NULL)
    {
        printf("Failed to open the file.\n");
    }

    //  Start Decoding
    for (int i = 0; i < maxColumns; i++)
    {
        char *Decoded_token = Decrypt(True_Order[i], i + 1);
        // printf("Decoded : %s\n", Decoded_token);
        char *Decode_Numbers_first = Decode_Numbers(Decoded_token);
        fprintf(file3, "%s\n", Decode_Numbers_first);
    }
    fclose(file3);

    // remove this

    // remove("REC_TEMP.txt");
    // Get Number of lines
    FILE *file35 = fopen("REC_TEMP.txt", "r");
    if (file35 == NULL)
    {
        printf("Failed to open the file.\n");
    }

    char temp_column[MAX_LENGTH];
    int Lines = 0;
    if (fgets(temp_column, sizeof(temp_column), file35) != NULL)
    {
        if (temp_column[strlen(temp_column) - 1] == '\n')
        {
            temp_column[strlen(temp_column) - 1] = '\0';
        }
        char *temp_word = strtok(temp_column, " ");
        while (temp_word != NULL)
        {
            Lines++;
            temp_word = strtok(NULL, " ");
        }
    }
    fclose(file35);

    // order in columns
    char column[MAX_LENGTH];
    char column_words[Lines][MAX_LENGTH];
    FILE *file4 = fopen("REC_TEMP.txt", "r");
    if (file4 == NULL)
    {
        printf("Failed to open the file.\n");
    }

    while (fgets(column, sizeof(column), file4) != NULL)
    {
        int index = 0;
        //  Remove the trailing newline character, if present
        if (column[strlen(column) - 1] == '\n')
        {
            column[strlen(column) - 1] = '\0';
        }

        // Split the row into words
        char *word = strtok(column, " ");

        while (word != NULL)
        {
            //  Copy the word into the array
            if (strcmp(word, "alright") == 0)
            {
                strcat(column_words[index], "");
            }
            else
            {
                strcat(column_words[index], word);
                strcat(column_words[index], " ");
            }

            index++;
            // Get the next word
            word = strtok(NULL, " ");
        }
    }
    fclose(file4);
    remove("REC_TEMP.txt");

    // write Decoded to receiver.txt
    FILE *file5 = fopen("receiver.txt", "a");
    if (file5 == NULL)
    {
        printf("Failed to open the file.\n");
    }

    for (int f = 0; f < Lines; f++)
    {
        fprintf(file3, "%s\n", column_words[f]);
    }
    fclose(file5);
}

char *Decode_Numbers(char *str)
{
    int length = strlen(str);
    char *newStr = (char *)calloc(length * 2, sizeof(char)); // allocate more space
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
                char buffer[50]; // temporary buffer for original number
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
        char buffer[50]; // temporary buffer for original number
        sprintf(buffer, "%d", number);
        strcat(newStr, buffer);
    }

    free(currentNumber);
    return newStr;
}

char *Decrypt(const char *str, int key)
{
    int length = strlen(str);
    char *Decrypted_string = (char *)malloc((length + 1) * sizeof(char)); // Allocate memory for decrypted string
    int i;
    for (i = 0; i < length; i++)
    {
        int mul = (i + 1) * key;
        // Add i+1 and take mod 26
        if (str[i] == ' ')
        {
            Decrypted_string[i] = ' ';
        }
        else if (str[i] == '!')
        {
            Decrypted_string[i] = '!';
        }
        else if (str[i] == '?')
        {
            Decrypted_string[i] = '?';
        }
        else if (str[i] == ',')
        {
            Decrypted_string[i] = ',';
        }
        else if (str[i] == ';')
        {
            Decrypted_string[i] = ';';
        }
        else if (str[i] == ':')
        {
            Decrypted_string[i] = ':';
        }
        else if (str[i] == '%')
        {
            Decrypted_string[i] = '%';
        }
        else if (str[i] == '.')
        {
            Decrypted_string[i] = '.';
        }
        else if (isdigit(str[i]))
        {
            Decrypted_string[i] = str[i];
            mul += mul;
        }
        else if (isalpha(str[i]))
        {
            if (str[i] >= 'A' && str[i] <= 'Z')
            {
                Decrypted_string[i] = 'A' + (str[i] - 'A' - (mul % 26) + 26) % 26; // Reverse the encryption
            }
            else if (str[i] >= 'a' && str[i] <= 'z')
            {
                Decrypted_string[i] = 'a' + (str[i] - 'a' - (mul % 26) + 26) % 26; // Reverse the encryption
            }
        }
    }
    Decrypted_string[length] = '\0';
    return Decrypted_string;
}

// ABCDEFGHIJKLMNOPQRSTUVWXYZ