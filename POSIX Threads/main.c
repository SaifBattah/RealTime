#include "header.h"

void *antThread(void *arg)
{
    int antIndex = *(int *)arg;
    free(arg);

    Ant *ant = &ants[antIndex];

    while (1)
    {
        if (ant->eating)
        {
            int foodIndex = ant->targetFoodIndex;
            FoodBlock *foodBlock = &foodBlocks[foodIndex];

            while (foodBlock->piecesRemaining >= 1)
            {
                sleep(1);
                float eat_just_this_amount = (FOOD_PIECES * Ant_Eating_Ratio) / 100;

                printf("EAT = %f\n", eat_just_this_amount);
                pthread_mutex_lock(&foodBlock->mutex);

                if (eat_just_this_amount < foodBlock->piecesRemaining)
                {
                    foodBlock->piecesRemaining -= eat_just_this_amount;
                }
                else
                {
                    foodBlock->piecesRemaining = 0;
                }
                pthread_mutex_unlock(&foodBlock->mutex);
                printf("Remaining pieces[%d]: %f\n", foodIndex, foodBlock->piecesRemaining);

                if (foodBlock->piecesRemaining < 1)
                {
                    // Food block is empty, reset eating flags
                    // and allow ant to move in a random direction
                    ant->wave.flag = 0;
                    ant->wave.radius = Ant_Scent_Radius;
                    foodBlock->piecesRemaining = 0;
                    foodBlock->beingEaten = 0;
                    ant->targetFoodIndex = -1;
                    ant->eating = 0;
                    ant->angle = rand() % 360; // Set a new random direction
                }
            }

            if (foodBlock->piecesRemaining < 1)
            {
                ant->wave.flag = 0;
                ant->wave.radius = Ant_Scent_Radius;
                foodBlock->piecesRemaining = 0;
                foodBlock->beingEaten = 0;
                ant->targetFoodIndex = -1;
                ant->eating = 0;
                ant->angle = rand() % 360; // Set a new random direction
            }
        }
        else
        {
            float prevX = ant->x;
            float prevY = ant->y;
            ant->x += ant->speed * cos(ant->angle * (M_PI / 180));
            ant->y += ant->speed * sin(ant->angle * (M_PI / 180));

            // Check if ant hits the simulation window limit
            if (ant->x >= WINDOW_WIDTH)
            {
                ant->x = WINDOW_WIDTH;
                ant->angle += 45;
                ant->angle = fmod(ant->angle, 360);
            }
            else if (ant->x <= 0)
            {
                ant->x = 0;
                ant->angle += 45;
                ant->angle = fmod(ant->angle, 360);
            }
            if (ant->y >= WINDOW_HEIGHT)
            {
                ant->y = WINDOW_HEIGHT;
                ant->angle += 45;
                ant->angle = fmod(ant->angle, 360);
            }
            else if (ant->y <= 0)
            {
                ant->y = 0;
                ant->angle += 45;
                ant->angle = fmod(ant->angle, 360);
            }

            // Check if the ant was moving in a straight line before hitting the limit -> prevent rotating
            if (prevX != ant->x || prevY != ant->y)
            {
                if (rand() % 100 == 0)
                {
                    ant->angle += (rand() % 91) - 45;
                    ant->angle = fmod(ant->angle, 360);
                }
            }

            // Check ants position Stats by the food circle
            for (int i = 0; i < FOOD_COUNT; i++)
            {
                // Check if ant within food circle
                if (sqrt(pow(foodBlocks[i].x - ant->x, 2) + pow(foodBlocks[i].y - ant->y, 2)) <= Food_Scent_Radius)
                {
                    // Update the movement direction of the ant within the Food circular wave
                    if (foodBlocks[i].piecesRemaining > 0)
                    {
                        float distance = sqrt(pow(ant->x - foodBlocks[i].x, 2) +
                                              pow(ant->y - foodBlocks[i].y, 2));
                        if (distance <= Food_Scent_Radius)
                        {
                            ants[i].wave.colorR = 1.0;
                            ants[i].wave.colorG = 1.0;
                            ants[i].wave.colorB = 1.0;
                            ant->wave.centerX = ant->x;
                            ant->wave.centerY = ant->y;
                            ant->wave.radius += (Ant_Scent_Radius + Food_Scent_Radius) / (ant->speed + distance);
                            ant->wave.flag = 1;
                            float dx = foodBlocks[i].x - ant->x;
                            float dy = foodBlocks[i].y - ant->y;
                            ant->angle = atan2(dy, dx) * (180 / M_PI);
                        }
                    }
                    else
                    {
                        ant->wave.flag = 0;
                        ant->wave.radius = Ant_Scent_Radius;
                    }
                }

                // Check if ant within ant circle
                if (!foodBlocks[i].beingEaten && fabs(ant->x - foodBlocks[i].x) < 10 &&
                    fabs(ant->y - foodBlocks[i].y) < 10)
                {
                    if (pthread_mutex_trylock(&foodBlocks[i].mutex) == 0)
                    {
                        if (!foodBlocks[i].beingEaten && foodBlocks[i].piecesRemaining > 0)
                        {
                            ant->eating = 1;
                            ant->targetFoodIndex = i;
                        }
                        pthread_mutex_unlock(&foodBlocks[i].mutex);
                    }
                }
            }

            // Check ant-ant interactions
            for (int i = 0; i < Ants_Number; i++)
            {
                if (i != antIndex)
                {
                    Ant *otherAnt = &ants[i];
                    if (otherAnt->wave.flag == 1)
                    {
                        float distance = sqrt(pow(ant->x - otherAnt->wave.centerX, 2) +
                                              pow(ant->y - otherAnt->wave.centerY, 2));
                        if (distance <= otherAnt->wave.radius)
                        {
                            // Ant entered another ant's wave circle
                            if (ant->eating && ant->targetFoodIndex == otherAnt->targetFoodIndex)
                            {
                                // Both ants are targeting the same food block
                                if (pthread_mutex_trylock(&foodBlocks[ant->targetFoodIndex].mutex) == 0)
                                {
                                    // Start eating together
                                    ant->wave.flag = 0;
                                    otherAnt->eating = 1;
                                    otherAnt->targetFoodIndex = ant->targetFoodIndex;
                                    pthread_mutex_unlock(&foodBlocks[ant->targetFoodIndex].mutex);
                                }
                            }
                        }
                    }
                }
            }
        }

        usleep(1000000 / FPS);
    }

    return NULL;
}

void initializeAnts()
{
    srand(time(NULL));
    int i;
    for (i = 0; i < Ants_Number; i++)
    {
        ants[i].x = rand() % WINDOW_WIDTH;
        ants[i].y = rand() % WINDOW_HEIGHT;
        ants[i].angle = rand() % 360;
        ants[i].colorR = 0 / 255;
        ants[i].colorG = 0 / 255;
        ants[i].colorB = 255.0 / 255;
        ants[i].targetFoodIndex = -1;
        ants[i].eating = 0;
        ants[i].speed = (rand() % 10) + 1.0;
        ants[i].wave.radius = Ant_Scent_Radius;
        ants[i].wave.colorR = 255.0;
        ants[i].wave.colorG = 1.0;
        ants[i].wave.colorB = 1.0;
        ants[i].wave.flag = 0;
        int *antIndex = malloc(sizeof(int));
        *antIndex = i;
        antIndices[i] = *antIndex;

        if (pthread_create(&antThreads[i], NULL, antThread, antIndex) != 0)
        {
            printf("Error Creating Thread!\n");
            exit(0);
        }
        printf("Thread %d has Started!\n", i + 1);
    }
}

void initializeFoodBlocks()
{
    srand(time(NULL));
    int numFoodBlocks = 1 + (rand() % (MAX_FOOD_BLOCKS));

    int j;
    int existingFoodBlocks = 0;
    for (icounter; existingFoodBlocks < numFoodBlocks; icounter++)
    {
        if (foodBlocks[icounter].piecesRemaining <= 0)
        {
            foodBlocks[icounter].x = rand() % WINDOW_WIDTH;
            foodBlocks[icounter].y = rand() % WINDOW_HEIGHT;
            foodBlocks[icounter].size = 4;
            foodBlocks[icounter].piecesRemaining = FOOD_PIECES;
            foodBlocks[icounter].beingEaten = 0;

            for (j = 0; j < FOOD_PIECES; j++)
            {
                foodBlocks[icounter].piecePositions[j][0] = foodBlocks[icounter].x + ((rand() % 9) - 4);
                foodBlocks[icounter].piecePositions[j][1] = foodBlocks[icounter].y + ((rand() % 9) - 4);
            }

            pthread_mutex_init(&foodBlocks[icounter].mutex, NULL);

            existingFoodBlocks++;
        }
    }
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw borderline
    glColor3f(1.0, 1.0, 1.0);
    glLineWidth(2.0);
    glBegin(GL_LINE_LOOP);
    glVertex2f(0, 0);
    glVertex2f(WINDOW_WIDTH, 0);
    glVertex2f(WINDOW_WIDTH, WINDOW_HEIGHT);
    glVertex2f(0, WINDOW_HEIGHT);
    glEnd();

    // Draw ants and their circular waves
    for (int i = 0; i < Ants_Number; i++)
    {
        Ant *ant = &ants[i];

        // Draw ants
        glColor3f(ant->colorR, ant->colorG, ant->colorB);
        glPushMatrix();
        glTranslatef(ant->x, ant->y, 0);
        glRotatef(ant->angle, 0, 0, 1);
        glBegin(GL_POLYGON);
        glVertex2f(-3, 0);
        glVertex2f(-1, 0);
        glVertex2f(1, 2);
        glVertex2f(3, 2);
        glVertex2f(1, -2);
        glVertex2f(-1, -2);
        glEnd();
        glPopMatrix();

        // Draw circular wave if ant entered food circle or another ant's wave circle
        if (ant->wave.flag == 1)
        {
            if (ant->eating)
            {
                // Color for ants eating together
                glColor3f(0.0, 1.0, 0.0); // Green color
            }
            else
            {
                // Color for ants near food blocks
                glColor3f(ant->wave.colorR, ant->wave.colorG, ant->wave.colorB);
            }
            glPushMatrix();
            glTranslatef(ant->wave.centerX, ant->wave.centerY, 0);
            glBegin(GL_LINE_LOOP);
            float radius = ant->wave.radius;
            for (int angle = 0; angle < 360; angle++)
            {
                float radian = angle * (M_PI / 180);
                float x = radius * cos(radian);
                float y = radius * sin(radian);
                glVertex2f(x, y);
            }
            glEnd();
            glPopMatrix();
        }
    }

    // Draw food blocks
    for (int i = 0; i < FOOD_COUNT; i++)
    {
        if (foodBlocks[i].piecesRemaining > 0)
        {
            glColor3f(0.8, 0.4, 0.4);
            for (int j = 0; j < FOOD_PIECES; j++)
            {
                if (j < foodBlocks[i].piecesRemaining)
                {
                    glPushMatrix();
                    glTranslatef(foodBlocks[i].piecePositions[j][0], foodBlocks[i].piecePositions[j][1], 0);
                    glBegin(GL_POLYGON);
                    glVertex2f(-foodBlocks[i].size, -foodBlocks[i].size);
                    glVertex2f(foodBlocks[i].size, -foodBlocks[i].size);
                    glVertex2f(foodBlocks[i].size, foodBlocks[i].size);
                    glVertex2f(-foodBlocks[i].size, foodBlocks[i].size);
                    glEnd();
                    glPopMatrix();
                }
            }

            // Draw the circle around the food block
            glColor3f(0.8, 0.4, 0.4);
            glPushMatrix();
            glTranslatef(foodBlocks[i].x, foodBlocks[i].y, 0);
            glBegin(GL_LINE_LOOP);
            for (int angle = 0; angle < 360; angle++)
            {
                float radian = angle * (M_PI / 180);
                float x = Food_Scent_Radius * cos(radian);
                float y = Food_Scent_Radius * sin(radian);
                glVertex2f(x, y);
            }
            glEnd();
            glPopMatrix();
        }
    }

    glutSwapBuffers();
    glutPostRedisplay();
}

void reshape(int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, width, height, 0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void timer(int value)
{
    initializeFoodBlocks();
    // Episode_Time
    if (time(NULL) - startTimer < Episode_Time * 60)
        glutTimerFunc(Food_Placement_Time * 1000, timer, 0);
    else
        glutTimerFunc(0, closeDisplay, 0);
}

void processLine()
{
    char line[MAX_LINE_LENGTH];
    char variablename[50];
    char value[50];

    FILE *file = fopen("VALUES.txt", "r");
    if (file == NULL)
    {
        printf("Failed to open the file.\n");
        return;
    }

    while (fgets(line, sizeof(line), file))
    {

        // Splitting the line
        char *token = strtok(line, "=");
        if (token != NULL)
        {
            strncpy(variablename, token, sizeof(variablename));
            variablename[strlen(variablename) - 1] = '\0';
            token = strtok(NULL, "=");
            if (token != NULL)
            {
                strncpy(value, token, sizeof(value));

                if (strcmp(variablename, "Ants_Number") == 0)
                {
                    Ants_Number = atoi(value);
                }
                else if (strcmp(variablename, "Food_Placement_Time") == 0)
                {
                    Food_Placement_Time = atoi(value);
                }
                else if (strcmp(variablename, "Food_Scent_Radius") == 0)
                {
                    Food_Scent_Radius = atoi(value);
                }
                else if (strcmp(variablename, "Ant_Scent_Radius") == 0)
                {
                    Ant_Scent_Radius = atoi(value);
                }
                else if (strcmp(variablename, "Ant_Eating_Ratio") == 0)
                {
                    Ant_Eating_Ratio = atoi(value);
                }
                else if (strcmp(variablename, "Episode_Time") == 0)
                {
                    Episode_Time = atoi(value);
                }
                else
                {
                    printf("FAIL\n");
                }
            }
        }
    }
    fclose(file);

    // Print the initialized values
    printf("Ants_Number: %d\n", Ants_Number);
    printf("Food_Placement_Time: %d\n", Food_Placement_Time);
    printf("Food_Scent_Radius: %d\n", Food_Scent_Radius);
    printf("Ant_Scent_Radius: %d\n", Ant_Scent_Radius);
    printf("Ant_Eating_Ratio: %f\n", Ant_Eating_Ratio);
    printf("Episode_Time: %d\n", Episode_Time);
}

void closeDisplay()
{
    exit(0);
}

int main(int argc, char **argv)
{
    processLine();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Ants");

    initializeAnts();

    startTimer = time(NULL);

    // initializeFoodBlocks();
    FOOD_COUNT = ((Episode_Time * 60) / Food_Placement_Time);
    // initializeFoodBlocks();

    glClearColor(0, 0, 0, 1);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    glutTimerFunc(Food_Placement_Time * 1000, timer, 0);

    glutMainLoop();
    return 0;
}
