#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "shared2.h"
#include <ctype.h>
#include <sys/sem.h>
#include <semaphore.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <sys/stat.h>
#include <wctype.h>

// Define variables
#define MAX_LENGTH 1000
#define MAX_SPIES 100
int flag = 0;
int breaker = 0;
int stop = 0;
int Number_Of_Spies;
pid_t child_pids[MAX_SPIES];

// Define Functions
int RemoveDuplicates();
void Decoding(int maxColumns);
char *Decode_Numbers(char *str);
char *Decrypt(const char *str, int key);

void handle_sigusr_parent(int sig)
{
    printf("------------------------------------------------- >>> MASTER-SPY SIGNAL REACHED\n");

    // printf("SPY Start picking from shared memory.\n");
    //  Send signal from Master spy to his childs
    for (int y = 0; y < Number_Of_Spies; y++)
    {
        usleep(10000 / Number_Of_Spies);
        kill(child_pids[y], SIGUSR2);
    }

    while (breaker != Number_Of_Spies)
        ;

    if (breaker == Number_Of_Spies)
    {
        // sleep();
        printf("-------------------------->>>| Master-Spy is Confident That He Recieve Enough Encrypted Columns, Time To Decode |<<<-------------------------- \n");
        int maxColumns = RemoveDuplicates(); // work here , i think that there is no duplicates anymore

        // printf(">>>>>>>>>>>>>>>>>>>>> Duplicates Removed!\n");
        Decoding(maxColumns);
        kill(getppid(), SIGUSR1);
    }
    breaker = 0;
}

void handle_sigusr_parent_stop(int sig)
{
    for (int e = 0; e < Number_Of_Spies; e++)
    {
        usleep(10000 / Number_Of_Spies);
        kill(child_pids[e], SIGUSR1);
    }
    breaker = 0;
    //exit(0);
}

void handle_sigusr_CHILDSPY(int sig)
{
    breaker += 1;
}

int main(int argc, char *argv[])
{
    // Define Signal variables
    // struct sigaction sa_parent;
    // sa_parent.sa_handler = &handle_sigusr_parent;
    // sa_parent.sa_flags = SA_RESTART;
    // sigaction(SIGUSR1, &sa_parent, NULL);
    signal(SIGUSR1, handle_sigusr_parent);

    // struct sigaction sa_child_spy;
    // sa_child_spy.sa_handler = &handle_sigusr_CHILDSPY;
    // sa_child_spy.sa_flags = SA_RESTART;
    // sigaction(SIGUSR2, &sa_child_spy, NULL);
    signal(SIGUSR2, handle_sigusr_CHILDSPY);

    signal(SIGINT, handle_sigusr_parent_stop);

    Number_Of_Spies = atoi(argv[1]);
    printf("You are in Spy.c | Childs = %d.\n", Number_Of_Spies);

    for (int i = 0; i < Number_Of_Spies; i++)
    {
        pid_t pid = fork();

        if (pid < 0)
        {
            // Fork failed
            perror("fork");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        {
            // Child process
            printf("spy %d: PID = %d\n", i + 1, getpid());
            char spy_pid[10];
            sprintf(spy_pid, "%d", getpid());
            char index[10];
            sprintf(index, "%d", i);
            execl("./spicatcher", "spicatcher", spy_pid, argv[1], index, NULL);
            exit(EXIT_SUCCESS);
        }
        else
        {
            // Parent process
            child_pids[i] = pid;
        }
    }

    while (1)
    {
    }

    return 0;
}

int RemoveDuplicates()
{
    FILE *inputFile;
    char line[MAX_LENGTH];
    int result = 0;
    // Open input file for reading
    inputFile = fopen("Unique_SPY_COLUMNS.txt", "r");
    if (inputFile == NULL)
    {
        printf("Unable to open the input file.\n");
    }

    // Read each line from the input file
    while (fgets(line, MAX_LENGTH, inputFile) != NULL)
    {
        result++;
    }

    // Close the input and output files
    fclose(inputFile);
    // remove("Unique_SPY_COLUMNS.txt");
    return result;
}

void Decoding(int maxColumns)
{

    printf("->>>>>>>>>>>>>>>>>>>>>>>>>| DECODING For MASTER-SPY Starts Now |<<<<<<<<<<<<<<<<<<<<<<<<<-\n");
    sleep(1);
    // Ordering Lines Correctly!
    FILE *file2 = fopen("Unique_SPY_COLUMNS.txt", "r");
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
    remove("Unique_SPY_COLUMNS.txt");

    FILE *file3 = fopen("MASTER_SPY_TEMP.txt", "a");
    if (file3 == NULL)
    {
        printf("Failed to open the file.\n");
    }

    //  Start Decoding
    for (int i = 0; i < maxColumns; i++)
    {
        char *Decoded_token = Decrypt(True_Order[i], i + 1);
        char *Decode_Numbers_first = Decode_Numbers(Decoded_token);
        fprintf(file3, "%s\n", Decode_Numbers_first);
    }
    fclose(file3);

    // remove("REC_TEMP.txt");
    // Get Number of lines
    FILE *file35 = fopen("MASTER_SPY_TEMP.txt", "r");
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
    FILE *file4 = fopen("MASTER_SPY_TEMP.txt", "r");
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
    remove("MASTER_SPY_TEMP.txt");

    // write Decoded to receiver.txt
    FILE *file5 = fopen("spy.txt", "a");
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

//