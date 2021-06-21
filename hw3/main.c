/* CSCI 5229 Computer Graphics
* University of Colorado Boulder
* @author : Madhukar Arora
* HW3
*/

/* Disclaimer : Most of the code is not my own work but referred from the example codes provided
   by the instructor in the class.
   https://thepentamollisproject.blogspot.com/2018/02/setting-up-first-person-camera-in.html
   https://github.com/davidwparker/opengl-screencasts-1/blob/master/010.c
   https://stackoverflow.com/questions/5717654/glulookat-explanation
*/


/* Includes */
#include "CSCIx229.h"

//Globals
int th=0;          //  Azimuth of view angle
int ph=0;          //  Elevation of view angle
int axes=1;        //  Display axes
int fov = 58;      // field of view for perspective
/* aspect ratio*/
double asp = 1;
double dim = 2.6; /*dimension of orthogonal box*/

double len = 2.0; //length of axes
/* parameters for first person view*/

/* where the camera is looking*/
double MX = 0;
double MY = 0;
double MZ = 0;

/* camera position*/
double EX = 0; 
double EY = 0;
double EZ = 6;

/* different modes*/
const char* text[] = {"Orthogonal","Perspective","First Person"};

typedef enum{
    ORTHOGONAL = 0,
    PERSPECTIVE,
    FIRSTPERSON,
}MODE_T;
MODE_T mode = ORTHOGONAL;

/* lighting parameters */
int move      =   1;  // move light 
int light     =   1;  // Lighting
int one       =   1;  // Unit value
int distance  =   3;  // Light distance
int inc       =   5;  // Ball increment
int smooth    =   1;  // Smooth/Flat shading
int local     =   0;  // Local Viewer Model
int emission  =   0;  // Emission intensity (%)
int ambient   =  10;  // Ambient intensity (%)
int diffuse   =  50;  // Diffuse intensity (%)
int specular  =   0;  // Specular intensity (%)
int shininess =   0;  // Shininess (power of two)
float shiny   =   1;  // Shininess (value)
int zh        =  90;  // Light azimuth
float ylight  =   0;  // Elevation of light
typedef struct {float x,y,z;} vtx;



/*
 *  Draw vertex in polar coordinates with normal
 */
static void Vertex(double th,double ph)
{
   double x = Sin(th)*Cos(ph);
   double y = Cos(th)*Cos(ph);
   double z =         Sin(ph);
   //  For a sphere at the origin, the position
   //  and normal vectors are the same
   glNormal3d(x,y,z);
   glVertex3d(x,y,z);
}

/*
 *  Draw a ball
 *     at (x,y,z)
 *     radius (r)
 */
static void ball(double x,double y,double z,double r)
{
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslatef(x,y,z);
   glScalef(r,r,r);
   //  White ball with yellow specular
   float yellow[]   = {1.0,1.0,0.0,1.0};
   float Emission[] = {0.0,0.0,0.01*emission,1.0};
   glColor3f(1,1,1);
   glMaterialf(GL_FRONT,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT,GL_SPECULAR,yellow);
   glMaterialfv(GL_FRONT,GL_EMISSION,Emission);
   //  Bands of latitude
   for (int ph=-90;ph<90;ph+=inc)
   {
      glBegin(GL_QUAD_STRIP);
      for (int th=0;th<=360;th+=2*inc)
      {
         Vertex(th,ph);
         Vertex(th,ph+inc);
      }
      glEnd();
   }
   //  Undo transofrmations
   glPopMatrix();
}


/* convenience function to draw the X-Y-Z axes*/
void drawXYZ(void)
{
    glPointSize(10);
    glColor3f(1.0,1.0,1.0); //color - white
    glBegin(GL_LINES);
    glVertex3d(-1,-1,-1);
    glVertex3d(len,-1,-1); //X-axis
    glVertex3d(-1,-1,-1);
    glVertex3d(-1,len,-1); //Y-axis
    glVertex3d(-1,-1,-1);
    glVertex3d(-1,-1,len); //Z-axis
    glEnd();

    //Label Axes
    glRasterPos3d(len,-1,-1);
    Print("X");
    glRasterPos3d(-1,len,-1);
    Print("Y");
    glRasterPos3d(-1,-1,len);
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
   
   //  Set specular color to white
   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);
   
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
   glNormal3f( 0, 0, 1);
   glVertex3f(-1,-1, 1);
   glVertex3f(+1,-1, 1);
   glVertex3f(+1,+1, 1);
   glVertex3f(-1,+1, 1);
   //  Back
   glColor3ub(0,255,0);
   glNormal3f( 0, 0,-1);
   glVertex3f(+1,-1,-1);
   glVertex3f(-1,-1,-1);
   glVertex3f(-1,+1,-1);
   glVertex3f(+1,+1,-1);
   //  Right
   glColor3ub(0,0,255);
   glNormal3f(+1, 0, 0);
   glVertex3f(+1,-1,+1);
   glVertex3f(+1,-1,-1);
   glVertex3f(+1,+1,-1);
   glVertex3f(+1,+1,+1);
   //  Left
   glColor3ub(125,0,255);
   glNormal3f(-1, 0, 0);
   glVertex3f(-1,-1,-1);
   glVertex3f(-1,-1,+1);
   glVertex3f(-1,+1,+1);
   glVertex3f(-1,+1,-1);
   //  Top
   glColor3f(0,1,1);
   glNormal3f( 0,+1, 0);
   glVertex3f(-1,+1,+1);
   glVertex3f(+1,+1,+1);
   glVertex3f(+1,+1,-1);
   glVertex3f(-1,+1,-1);
   //  Bottom
   glColor3f(1,0,1);
   glNormal3f( 0,-1, 0);
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
* Draw triangle
*/
static void triangle(vtx A, vtx B, vtx C)
{
     //  Planar vector 0
   float dx0 = A.x-B.x;
   float dy0 = A.y-B.y;
   float dz0 = A.z-B.z;
   //  Planar vector 1
   float dx1 = C.x-A.x;
   float dy1 = C.y-A.y;
   float dz1 = C.z-A.z;
   //  Normal
   float Nx = dy0*dz1 - dy1*dz0;
   float Ny = dz0*dx1 - dz1*dx0;
   float Nz = dx0*dy1 - dx1*dy0;
   //  Draw triangle
   glNormal3f(Nx,Ny,Nz);
   glBegin(GL_TRIANGLES);
   glVertex3f(A.x,A.y,A.z);
   glVertex3f(B.x,B.y,B.z);
   glVertex3f(C.x,C.y,C.z);
   glEnd();
}




/* function to draw a roof on top of the house*/

static void drawRoof(double x, double y, double z,
                    double dx, double dy, double dz,
                    double th)
{
    
    vtx A,B,C;
    // Save transformation
    glPushMatrix();

    // Offset, scale
    glTranslated(x,y,z);
    glRotated(th,0,1,0);
    glScaled(dx,dy,dz);

    //front side
    // glBegin(GL_TRIANGLES);
    // glColor3ub(188,143,143);
    // glVertex3f(+1.0,0,1.0);
    // glVertex3f(-1.0,0,1.0); //symmetric about the x-axis
    // glVertex3f(0,+1.5, 0);
    // glEnd();
    A.x = 1.0; A.y = 0; A.z = +1.0;
    B.x = -1.0; B.y = 0; B.z = +1.0;
    C.x = 0; C.y = +1.5; C.z = 0;
    glColor3ub(188,143,143);
    triangle(A,B,C);

    //back side
    // glBegin(GL_TRIANGLES);
    // glColor3ub(188,143,143);
    // glVertex3f(-1.0,0,-1.0);
    // glVertex3f(+1.0,0,-1.0); //symmetric about the x-axis
    // glVertex3f(0,+1.5, 0);
    // glEnd();
    A.x = -1.0; A.y = 0; A.z = -1.0;
    B.x = +1.0; B.y = 0; B.z = -1.0;
    C.x = 0; C.y = +1.5; C.z = 0;
    glColor3ub(188,143,143);
    triangle(A,B,C);

     //left side
    // glBegin(GL_TRIANGLES);
    // glColor3ub(188,143,143);
    // glVertex3f(-1.0,0,+1.0);
    // glVertex3f(-1.0,0,-1.0); //symmetric about the z-axis
    // glVertex3f(0,+1.5, 0);
    // glEnd();
    A.x = -1.0; A.y = 0; A.z = +1.0;
    B.x = -1.0; B.y = 0; B.z = -1.0;
    C.x = 0; C.y = +1.5; C.z = 0;
    glColor3ub(188,143,143);
    triangle(A,B,C);

     //right side
    // glBegin(GL_TRIANGLES);
    // glColor3ub(188,143,143);
    // glVertex3f(+1.0,0,+1.0);
    // glVertex3f(+1.0,0,-1.0); //symmetric about the z-axis
    // glVertex3f(0,+1.5, 0);
    // glEnd();
    A.x = +1.0; A.y = 0; A.z = +1.0;
    B.x = +1.0; B.y = 0; B.z = -1.0;
    C.x = 0; C.y = +1.5; C.z = 0;
    glColor3ub(188,143,143);
    triangle(A,B,C);


  
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
        float brown[] = {0.64,0.16,0.16,1};
        float emit[] = {0.0,0.0,0.01*emission,1};
        glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
        glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,brown);
        glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,emit);
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
            glNormal3d(Sin(th),0,Cos(th));
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
    
    vtx A,B,C;
    const int deg = 5;
    int k;
    glPushMatrix();

    /*  Transformation */
    glTranslated(x,y,z);
    glScalef(radius,height,radius);
    glRotated(-90,1,0,0);

    /* sides */
    
    glVertex3f(0,0,0);
    for (k=0;k<=360;k+=deg){
        glColor3f(0.2,1.0,0.0);
        float green[] = {0,1.0,0,1.0};
        float emit[] = {0.0,0.0,0.01*emission,1.0};
        glMaterialf(GL_FRONT,GL_SHININESS, shiny);
        glMaterialfv(GL_FRONT,GL_SPECULAR, green);
        glMaterialfv(GL_FRONT,GL_EMISSION, emit);
        // //glVertex3f(0,0,1);
        A.x = 0; A.y = 0, A.z = 1;
        // // glVertex3f(Cos(k),Sin(k),0);
        B.x = Cos(k); B.y = Sin(k); B.z = 0;
        // //glVertex3f(Cos(k+deg),Sin(k+deg),0);
        C.x = Cos(k+deg); C.y = Sin(k+deg); C.z = 0;
        triangle(A,B,C);
       
    }
    
 

  /* bottom circle */ 
  /* rotate back */
  glRotated(90,1,0,0);
  glColor3f(0.3,1.0,0.0);
  for (k=0;k<=360;k+=deg) {
    float green[] = {0.0,1.0,0,1.0};
    float emit[] = {0.0,0.0,0.01*emission,1.0};
    glMaterialf(GL_FRONT,GL_SHININESS, shiny);
    glMaterialfv(GL_FRONT,GL_SPECULAR, green);
    glMaterialfv(GL_FRONT,GL_EMISSION, emit);  
    // glVertex3f(0,0,0);
    // glVertex3f(Cos(k),0,Sin(k));
    // glVertex3f(Cos(k+deg),0,Sin(k+deg));
    A.x = 0; A.y = 0; A.z = 0;
    B.x = Cos(k); B.y = 0; B.z = Sin(k);
    C.x = Cos(k+deg); C.y = 0; C.z = Sin(k+deg);
    triangle(A,B,C);
  }
 

  glPopMatrix();
}


static void tree(double x, double y, double z,
                 double radius, double height)
{
    cylinder(x,y,z,radius/5,height);
    cone(x,y+height/2,z,height,radius);
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

    /* enable z-buffer depth test */
    glEnable(GL_DEPTH_TEST);

    /* reset transformation - clears changes made earlier */
    glLoadIdentity();

   
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
    if(light) //taken from example 13 provided by Instructor.
    {
        //translate intensity to color vectors
        float Ambient[] = {0.01*ambient,0.01*ambient,0.01*ambient,1.0};
        float Diffuse[] = {0.01*diffuse,0.01*diffuse,0.01*diffuse,1.0};
        float Specular[] = {0.01*specular,0.01*specular,0.01*specular,1.0};

        // light position
        float Position[] = {distance*Cos(zh),ylight,distance*Sin(zh),1.0};

        // draw light position as ball (still no lighting here)
        glColor3f(1,1,1);
        ball(Position[0],Position[1],Position[2],0.1);

        //OpenGL should normalize normal vectors
        glEnable(GL_NORMALIZE);

        //Enable lighting
        glEnable(GL_LIGHTING);

        //location of viewer for specular calculations
        glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,local);

        // glColor sets ambient and diffuse color materials
        glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
        glEnable(GL_COLOR_MATERIAL);

        //enable light 0
        glEnable(GL_LIGHT0);

        //set ambient, diffuse, specular components and postion of light 0
        glLightfv(GL_LIGHT0,GL_AMBIENT,Ambient);
        glLightfv(GL_LIGHT0,GL_DIFFUSE,Diffuse);
        glLightfv(GL_LIGHT0,GL_SPECULAR,Specular);
        glLightfv(GL_LIGHT0,GL_POSITION,Position);

      glWindowPos2i(5,45);
      Print("Model=%s LocalViewer=%s Distance=%d Elevation=%.1f",smooth?"Smooth":"Flat",local?"On":"Off",distance,ylight);
      glWindowPos2i(5,25);
      Print("Ambient=%d  Diffuse=%d Specular=%d Emission=%d Shininess=%.0f",ambient,diffuse,specular,emission,shiny);
        
    }
    else
        glDisable(GL_LIGHTING);


    glPushMatrix();
    renderScene();
    glPopMatrix();

    
    /* draw the axis */
    glDisable(GL_LIGHTING); 
    if(axes){
        drawXYZ();
    }
    
    
    /* Sanity check */
    ErrCheck("display"); 
    /* make scene visible */
    glFlush();
    /* swap double buffer */
    glutSwapBuffers();

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

/* function called by GLUT when special keys are pressed*/
void special(int key,int x, int y)
{
    
    switch(key)
    {
            //  Right arrow key - increase angle by 5 degrees
            case GLUT_KEY_RIGHT:
                th = (th + 5) % 360;
                break;
            //  Left arrow key - decrease angle by 5 degrees
            case GLUT_KEY_LEFT:
                th = (th - 5) % 360;
                break;
            //  Up arrow key - increase elevation by 5 degrees
            case GLUT_KEY_UP:
                ph = (ph + 5) % 360;
                break;
            //  Down arrow key - decrease elevation by 5 degrees
            case GLUT_KEY_DOWN:
                ph = (ph - 5) % 360;
                break; 
            //  PageUp key - increase dim
            case GLUT_KEY_PAGE_UP:
                dim+= 0.1;
                break;
            //  PageDown key - decrease dim
            case GLUT_KEY_PAGE_DOWN:
                dim -= 0.1;
                break;
            // Smooth color model
            case GLUT_KEY_F1:
                smooth = 1-smooth;
                break;
            // Local Viewer
            case GLUT_KEY_F2:
                local = 1-local;
                break;
            //change light distance
            case GLUT_KEY_F3:
                distance = (distance==1)?3:1;
                break;
            // toggle ball increment
            case GLUT_KEY_F8:
                inc = (inc==10)?3:10;
                break;
            default:
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
  else if(ch == 'x' || ch == 'X') // toggle Axes
  {
      axes = 1-axes;
  }
  else if (ch == 'm' || ch == 'M') // change between different modes
  {
    mode = (mode+1)%3;
  }
  else if (ch == 'l' || ch == 'L') // toggle light on-off
  {
    light = 1-light;
  }
  else if(ch == 'p' || ch == 'P') // toggle light movement
  {
      move = 1-move;
  }
  //move light
  else if(ch == '<')
  {
      zh += 1;
  }
  else if(ch == '>')
  {
      zh -= 1;
  }
  //light elevation
  else if(ch == '[')
  {
      ylight -= 0.1;
  }
  else if(ch == ']')
  {
      ylight += 0.1;
  }
  //  Ambient level
  else if (ch=='a' && ambient>0)
      ambient -= 5;
  else if (ch=='A' && ambient<100)
      ambient += 5;
      //  Diffuse level
  else if (ch=='d' && diffuse>0)
      diffuse -= 5;
  else if (ch=='D' && diffuse<100)
      diffuse += 5;
      //  Specular level
  else if (ch=='v' && specular>0)
      specular -= 5;
  else if (ch=='V' && specular<100)
      specular += 5;
  //  Emission level
   //  Emission level
   else if (ch=='e' && emission>0)
      emission -= 5;
   else if (ch=='E' && emission<100)
      emission += 5;       
    //  Shininess level
   else if (ch=='b' && shininess>-1)
      shininess -= 1;
   else if (ch=='B' && shininess<7)
      shininess += 1;
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
  }
    //  Translate shininess power to value (-1 => 0)
    shiny = shininess<0 ? 0 : pow(2.0,shininess);
    projection();
    /* update the display */
    glutIdleFunc(move?idle:NULL);
    glutPostRedisplay();
}


/* function called by GLUT when window is resized */
void reshape(int width,int height) //DO NOT MODIFY
{
    //  Ratio of the width to the height of the window
  asp = (height>0) ? (double)width/height : 1;
  //  Set the viewport to the entire window
  glViewport(0,0, width,height);
  //  Tell OpenGL we want to manipulate the projection matrix
  glMatrixMode(GL_PROJECTION);
  //  Undo previous transformations
  glLoadIdentity();
  //  Switch to manipulating the model matrix
  glMatrixMode(GL_MODELVIEW);
  //  Undo previous transformations
  glLoadIdentity();
  //  Set the viewport to the entire window
  glViewport(0,0, width,height);
  //  Set projection
  projection();
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
    glutCreateWindow("Madhukar's Lighting and Texture");

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
    
    /* pass control for GLUT for events */
    glutMainLoop();

    /* return to OS */
    return 0;

}
