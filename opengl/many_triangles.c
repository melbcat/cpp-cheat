#include <stdlib.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

/* To see anythingwe need at least a few pixels per triangle. */
#define HOW_MANY 200
/* Increase this to stress your GPU. */
#define REPEAT 10

static void init(void) {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);
}

static void display(void) {
    int i;
    int j;
    int repeats;
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    glColor3f(1.0f, 0.0f, 0.0f);
    for (repeats = 0; repeats < REPEAT; repeats++) {
        for (i = 0; i < HOW_MANY; i++) {
            for (j = 0; j < HOW_MANY; j++) {
                glPushMatrix();
                glTranslatef(2*i, 2*j, 0);
                glBegin(GL_TRIANGLES);
                glVertex3f( 0.0f, 1.0f, 0.0f);
                glVertex3f(-1.0f, -1.0f, 0.0f);
                glVertex3f( 1.0f, -1.0f, 0.0f);
                glEnd();
                glPopMatrix();
                glEnd();
            }
        }
    }
    glFlush();
}

static void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, HOW_MANY + 1, -1.0, HOW_MANY + 1, -1.5, 1.5);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(1000, 1000);
    glutInitWindowPosition(0, 0);
    glutCreateWindow(argv[0]);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMainLoop();
    return EXIT_SUCCESS;
}
