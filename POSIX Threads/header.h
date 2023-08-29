#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <GL/glut.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#define MAX_FOOD_BLOCKS 3

#define FOOD_PIECES 500
#define WINDOW_WIDTH 1800
#define WINDOW_HEIGHT 1000
#define FPS 60
#define MAX_LINE_LENGTH 100
#define MAX_ANTS 1000

// Initialize variables with default values
int Ants_Number;
int Food_Placement_Time;
int Food_Scent_Radius;
int Ant_Scent_Radius;
int Episode_Time;
int icounter = 0;
int FOOD_COUNT;
float Ant_Eating_Ratio;
time_t startTimer;

typedef struct
{
    float x;
    float y;
    float angle;
    float colorR;
    float colorG;
    float colorB;
    int speed;
    int targetFoodIndex;    // Index of the food being targeted
    int eating;             // Flag to indicate if the ant is currently eating
    int lastFoodBlockIndex; // Index of the last food block from which the ant was eating
    struct
    {
        int flag;
        float centerX;
        float centerY;
        float radius;
        float colorR;
        float colorG;
        float colorB;
    } wave; // Circular wave created by the ant when eating
} Ant;

typedef struct
{
    float x;
    float y;
    float size;
    float piecesRemaining;                // Remaining pieces of the food block
    int beingEaten;                       // Flag to indicate if the food block is currently being eaten
    float piecePositions[FOOD_PIECES][2]; // Positions of each piece of food
    pthread_mutex_t mutex;                // Mutex to control access to the food block
} FoodBlock;

Ant ants[MAX_ANTS];
FoodBlock foodBlocks[100];

pthread_t antThreads[MAX_ANTS];
int antIndices[MAX_ANTS];

void *antThread(void *arg);

void initializeAnts();

void initializeFoodBlocks();

void display();

void reshape(int width, int height);

void timer(int value);

void processLine();

void closeDisplay();
