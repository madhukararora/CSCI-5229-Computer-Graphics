/* CSCI 5229 Computer Graphics
* University of Colorado Boulder
* @author : Madhukar Arora
* HW2
*/

/* Disclaimer : Most of the code is not my own work but referred from the example codes provided
   by the instructor in the class.
   https://thepentamollisproject.blogspot.com/2018/02/setting-up-first-person-camera-in.html
   https://github.com/davidwparker/opengl-screencasts-1/blob/master/010.c
   Used example 8 and example 9 from the lectures.
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
double zh=0;       //  aeroplane flying

int axes=0;        //  Display axes

double dim = 5.0; /*dimension of orthogonal box*/

const char* text[] = {"Orthogonal","Perspective","First Person"};

int fov = 50; // field of view for perspective

/* aspect ratio*/
double asp = 1;

/* parameters for first person view*/
double fpX = -1.4;
double fpZ = 4.8;

int fpTh = 280; 
int fpPh = 20;





//  Cosine and Sine in degrees
#define Cos(x) (cos((x)*3.1415927/180))
#define Sin(x) (sin((x)*3.1415927/180))

typedef enum{
    ORTHOGONAL = 0,
    PERSPECTIVE,
    FIRSTPERSON,
}MODE_T;
MODE_T mode = ORTHOGONAL;

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
   glColor3ub(204,204,0);
   glVertex3f(-1,-1, 1);
   glVertex3f(+1,-1, 1);
   glVertex3f(+1,+1, 1);
   glVertex3f(-1,+1, 1);
   //  Back
   glColor3ub(204,204,0);
   glVertex3f(+1,-1,-1);
   glVertex3f(-1,-1,-1);
   glVertex3f(-1,+1,-1);
   glVertex3f(+1,+1,-1);
   //  Right
   glColor3ub(204,204,0);
   glVertex3f(+1,-1,+1);
   glVertex3f(+1,-1,-1);
   glVertex3f(+1,+1,-1);
   glVertex3f(+1,+1,+1);
   //  Left
   glColor3ub(204,204,0);
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

static void drawRoof(double x, double y, double z,
                    double dx, double dy, double dz,
                    double th)
{
    // Save transformation
    glPushMatrix();

    // Offset, scale
    glTranslated(x,y,z);
    glRotated(th,0,1,0);
    glScaled(dx,dy,dz);

    //front side
    glBegin(GL_TRIANGLES);
    glColor3ub(188,143,143);
    glVertex3f(+1.5,0,1.5);
    glVertex3f(-1.5,0,1.5); //symmetric about the x-axis
    glVertex3f(0,+2, 0);
    glEnd();

    //back side
    glBegin(GL_TRIANGLES);
    glColor3ub(188,143,143);
    glVertex3f(-1.5,0,-1.5);
    glVertex3f(+1.5,0,-1.5); //symmetric about the x-axis
    glVertex3f(0,+2, 0);
    glEnd();

     //left side
    glBegin(GL_TRIANGLES);
    glColor3ub(188,143,143);
    glVertex3f(-1.5,0,+1.5);
    glVertex3f(-1.5,0,-1.5); //symmetric about the z-axis
    glVertex3f(0,+2, 0);
    glEnd();

     //right side
    glBegin(GL_TRIANGLES);
    glColor3ub(188,143,143);
    glVertex3f(+1.5,0,+1.5);
    glVertex3f(+1.5,0,-1.5); //symmetric about the z-axis
    glVertex3f(0,+2, 0);
    glEnd();

     //  Undo transformations
   glPopMatrix();
}


static void house(double x,double y, double z, 
                  double width, double height, double length,
                  double th)
{
    cube(x,y,z,width,height,length,th);
    drawRoof(x,y+height,z,width,height/2,length+0.1,th);
}


/* function to draw a cylinder
https://www.youtube.com/watch?v=Kujd0RTsaAQ
*/
static void cylinder(double x, double y, double z,
                     double radius, double height)
{
    const int delta_Deg = 5;

     // Save transformation
    glPushMatrix();

    // Offset, scale
    glTranslated(x,y,z);
    glRotated(th,0,1,0);
    glScaled(radius,height,radius);
    /*sides*/
    glBegin(GL_QUAD_STRIP);

    for(int j = 0; j <= 360; j+= delta_Deg)
    {
        glColor3ub(165,42,42); //brown
        glVertex3f(Cos(j),+1,Sin(j));
        glVertex3f(Cos(j),-1,Sin(j));
    }
    glEnd();

    /* top and bottom circles */
    /* reuse the texture on top and bottom */

    for(int i = 1; i>= -1; i-=2)
    {
        glBegin(GL_TRIANGLE_FAN);
        glColor3ub(165,42,42); //brown
        glVertex3f(0,i,0);
        for(int k = 0; k <= 360; k+= delta_Deg){
            glColor3ub(165,42,42); //brown
            glVertex3f(i*Cos(k),i,Sin(k));   
        }
        glEnd();
    }    
      //  Undo transformations
   glPopMatrix();
}

/*
* function to generate a cone
https://www.youtube.com/watch?v=Kujd0RTsaAQ
*/
static void cone(double x, double y, double z, double height, double radius)
{
      const int deg = 5;
      int k;
      glPushMatrix();

  /*  Transformation */
  glTranslated(x,y,z);
  glScaled(radius,height,radius);
  glRotated(-90,1,0,0);

  /* sides */
  glBegin(GL_TRIANGLES);
  for (k=0;k<=360;k+=deg){
    glColor3f(0.2,1.0,0.0);
    glVertex3f(0,0,1);
    glColor3f(0.2,1.0,0.0);
    glVertex3f(Cos(k),Sin(k),0);
    glColor3f(0.2,1.0,0.0);
    glVertex3f(Cos(k+deg),Sin(k+deg),0);
  }
  glEnd();

  /* bottom circle */ 
  /* rotate back */
  glRotated(90,1,0,0);
  glBegin(GL_TRIANGLES);
  glColor3f(0.3,1.0,0.0);
  for (k=0;k<=360;k+=deg) {
    glVertex3f(0,0,0);
    glVertex3f(Cos(k),0,Sin(k));
    glVertex3f(Cos(k+deg),0,Sin(k+deg));
  }
  glEnd();

  glPopMatrix();
}


static void tree(double x, double y, double z,
                 double radius, double height)
{
    cylinder(x,y,z,radius/5,height);
    cone(x,y+height,z,radius,height);
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

void airplane()
{
    SolidPlane(1.5*Cos(zh),1.5,0.25*Sin(zh),
    -Sin(zh),0,Cos(zh),
    -0.30*Cos(zh),1,-0.45*Sin(zh));
}
void renderScene(void)
{
   
    double dist  = 0.5;
    house(0,0,0,0.3,0.5,0.5,0);
    tree(dist, 0, dist, 0.1, 0.1);

    house(-dist*3, 0, -dist*4, 0.2, 1, 0.3, 45);
    tree(dist*1.5, 0, -dist, 0.1, 0.4);

    house(dist*4, 0, 0, 0.5, 0.4, 0.6, 30);
    tree(-dist*2, 0, dist*2, 0.2, 0.6);

    house(dist*1.5, 0, -dist*4, 0.2, 1, 0.3, 20);
    tree(dist, 0, -dist*2, 0.2, 0.5);

    house(-dist*4, 0, dist*2, 0.3, 0.7, 0.4, 80);
    tree(dist*2, 0, -dist, 0.1, 0.2);
    
}

void projection(void)
{
    // inform OpenGL we want to manipulate the projection matrix
    glMatrixMode(GL_PROJECTION);

    //reset transformation - clears changes made earlier
    glLoadIdentity();

    if(mode == PERSPECTIVE)
    {
        gluPerspective(fov,asp,dim/8,8*dim);
    }
    else
    {
        glOrtho(-asp*dim,+asp*dim,-dim,+dim,-dim,+dim);
    }
    glMatrixMode(GL_MODELVIEW);

    //reset transformation
    glLoadIdentity();
}

/* function called by GLUT to display the scene */
void display(void)
{
    double Ex, Ey, Ez;
    /* clear screen and Z-Buffer*/
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* reset transformation - clears changes made earlier */
    glLoadIdentity();

    /* draw the axis */ 
    if(axes){
        drawXYZ();
    }

    

    switch(mode)
    {
        case ORTHOGONAL:
            glRotatef(ph,1,0,0);
            glRotatef(th,0,1,0);
            //  Display parameters
            glColor3f(1.0,1.0,1.0);
            glWindowPos2i(5,5);
            Print("Angle= x: %d,y :%d   Projection=%s",th,ph,text[mode]);
            break;
        case PERSPECTIVE:
            Ex = -2*dim*Sin(th)*Cos(ph);
            Ey = +2*dim*Sin(ph);
            Ez = +2*dim*Cos(th)*Cos(ph);
            /* camera/eye position*/
            gluLookAt(Ex,Ey,Ez,0,0,0,0,Cos(ph),0);
            glColor3f(1.0,1.0,1.0);
            glWindowPos2i(5,5);
            Print("Angle= x: %d,y :%d Dim:%.1f FOV = %d Projection=%s",th,ph,dim,fov,text[mode]);
            break;
        case FIRSTPERSON:
            
            
            // imitate first person movement
            gluLookAt(fpX,-1.4,fpZ,Cos(fpTh)+fpX,Sin(fpPh)-1.4,Sin(fpTh)+fpZ,
                    0,1,0);
            glColor3f(1.0,1.0,1.0);
            glWindowPos2i(5,5);
            Print("Angle=%d,%d  Position=%.1f,%.1f   Projection=%s",fpTh,fpPh,fpX,fpZ,"First Person");
            break;
        default:
            break;
    }
    
    glPushMatrix();
    
    airplane();
    renderScene();
    
    glPopMatrix();

   
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
    if(mode != FIRSTPERSON){
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
    }
    else
    {
        switch(key)
        {
            case GLUT_KEY_RIGHT:
                fpTh = (th + 5) % 360;
                break;
            case GLUT_KEY_LEFT:
                fpTh = (th - 5) % 360;
                break;
            case GLUT_KEY_UP:
                fpPh = (ph + 5);
                break;
            case GLUT_KEY_DOWN:
                fpPh = (ph - 5);
                break;   
            if(fpPh > 90)
                fpPh = 90;
            if(fpPh < -90)
                fpPh = -90;


        } 
    }
    projection();
    /* update the display */
    glutPostRedisplay();
}

void key(unsigned char ch,int x, int y)
{
   //  Exit on ESC
  if (ch == 27)
  {
    exit(0);
  }
  else if (ch == 'm' || ch == 'M')
  {
    mode += 1;
    mode %= 3;
  }
  
  if (mode != 2) //mode not first person
  {
    //  Reset view angle
    if (ch == '0')
    {
      th = ph = 0;
    }
    //  Change field of view angle
    else if (ch == '-' && ch>1)
    {
      fov--;
    }
    else if (ch == '+' && ch<179)
    {
      fov++;
    }
    else if(ch == 'a' || ch == 'A')
    {
        axes = 1 - axes;
    }
  }
  else //first person
  {
    if (ch == '0')
    {
        fpX = -1.4;
        fpZ = 4.8;
        fpTh = 180; 
        fpPh = 20;
    }
    else if (ch == 'w')
    {
      fpX += 0.2*Cos(fpTh);
      fpZ += 0.2*Sin(fpTh);
    }
    else if (ch == 's')
    {
      fpX -= 0.2*Cos(fpTh);
      fpZ -= 0.2*Sin(fpTh);
    }
    else if (ch == 'd')
    {
      fpZ += 0.1*Cos(fpTh);
      fpX += -0.1*Sin(fpTh);
    }
    else if (ch == 'a')
    {
      fpZ -= 0.1*Cos(fpTh);
      fpX -= -0.1*Sin(fpTh);
    }
    //  Change field of view angle
    else if (ch == '-' && ch>1)
    {
      dim -= 0.1;
    }
    else if (ch == '+' && ch<179)
    {
      dim += 0.1;
    }
  }
    projection();
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
