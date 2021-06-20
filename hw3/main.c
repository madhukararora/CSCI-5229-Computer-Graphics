/* CSCI 5229 Computer Graphics
* University of Colorado Boulder
* @author : Madhukar Arora
* HW3
*/

/* Disclaimer : Most of the code is not my own work but referred from the example codes provided
   by the instructor in the class.
   https://thepentamollisproject.blogspot.com/2018/02/setting-up-first-person-camera-in.html
   https://github.com/davidwparker/opengl-screencasts-1/blob/master/010.c
   Used example 8 and example 9 from the lectures.
   https://stackoverflow.com/questions/5717654/glulookat-explanation
*/


/* Includes */
#include "CSCIx229.h"

//Globals
int th=0;          //  Azimuth of view angle
int ph=0;          //  Elevation of view angle
int axes=0;        //  Display axes
int fov = 58;      // field of view for perspective
double zh=0;       //  aeroplane flying
/* aspect ratio*/
double asp = 1;
double dim = 2.6; /*dimension of orthogonal box*/
/* parameters for first person view*/

/* where the camera is looking*/
double MX = 0;
double MY = 0;
double MZ = 0;

/* camera position*/
double EX = 0; 
double EY = 0;
double EZ = 6;


const char* text[] = {"Orthogonal","Perspective","First Person"};



typedef enum{
    ORTHOGONAL = 0,
    PERSPECTIVE,
    FIRSTPERSON,
}MODE_T;
MODE_T mode = ORTHOGONAL;







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
   glColor3ub(255,0,0);
   glVertex3f(-1,-1, 1);
   glVertex3f(+1,-1, 1);
   glVertex3f(+1,+1, 1);
   glVertex3f(-1,+1, 1);
   //  Back
   glColor3ub(0,255,0);
   glVertex3f(+1,-1,-1);
   glVertex3f(-1,-1,-1);
   glVertex3f(-1,+1,-1);
   glVertex3f(+1,+1,-1);
   //  Right
   glColor3ub(0,0,255);
   glVertex3f(+1,-1,+1);
   glVertex3f(+1,-1,-1);
   glVertex3f(+1,+1,-1);
   glVertex3f(+1,+1,+1);
   //  Left
   glColor3ub(125,0,255);
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
    glVertex3f(+1.0,0,1.0);
    glVertex3f(-1.0,0,1.0); //symmetric about the x-axis
    glVertex3f(0,+1.5, 0);
    glEnd();

    //back side
    glBegin(GL_TRIANGLES);
    glColor3ub(188,143,143);
    glVertex3f(-1.0,0,-1.0);
    glVertex3f(+1.0,0,-1.0); //symmetric about the x-axis
    glVertex3f(0,+1.5, 0);
    glEnd();

     //left side
    glBegin(GL_TRIANGLES);
    glColor3ub(188,143,143);
    glVertex3f(-1.0,0,+1.0);
    glVertex3f(-1.0,0,-1.0); //symmetric about the z-axis
    glVertex3f(0,+1.5, 0);
    glEnd();

     //right side
    glBegin(GL_TRIANGLES);
    glColor3ub(188,143,143);
    glVertex3f(+1.0,0,+1.0);
    glVertex3f(+1.0,0,-1.0); //symmetric about the z-axis
    glVertex3f(0,+1.5, 0);
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
    cylinder(x,y,z,radius/4,height);
    cone(x,y+height,z,height/3,radius);
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
   
    //double dist  = 0.5;
    house(-1,-1,-1,0.3,0.3,0.3,0);
    tree(-0.5,-1,-1, 0.2, 0.3);
    tree(0,-1,-1, 0.2, 0.3);
     
    house(-1,-1,-2.2,0.4,0.4,0.4,60);
    tree(-0.3,-1,-2, 0.2, 0.3);
    tree(+0.2,-1,-2, 0.2, 0.3);

    house(+1,-1,-1,0.3,0.3,0.3,90);
    tree(+1.5,-1,-1, 0.2, 0.3);
    tree(+2,-1,-1, 0.2, 0.3);

    house(+1,-1,-2,0.3,0.3,0.3,45);
    tree(+1.8,-1,-2, 0.2, 0.3);
    tree(+2.2,-1,-2, 0.2, 0.3);
    
    
}

void projection(void)
{
    // inform OpenGL we want to manipulate the projection matrix
    glMatrixMode(GL_PROJECTION);

    //reset transformation - clears changes made earlier
    glLoadIdentity();

    if(mode)
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
            
            MX = -2*dim*Sin(th)*Cos(ph);
            MY = -2*dim*Sin(ph);
            MZ = -2*dim*Cos(th)*Cos(ph);
            // imitate first person movement
            gluLookAt(EX,EY,EZ,EX+MX,EY+MY,EZ+MZ,
                    0,1,0);
            glColor3f(1.0,1.0,1.0);
            glWindowPos2i(5,5);
            Print("Angle=%d,%d  Position=%.1f,%.1f  Dim:%.1f Projection=%s",th,ph,EX,EZ,dim,text[mode]);
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
  
  if (mode != FIRSTPERSON) //mode not first person
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
        th = 0; ph =0;
        dim = 5.2;
        MX = 0;
        MY = 0;
        MZ = 0;
        EX = 0;
        EY = 0;
        EZ = 5.2;
    }
    else if (ch == 'w')
    {
        EX += MX * 0.1;
        EZ += MZ * 0.1;
    }
    else if (ch == 's')
    {
      EX -= MX * 0.1;
      EZ -= MZ * 0.1;
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

    /* Request 960 x 840 pixel window */
    glutInitWindowSize(960,840);
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
