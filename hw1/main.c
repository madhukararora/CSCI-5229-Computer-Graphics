/* CSCI 5229 Computer Graphics
*  University of Colorado Boulder
* @author : Madhukar Arora
* HW1
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif


#define LEN 8192  //  Maximum amount of text
double th = 0; // rotation angle
double zh = 0; //spin angle
#ifndef RES
#define RES 1
#endif



/* function for text */
void Print(const char* format , ...)
{
   char    buf[LEN]; // Text storage
   char*   ch=buf;   // Text pointer
   //  Create text to be display
   va_list args;
   va_start(args,format);
   vsnprintf(buf,LEN,format,args);
   va_end(args);
   //  Display text string
   while (*ch)
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*ch++);
}

/* prints message to stderr and exit */
void Fatal(const char* format, ...)
{
   va_list args;
   va_start(args,format);
   vfprintf(stderr,format,args);
   va_end(args);
   exit(1);
}

/* function to print any errors encountered */
void ErrCheck(char* where)
{
    int err = glGetError();
    if(err)
    {
        fprintf(stderr,"ERROR: %s [%s]\n",gluErrorString(err),where);
    }
}

/* function called by GLUT to display the scene */
void display(void)
{
    /* clear screen and Z-Buffer*/
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* reset transformation */
    glLoadIdentity();

   

     /* rotate around Y axis */
    glRotatef(th,0.0,1.0,0.0);

    /* draw triangle - RGB*/
    glBegin(GL_POLYGON);
    glColor3f(1.0,0.0,0.0);
    glVertex2f(0.0,0.5);
    glColor3f(0.0,1.0,0.0);
    glVertex2f(0.5,0.5);
    glColor3f(0.0,0.0,1.0);
    glVertex2f(-0.5,-0.5);
    glEnd();


     /* rotate around Z axis */
    glRotatef(zh,0.0,0.0,1.0);
    /* offset second triangle */
    glTranslatef(0.2,0.2,0.2);

    /* draw second triangle*/
    glBegin(GL_POLYGON);
    glColor3f(1.0,1.0,0.0);
    glVertex2f(0.0,0.5);
    glColor3f(0.0,1.0,1.0);
    glVertex2f(0.5,0.5);
    glColor3f(1.0,0.0,1.0);
    glVertex2f(-0.5,-0.5);
    glEnd();

    /* print to window rotation angle */
    glColor3f(1,1,1);
    glWindowPos2i(5,5);
    Print("Angle = %.1f",th);

    /* Sanity check */
    ErrCheck("display"); 
    /* make scene visible */
    glFlush();

    /* swap double buffer */
    glutSwapBuffers();

}

/* function called by GLUT when special keys are pressed*/
void special(int key,int x, int y)
{
    switch (key)
    {
        case GLUT_KEY_RIGHT:
            th += 5;
            break;
        case GLUT_KEY_LEFT:
            th -= 5;
            break;
        default:
            break;
    }

    /* update the display */
    glutPostRedisplay();
}


/* function called by GLUT when window is resized */
void reshape(int width,int height)
{
   //  Set viewport as entire window
   glViewport(0,0, RES*width,RES*height);
   //  Select projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Set projection to identity
   glLoadIdentity();
   //  Orthogonal projection:  unit cube adjusted for aspect ratio
   double asp = (height>0) ? (double)width/height : 1;
   glOrtho(-asp,+asp, -1.0,+1.0, -1.0,+1.0);
   //  Select model view matrix
   glMatrixMode(GL_MODELVIEW);
   //  Set model view to identity
   glLoadIdentity();
}
 
/* function called by GLUT when idle */
void idle(void)
{
    /* get elapsed time in seconds */
    double t = glutGet(GLUT_ELAPSED_TIME)/1000.0;

    /* spin angle - 90 degrees/second */
    zh = fmod(90*t,360);

    /* update display */
    glutPostRedisplay();
}

int main(int argc,char* argv[])
{
    /* initialize OpenGL Utility Tool */
    glutInit(&argc,argv);

    /* Request double bufferred true color window */
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH); //GLUT_DEPTH adds z-buffer

    /* Create Window */
    glutCreateWindow("Hello World");

    /* Register function used to 
       display scene 
       window resizing
       idle function
       handle key presses
    */
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);
    glutSpecialFunc(special);

    /* enable z-buffer depth test */
    glEnable(GL_DEPTH_TEST);

    /* pass control for GLUT for events */
    glutMainLoop();

    /* return to OS */
    return 0;

}
