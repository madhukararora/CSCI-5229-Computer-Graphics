/* CSCI 5229 Computer Graphics
* University of Colorado Boulder
* @author : Madhukar Arora
* HW2
*/

/* Disclaimer : Most of the code is not my own work but referred from the example codes provided
   by the instructor in the class.

*/


/* Includes */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <stdbool.h>
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
//  Default resolution
//  For Retina displays compile with -DRES=2
#ifndef RES
#define RES 1
#endif
//Globals

int th=0;          //  Azimuth of view angle
int ph=0;          //  Elevation of view angle
double zh=0;       //  Rotation of teapot
int axes=1;        //  Display axes
const char* text[] = {"Cuboids","Spheres","FlatPlane Outline","FlatPlane Fill","SolidPlane","Icosahedron DrawElements","Icosahedron DrawArrays","Icosahedron VBO","Scene"};

//  Cosine and Sine in degrees
#define Cos(x) (cos((x)*3.1415927/180))
#define Sin(x) (sin((x)*3.1415927/180))

typedef enum{
    CUBE = 0,
    SPHERE,
    FLATPLANE,
    AIRPLANE,
}shapes_t;

shapes_t mode = CUBE;

#define LEN 8192  //  Maximum amount of text
/* function for text */
void Print(const char* format , ...) //DO NOT MODIFY
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
void Fatal(const char* format, ...) //DO NOT MODIFY
{
   va_list args;
   va_start(args,format);
   vfprintf(stderr,format,args);
   va_end(args);
   exit(1);
}

/* function to print any errors encountered */
void ErrCheck(char* where) //DO NOT MODIFY
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



/*
 *  Draw a cube
 *     at (x,y,z)
 *     dimensions (dx,dy,dz)
 *     rotated th about the y axis
 */
static void cube(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th)
{
   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   //  Cube
   glBegin(GL_QUADS);
   //  Front
   glColor3f(1,0,0);
   glVertex3f(-1,-1, 1);
   glVertex3f(+1,-1, 1);
   glVertex3f(+1,+1, 1);
   glVertex3f(-1,+1, 1);
   //  Back
   glColor3f(0,0,1);
   glVertex3f(+1,-1,-1);
   glVertex3f(-1,-1,-1);
   glVertex3f(-1,+1,-1);
   glVertex3f(+1,+1,-1);
   //  Right
   glColor3f(1,1,0);
   glVertex3f(+1,-1,+1);
   glVertex3f(+1,-1,-1);
   glVertex3f(+1,+1,-1);
   glVertex3f(+1,+1,+1);
   //  Left
   glColor3f(0,1,0);
   glVertex3f(-1,-1,-1);
   glVertex3f(-1,-1,+1);
   glVertex3f(-1,+1,+1);
   glVertex3f(-1,+1,-1);
   //  Top
   glColor3f(0,1,1);
   glVertex3f(-1,+1,+1);
   glVertex3f(+1,+1,+1);
   glVertex3f(+1,+1,-1);
   glVertex3f(-1,+1,-1);
   //  Bottom
   glColor3f(1,0,1);
   glVertex3f(-1,-1,-1);
   glVertex3f(+1,-1,-1);
   glVertex3f(+1,-1,+1);
   glVertex3f(-1,-1,+1);
   //  End
   glEnd();
   //  Undo transformations
   glPopMatrix();
}

/*
 *  Draw vertex in polar coordinates
 */
static void Vertex(double th,double ph)
{
   glColor3f(Cos(th)*Cos(th) , Sin(ph)*Sin(ph) , Sin(th)*Sin(th));
   glVertex3d(Sin(th)*Cos(ph) , Sin(ph) , Cos(th)*Cos(ph));
}

/*
 *  Draw a sphere (version 1)
 *     at (x,y,z)
 *     radius (r)
 */
static void sphere1(double x,double y,double z,double r)
{
   const int d=15;

   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x,y,z);
   glScaled(r,r,r);

   //  South pole cap
   glBegin(GL_TRIANGLE_FAN);
   Vertex(0,-90);
   for (int th=0;th<=360;th+=d)
   {
      Vertex(th,d-90);
   }
   glEnd();

   //  Latitude bands
   for (int ph=d-90;ph<=90-2*d;ph+=d)
   {
      glBegin(GL_QUAD_STRIP);
      for (int th=0;th<=360;th+=d)
      {
         Vertex(th,ph);
         Vertex(th,ph+d);
      }
      glEnd();
   }

   //  North pole cap
   glBegin(GL_TRIANGLE_FAN);
   Vertex(0,90);
   for (int th=0;th<=360;th+=d)
   {
      Vertex(th,90-d);
   }
   glEnd();

   //  Undo transformations
   glPopMatrix();
}

/*
 *  Draw a sphere (version 2)
 *     at (x,y,z)
 *     radius (r)
 */
static void sphere2(double x,double y,double z,double r)
{
   const int d=15;

   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x,y,z);
   glScaled(r,r,r);

   //  Latitude bands
   for (int ph=-90;ph<90;ph+=d)
   {
      glBegin(GL_QUAD_STRIP);
      for (int th=0;th<=360;th+=d)
      {
         Vertex(th,ph);
         Vertex(th,ph+d);
      }
      glEnd();
   }

   //  Undo transformations
   glPopMatrix();
}

/*
 *  Draw a airplane shaped polygon at (x,y,z)
 */
static void FlatPlane(int type,double x,double y,double z)
{
   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   //  Fuselage and wings
   glColor3f(1,1,0); 
   glBegin(type);
   glVertex2f( 1.0, 0.0);
   glVertex2f( 0.8, 0.1);
   glVertex2f( 0.0, 0.1);
   glVertex2f(-1.0, 0.5);
   glVertex2f(-1.0,-0.5);
   glVertex2f( 0.0,-0.1);
   glVertex2f( 0.8,-0.1);
   glEnd();
   //  Vertical tail
   glColor3f(1,0,0);
   glBegin(type);
   glVertex3f(-1.0, 0.0,0.0);
   glVertex3f(-1.0, 0.0,0.5);
   glVertex3f(-0.5, 0.0,0.0);
   glEnd();
   //  Undo transformations
   glPopMatrix();
}

/*
 *  Draw solid airplane
 *    at (x,y,z)
 *    nose towards (dx,dy,dz)
 *    up towards (ux,uy,uz)
 */
static void SolidPlane(double x,double y,double z,
                       double dx,double dy,double dz,
                       double ux,double uy, double uz)
{
   // Dimensions used to size airplane
   const double wid=0.05;
   const double nose=+0.50;
   const double cone= 0.20;
   const double wing= 0.00;
   const double strk=-0.20;
   const double tail=-0.50;
   //  Unit vector in direction of flght
   double D0 = sqrt(dx*dx+dy*dy+dz*dz);
   double X0 = dx/D0;
   double Y0 = dy/D0;
   double Z0 = dz/D0;
   //  Unit vector in "up" direction
   double D1 = sqrt(ux*ux+uy*uy+uz*uz);
   double X1 = ux/D1;
   double Y1 = uy/D1;
   double Z1 = uz/D1;
   //  Cross product gives the third vector
   double X2 = Y0*Z1-Y1*Z0;
   double Y2 = Z0*X1-Z1*X0;
   double Z2 = X0*Y1-X1*Y0;
   //  Rotation matrix
   double mat[16];
   mat[0] = X0;   mat[4] = X1;   mat[ 8] = X2;   mat[12] = 0;
   mat[1] = Y0;   mat[5] = Y1;   mat[ 9] = Y2;   mat[13] = 0;
   mat[2] = Z0;   mat[6] = Z1;   mat[10] = Z2;   mat[14] = 0;
   mat[3] =  0;   mat[7] =  0;   mat[11] =  0;   mat[15] = 1;

   //  Save current transforms
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glMultMatrixd(mat);
   //  Nose
   glColor3f(0,0,1);
   glBegin(GL_TRIANGLE_FAN);
   glVertex3d(nose, 0.0, 0.0);
   for (int th=0;th<=360;th+=30)
      glVertex3d(cone,wid*Cos(th),wid*Sin(th));
   glEnd();
   //  Fuselage
   glBegin(GL_QUAD_STRIP);
   for (int th=0;th<=360;th+=30)
   {
      glVertex3d(cone,wid*Cos(th),wid*Sin(th));
      glVertex3d(tail,wid*Cos(th),wid*Sin(th));
   }
   glEnd();
   // Tailpipe
   glBegin(GL_TRIANGLE_FAN);
   glVertex3d(tail, 0.0, 0.0);
   for (int th=0;th<=360;th+=30)
      glVertex3d(tail,wid*Cos(th),wid*Sin(th));
   glEnd();
   //  Wings
   glColor3f(1,1,0);
   glBegin(GL_TRIANGLES);
   glVertex3d(wing, 0.0, wid);
   glVertex3d(tail, 0.0, wid);
   glVertex3d(tail, 0.0, 0.5);

   glVertex3d(wing, 0.0,-wid);
   glVertex3d(tail, 0.0,-wid);
   glVertex3d(tail, 0.0,-0.5);
   glEnd();
   //  Vertical tail
   glColor3f(1,0,0);
   glBegin(GL_TRIANGLES);
   glVertex3d(strk, 0.0, 0.0);
   glVertex3d(tail, 0.3, 0.0);
   glVertex3d(tail, 0.0, 0.0);
   glEnd();
   //  Undo transformations
   glPopMatrix();
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
    if(axes){
        drawXYZ();
    }

    switch(mode)
    {
        case CUBE:
         cube(0,0,0 , 0.3,0.3,0.3 , 0);
         cube(1,0,0 , 0.2,0.2,0.2 , 45);
         cube(0,1,0 , 0.4,0.4,0.2 , 90);  
         break;
        
        case SPHERE:
         sphere1(0,0,0 , 0.4);
         sphere1(1,0,0 , 0.2);
         sphere2(0,1,0 , 0.2);
         break;
        
        case FLATPLANE:
         FlatPlane(GL_LINE_LOOP , 0,0,0);
         break;
        case AIRPLANE:
         SolidPlane( 0, 0, 0 , 1,0,0 , 0, 1,0);
         SolidPlane(-1, 1, 0 ,-1,0,0 , 0,-1,0);
         SolidPlane(-1,-1, 0 ,-1,0,0 , 0, 1,0);
         break;
    }
    

   
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
           th = 0;      //Azimuth of view angle
           ph = 0;      //Elevation of view angle
           break;
        case('A'):
        case('a'):
            axes = 1 - axes;
            break;
        case('M'):
        case('m'):
            mode = (mode+1)%4;
            break;

        default:
            break;

    }
    /* update the display */
    glutPostRedisplay();
}


/* function called by GLUT when window is resized */
void reshape(int width,int height) //DO NOT MODIFY
{
   //  Set viewport as entire window
   glViewport(0,0, RES*width,RES*height);
   //  Select projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Set projection to identity
   glLoadIdentity();
   //  Orthogonal projection:  unit cube adjusted for aspect ratio
   const double dim = 2.5;
   double asp = (height>0) ? (double)width/height : 1;
   glOrtho(-asp*dim,+asp*dim, -dim,+dim, -dim,+dim);
   //  Select model view matrix
   glMatrixMode(GL_MODELVIEW);
   //  Set model view to identity
   glLoadIdentity();
}
 
/* function called by GLUT when idle */
void idle(void) //DO NOT MODIFY
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

    /* Request 940 x 640 pixel window */
    glutInitWindowSize(940,640);
    /* Create Window */
    glutCreateWindow("Madhukar's Scene Creation");

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
