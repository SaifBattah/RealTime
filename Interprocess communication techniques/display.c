#include <stdio.h>
#include <GL/glut.h>
#include <GL/freeglut.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "shared.h"
#include <math.h>

#define MAX_VALUES 100

char middleData[MAX_VALUES][100000];


void drawText(char *text, float x, float y, float size) {
    glRasterPos2f(x, y);

    for (int i = 0; text[i] != '\0'; ++i) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
    }
}

void renderText(float x, float y, void *font, char *string) {
    glRasterPos2f(x, y);

    int len = strlen(string);
    for (int i = 0; i < len; i++) {
        glutBitmapCharacter(font, string[i]);
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    renderText(-0.1f, 0.9f, GLUT_BITMAP_HELVETICA_18, "Shared Memory");
    float x = -0.2f, y = 0.85f, spacing = 0.05f;
    // Draw the middle containers and text
    for (int i = 0; i < 17; i++) {
        // Calculate the position of the current container

        // Set the color of the container
        glColor3f(0.0f, 1.0f, 0.0f);

        // Draw the container
        renderText(x, y, GLUT_BITMAP_HELVETICA_18, middleData[i]);
        y -= spacing;
    }


    glutSwapBuffers();
    glFlush();
}


void updateData() {
    key_t key_sm = 1234; // create unique key
    int shmid = shmget(key_sm, sizeof(struct shared_data), 0666 | IPC_CREAT);
    struct shared_data *data = (struct shared_data *) shmat(shmid, NULL, 0);

    for (int i = 0; i < 17; i++) {
        strcpy(middleData[i], data->value[i]);
    }
    shmdt(data);

}

void timer(int) {
    updateData();

    // Redraw the display
    glutPostRedisplay();

    // Call the timer function again after a certain interval
    glutTimerFunc(1000 / 60, timer, 0);
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(1920, 1080);
    glutCreateWindow("Shared Memory User Interface");

    // Set the display function
    glutDisplayFunc(display);

    // Set the clear color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Start the timer to continuously update the data
    glutTimerFunc(0, timer, 0);

    // Start the GLUT main loop
    glutMainLoop();

    return 0;
}