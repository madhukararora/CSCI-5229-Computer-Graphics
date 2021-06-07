/* CSCI 5229 Computer Graphics
* University of Colorado Boulder
* @author : Madhukar Arora
* HW1
*/

/* Disclaimer : Most of the code is not my own work but referred from the example codes provided
   by the instructor in the class.*/


/* Includes */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>

#ifdef USEGLEW
#include <GL/glew.h>
#endif
//  OpenGL with prototypes for glext
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// Default resolution 
// For Retina Displays compile with -DRES=2
#ifndef RES
#define RES 1
#endif

//Globals
int th = 0;      //Azimuth of view angle
int ph = 0;      //Elevation of view angle
int zh = 0;
int mode = 1;    //Dimension (1-4)
double z = 0;    //Z variable
double w = 1;    //W variable
double dim = 3.0;   //Dimension of orthogonal box
const char* text[] = {"","2D","3D constant Z","3D","4D"};  //Dimension display text


// Lorenz Parameters
#define NUM_STEPS  (50000) //number of points
double s = 10;
double b = 2.6666;
double r = 28;

float coordinates[NUM_STEPS][3]; // to store 50000 XYZ coordinates

int redGradient = 100, greenGradient = 100, blueGradient = 100;

#define LEN 8192  //  Maximum amount of text
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



/* convenience function to draw the X-Y-Z axes*/
void drawXYZ(void)
{
    glPointSize(10);
    glColor3f(1.0,1.0,1.0); //color - white
    glBegin(GL_LINES);
    glVertex3d(0,0,0);
    glVertex3d(1,0,0); //X-axis
    glVertex3d(0,0,0);
    glVertex3d(0,1,0); //Y-axis
    glVertex3d(0,0,0);
    glVertex3d(0,0,1); //Z-axis
    glEnd();

    //Label Axes
    glRasterPos3d(1,0,0);
    Print("X");
    glRasterPos3d(0,1,0);
    Print("Y");
    glRasterPos3d(0,0,1);
    Print("Z");
    
    /* update display */
    glutPostRedisplay();
}


/* convenience function to store Lorenz Points */
void generateLorenzPoints(void)
{
    double x = 1.0;
    double y = 1.0;
    double z = 1.0; 

    double dt = 0.001; //time step

    for(int i = 0; i < NUM_STEPS; i++)
    {
        double dx = s * (y-x);
        double dy = x * (r-z)-y;
        double dz = x*y - b*z;

        x += (dt*dx);
        y += (dt*dy);
        z += (dt*dz);
        //printf("%d %lf %lf %lf\n",i,x,y,z);
        /*scaling down by a factor of 100*/
        coordinates[i][0] = x*0.01;
        coordinates[i][1] = y*0.01;
        coordinates[i][2] = z*0.01;
    }

}


/* function to generate random number between a range*/
// Referred Geeks for Geeks code
// using it to generate R Y B values


int returnRandoms(int low,int high)
{
    if((low == 0) && (high == 0))
        return 0;
    int num = (rand() % (high-low+1))+ low;
    //printf("num : %d\n",num);
    return num;
    
}

/* convenience function to generate the Lorenz Graph*/
void generateLorenzGraph(void)
{
    
    /* generate coordinates for Lorenz Graph*/
    generateLorenzPoints();
    glPointSize(5);
    glBegin(GL_LINE_STRIP); //connected vertices
   for (int i=0;i<NUM_STEPS;i++)
  	{
        /*For adding shades of color*/
		//printf("%d %d %d\n",redGradient,blueGradient,greenGradient);
        glColor3ub(redGradient,greenGradient,blueGradient);
		glVertex3f(coordinates[i][0],coordinates[i][1],coordinates[i][2]);
        glVertex3fv(coordinates[i]);
  	}
    glEnd();
}






/* function called by GLUT to display the scene */
void display(void)
{
    /* clear screen and Z-Buffer*/
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* reset transformation - clears changes made earlier */
    glLoadIdentity();

    /* rotation about X axis */
    glRotated(ph,1,0,0);
    
    /* rotation about Y axis */
    glRotated(th,0,1,0);

    /* draw the axis */ 
    drawXYZ();

    /* generate the Lorenz Curve */
    generateLorenzGraph();

    /* print to window rotation angle */
    glColor3ub(redGradient,greenGradient,blueGradient);
    glWindowPos2i(5,5);
    Print("view angle %d %d, b = %f, r = %f, s = %f",th,ph,b,r,s);

    glWindowPos2i(5,30);
    Print("S to increase, s to decrease, B to increase, b to decrease, R to increase, r to decrease, D to go to default");

    glWindowPos2i(5,60);
    Print("Arrow keys to change view angle about X and Y axis");

    glWindowPos2i(5,90);
    Print("F1 to increase Red color, F2 to increase Green color, F3 to increase Blue color");
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
    switch(key)
    {
        case GLUT_KEY_RIGHT:
            th = (th + 5) % 360;
            break;
        case GLUT_KEY_LEFT:
            th = (th - 5) % 360;
            break;
        case GLUT_KEY_UP:
            ph = (ph + 5) % 360;
            break;
        case GLUT_KEY_DOWN:
            ph = (ph - 5) % 360;
            break;   
        case GLUT_KEY_F1:
            
            redGradient += 25;
            if(redGradient >= 255)
                redGradient = 255;
            
            greenGradient -= 5;
            if(greenGradient <= 0)
                greenGradient = 0;
                
            blueGradient -= 5;
            if(blueGradient <= 0)
                blueGradient = 0;
            break; 

        case GLUT_KEY_F2:
            
            greenGradient += 25;
            if(greenGradient >= 255)
                greenGradient = 255;
            
            redGradient -= 5;
            if(redGradient <= 0)
                redGradient = 0;
                
            blueGradient -= 5;
            if(blueGradient <= 0)
                blueGradient = 0;
            break; 
        case GLUT_KEY_F3:
            
            blueGradient += 25;
            if(blueGradient >= 255)
                blueGradient = 255;
            
            greenGradient -= 5;
            if(greenGradient <= 0)
                greenGradient = 0;
                
            redGradient -= 5;
            if(redGradient <= 0)
                redGradient = 0;
            break; 
        default:
            break;
    }

    /* update the display */
    glutPostRedisplay();
}

void key(unsigned char ch,int x, int y)
{
    switch(ch)
    {
        //ESC key
        case(27):
            exit(0);
            break;
        //Default
        case('D'):
        case('d'):
           s = 10;
           b = 2.6666;
           r = 28;
           th = 0;      //Azimuth of view angle
           ph = 0;      //Elevation of view angle
           break;
        case('S'):
            s = s+1;
            break;
        case('s'):
            s = s-1;
            break;
        case('B'):
            b = b+1;
            break;
        case('b'):
            b = b-1;
            break;
        case('R'):
            r = r+1;
            break;
        case('r'):
            r = r-1;
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

    /* Request 500 x 500 pixel window */
    glutInitWindowSize(500,500);
    /* Create Window */
    glutCreateWindow("Madhukar's Lorenz Attractor");

#ifdef USEGLEW
   //  Initialize GLEW
   if (glewInit()!=GLEW_OK) Fatal("Error initializing GLEW\n");
#endif
    /* Register function used to 
       display scene 
       window resizing
       idle function
       handle Special key presses
       handle keyboard key presses
    */
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);
    glutSpecialFunc(special);
    glutKeyboardFunc(key);
    /* enable z-buffer depth test */
    glEnable(GL_DEPTH_TEST);

    /* pass control for GLUT for events */
    glutMainLoop();

    /* return to OS */
    return 0;

}
