/* CSCI 5229 Computer Graphics
*  University of Colorado Boulder
* @author : Madhukar Arora
* HW1
*/
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif


/* function called by GLUT to display the scene */
void display(void)
{
    /* clear screen */
    glClear(GL_COLOR_BUFFER_BIT);

    /* draw triangle */
    glBegin(GL_POLYGON);
    glVertex2f(0.0,0.5);
    glVertex2f(0.5,0.5);
    glVertex2f(-0.5,-0.5);
    glEnd();

    /* make scene visible */
    glFlush();

}

int main(int argc,char* argv[])
{
    /* initialize OpenGL Utility Tool */
    glutInit(&argc,argv);

    /* Create Window */
    glutCreateWindow("Hello World");

    /* Register function used to display scene */
    glutDisplayFunc(display);

    /* pass control for GLUT for events */
    glutMainLoop();

    /* return to OS */
    return 0;

}
