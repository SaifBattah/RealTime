#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "shared.h"

#define MAX_LENGTH 100000
#define MAX_WORD_LENGTH 1000
#define buffer 10000

// Defining global variables
int counter = 0;
int sleep_time = 0.25;

// Defining Functions
void Clean_Sender_Spaces();
int Get_Longest_Line();
int Get_Number_Of_Lines();

void handle_sigusrp(int sig)
{
    counter++;
}

int main()
{
    int parent_id = getppid();

    key_t key = 1234; // create unique key
    int shmid = shmget(key, sizeof(struct shared_data), 0666 | IPC_CREAT);
    struct shared_data *data = (struct shared_data *)shmat(shmid, NULL, 0);
    int Number_Of_Columns = 0;
    int Number_Of_Rows = 0;
    char *token = (char *)malloc(buffer);
    token[0] = '\0';

    // Cleaning file from over spaces
    Clean_Sender_Spaces();

    Number_Of_Columns = Get_Longest_Line();
    Number_Of_Rows = Get_Number_Of_Lines();
    data->number_of_columns = Number_Of_Columns;
    // Get the longest Number of columns
    printf("Number Of Columns(maximum number of words) = %d\nNumber of Rows(lines) = %d\n", Number_Of_Columns, Number_Of_Rows);
    printf("-----------------------------------------------\n\n");
    // printf("All child processes have finished.\n");

    // Create Shared memory.
    printf("Creating Shared Memory!\n");
    // Create_Shared_Memory(Number_Of_Columns);

    printf("Shared Memory Created Successfully with %d locations.\n", Number_Of_Columns);
    printf("-----------------------------------------------------\n\n");

    // Create Array of words to pass them to childs
    // Initialize variables
    char columns_words[Number_Of_Columns][Number_Of_Rows][MAX_WORD_LENGTH];
    int num_lines = 0;
    int num_words[100] = {0};
    char line[MAX_WORD_LENGTH];

    pid_t processes[Number_Of_Columns];
    char pid_str[Number_Of_Columns][10];
    char Column_Number[10];

    // Open the file for reading
    printf("Open Sender.txt to load contents and divide into columns.\n");
    FILE *file = fopen("Sender.txt", "r");
    if (file == NULL)
    {
        printf("Failed to open the file.\n");
        return 1;
    }

    // Read the file line by line
    while (fgets(line, sizeof(line), file) != NULL && num_lines <= Number_Of_Rows)
    {
        // Remove the trailing newline character, if present
        if (line[strlen(line) - 1] == '\n')
        {
            line[strlen(line) - 1] = '\0';
        }

        // Split the line into words
        char *word = strtok(line, " ");
        int num_cols = 0;
        while (word != NULL && num_cols < Number_Of_Columns)
        {

            // Copy the word into the array
            strcpy(columns_words[num_cols][num_lines], word);
            num_words[num_lines]++;
            num_cols++;

            // Get the next word
            word = strtok(NULL, " ");
        }

        if (word == NULL)
        {
            while (num_cols < Number_Of_Columns)
            {
                strcpy(columns_words[num_cols][num_lines], "alright");
                num_words[num_lines]++;
                num_cols++;
            }
        }
        num_lines++;
    }
    fclose(file);

    // Create Child Sender Processes and pass them columns

    // Execl childs
    printf("Creating %d child processes to handle columns encryption process.\n", Number_Of_Columns);
    printf("----------------------------------------------------------------\n\n");
    for (int i = 0; i < Number_Of_Columns; i++)
    {
        pid_t pid = fork();

        if (pid < 0)
        {
            printf("Fork failed.\n");
            return 1;
        }
        else if (pid == 0)
        {
            sleep(2);
            //  Contacinate words from different rows in same column to token string.
            for (int m = 0; m < Number_Of_Rows; m++)
            {
                strcat(token, columns_words[i][m]);
                strcat(token, " ");
            }
            // Child process
            // printf("Child process %d with PID %d\n", i + 1, getpid());
            // printf("Column %d : %s\n", i + 1, token);
            sprintf(pid_str[i], "%d", pid); // from pid_t to string
            snprintf(Column_Number, sizeof(Column_Number), "%d", i);
            execl("./enc", "enc", pid_str[i], token, Column_Number, NULL);
            perror("execl");
            memset(token, '\0', buffer);
            exit(0);
        }
        else
        {
            // Parent process
            processes[i] = pid;
        }
        sleep(sleep_time * 0.3);
        sleep_time += 0.125;
    }
    free(token);

    struct sigaction sap;
    sap.sa_handler = &handle_sigusrp;
    sap.sa_flags = SA_RESTART;
    sigaction(SIGUSR1, &sap, NULL);
    sleep(1);

    while (counter < Number_Of_Columns)
    {
    }

    if (counter == Number_Of_Columns)
    {
        printf("-------------------------------------------------------------\n\n");
        printf("%d Signals Received By Sender Process.\n", Number_Of_Columns);
        for (int r = 0; r < Number_Of_Columns; r++)
        {
            kill(processes[r], SIGKILL);
        }
        printf("Column Carrier childs all killed!\n");

        // sleep(2);
        printf("\n\nShared Memory Contents[in Sender]:\n");
        for (int i = 0; i < Number_Of_Columns; i++)
        {
            printf("Location[%d]: %s\n", i + 1, data->value[i]);
        }
        printf("---------------------------------------------------------\n\n");
        kill(getppid(), SIGUSR2);
    }
    // Detach and destroy shared memory

    shmdt(data);
    // shmctl(shmid, IPC_RMID, NULL);

    return 0;
}

void Clean_Sender_Spaces()
{
    FILE *inputFile, *outputFile;
    char character;

    // Open the input file
    inputFile = fopen("Sender.txt", "r");
    if (inputFile == NULL)
    {
        printf("Failed to open input file.\n");
        exit(1);
    }

    // Open the output file
    outputFile = fopen("temp.txt", "w");
    if (outputFile == NULL)
    {
        printf("Failed to create output file.\n");
        fclose(inputFile);
        exit(1);
    }

    // Copy characters from input to output, removing extra spaces
    int previousChar = ' ';
    while ((character = fgetc(inputFile)) != EOF)
    {
        if (character != ' ' || previousChar != ' ')
        {
            fputc(character, outputFile);
        }
        previousChar = character;
    }

    // Close the files
    fclose(inputFile);
    fclose(outputFile);
    remove("Sender.txt");
    rename("temp.txt", "Sender.txt");
    printf("Over-Spaces removed successfully.\n");
    printf("---------------------------------\n\n");
}

int Get_Longest_Line()
{
    FILE *inputFile = fopen("Sender.txt", "r");

    if (inputFile == NULL)
    {
        printf("Error opening the input file.\n");
        return 1;
    }

    char line[MAX_LENGTH];
    char largestLine[MAX_LENGTH] = "";
    int largestWordCount = 0;

    while (fgets(line, sizeof(line), inputFile))
    {
        int lineLength = strlen(line);

        if (lineLength > 0 && line[lineLength - 1] == '\n')
        {
            line[lineLength - 1] = '\0'; // Remove the newline character
            lineLength--;
        }

        // Remove spaces from the line
        int wordCount = 0;
        char *token = strtok(line, " ");
        while (token != NULL)
        {
            wordCount++;
            token = strtok(NULL, " ");
        }

        if (wordCount > largestWordCount)
        {
            largestWordCount = wordCount;
            strcpy(largestLine, line);
        }
    }

    // Print the largest line and split it into separate columns
    // printf("largest line is: %s\n\n", largestLine);

    if (largestWordCount > 0)
    {
        char *token = strtok(largestLine, " ");
        while (token != NULL)
        {
            token = strtok(NULL, " ");
        }
    }
    fclose(inputFile);

    return largestWordCount;
}

int Get_Number_Of_Lines()
{
    char *filename = "Sender.txt";
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Error opening the file.\n");
        return -1;
    }

    int lineCount = 1; // Initialize lineCount to 0
    int ch;

    // Check if the file is empty
    if (fgetc(file) == EOF)
    {
        fclose(file);
        return 0;
    }

    // Reset the file position indicator to the beginning of the file
    rewind(file);

    while ((ch = fgetc(file)) != EOF)
    {
        if (ch == '\n')
        {
            lineCount++;
        }
    }

    fclose(file);
    return lineCount;
}

//