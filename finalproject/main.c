/* CSCI 5229 Computer Graphics
* University of Colorado Boulder
* @author : Madhukar Arora
* Final Project Summer 2021
*/

/*
* Disclaimer:
  I've borrowed most of the source code from the references provided in the class and other resource online.
  Links to referred material are attached with the code segment. 

  References:
  Fractal  mountain - https://bradleycarey.com/posts/2012-11-24-fractal-mountains-in-opengl-using-glut/

*/


/* Includes */
#include "CSCIx229.h"
#include <time.h>
#include <limits.h>
#include <float.h>
#include <stdbool.h>


//Globals
int th = -25;             //  Azimuth of view angle
int ph = 15;            //  Elevation of view angle
int axes=1;           //  Display axes
int fov = 60;        //  field of view for perspective

double asp = 1;    //  aspect ratio
double dim = 5.0;  // dimension of orthogonal box
double len = 2.0;  //length of axes

//parameters for first person view
int th_FP = -30;
int ph_FP = 0;
/* where the camera is looking*/
double MX = 0.0;
double MY = 0.0;
double MZ = 0.0;

/* camera position*/
double EX = 2.0; 
double EY = 0.7;
double EZ = 1.8;

/* different modes*/
const char* text[] = {"Orthogonal","Perspective","First Person"};

typedef enum{ ORTHOGONAL = 0, PERSPECTIVE,FIRSTPERSON}MODES_T;
MODES_T mode = ORTHOGONAL;


typedef enum{DIRTROAD, GRASS, LAKE, COT, MATTRESS, CHAIR, BENCH,BRIDGE}OBJECTS_T;
OBJECTS_T object;

float tx_X, tx_Y; // variables for applying texture coordinates to different objects


// lighting parameters 
int move      =   1;  // move light 
int light     =   0;  // Lighting
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

const float white[] = {1,1,1,1};
const float black[] = {0,0,0,1};


// door
int doorOpen = 0; // open or close door





// texture parameters

// cabin 
unsigned int exteriorWall;
unsigned int interiorWall;
unsigned int floorTexture;
unsigned int roofTexture;
unsigned int cotWoodTexture;
unsigned int mattressTexture;
unsigned int chairTexture;
unsigned int doorTexture;



// surrounding
unsigned int roadTexture;
unsigned int grassTexture;
unsigned int leafTexture;
unsigned int woodTexture;
unsigned int lakeTexture;
unsigned int fenceTexture;
unsigned int benchTexture;
unsigned int bridgeTexture;


// Fractal Tree

int nBranches = 5; // number of branches
float branch_leaf = 0.1; // when to sprout leaves - if length of branch is shorter than this draw leaves
float branch_angle = 20; // angle of branches - between the vertical and the next branch
float branch_ratio = 0.69; // length before branching
float branch_shrink = 0.7; // branch radius shrink


//Waves for the pond
#define NUM_CIRCLES 300
int angle_increment=10, angle=5;
double vertex[NUM_CIRCLES][NUM_CIRCLES][3];
double normal[NUM_CIRCLES][NUM_CIRCLES][3];


/*
* Random float between min and max
*
*/
float frand(float min, float max)
{
    return min+random()*(max-min)/RAND_MAX;
}


/*
* Draw leaf
*
*/
int leaf()
{
    // bright green with two sided lighting
    glColor3f(0.1,0.8,0.1);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,1); // leaf to light up on both sides
    
    // Normal is for forward face
    glNormal3f(0,0,1);
    glBegin(GL_QUADS);
    glVertex3f(0.0,0.0,0.0);
    glVertex3f(+0.03,0.03,0.0);
    glVertex3f(0.0,0.1,0.0);
    glVertex3f(-0.03,0.03,0.0);
    glEnd();

    // Disable two sided lighting for performance
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,0);

    //Triangle count 
    return 2;
}



/*
* Draw  a branch as a hollow cylinder
*  of length l radius r
*/
int branch(float l, float r)
{
    // scale to dimensions
    glPushMatrix();
    glScaled(r,l,r);

    //Bark texture
    glColor3f(1,1,1);
    glBindTexture(GL_TEXTURE_2D,woodTexture); 
    glEnable(GL_TEXTURE_2D);

    //Draw truncated cone
    //Face cull for performance - cut down the number of triangles needed to draw
    glEnable(GL_CULL_FACE);
    glBegin(GL_QUAD_STRIP);
    for(int th=0; th<=360; th+=30)
    {
        //Normal for truncated cone
        glNormal3f(Cos(th),1-branch_shrink,Sin(th));

        //Root
        glTexCoord2d(th/120.0,0);
        glVertex3f(Cos(th),0,Sin(th));
        
        //End (shrunk)
        glTexCoord2d((th/120.0),(l/r));
        glVertex3f(branch_shrink*Cos(th),1,branch_shrink*Sin(th));
    }
    glEnd();

    //Undo
    glDisable(GL_CULL_FACE);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

    //triangle count
    return (2*360)/30;

}

/*
* Draw a tree branch of length l0
* and radius r0
*
*/
int tree_branch(float l0, float r0)
{


    // randomize length +/- 15%
    float l = l0*frand(0.85,1.15);

    // randomize angle 
    float angle = frand(0,360.0/nBranches);

    // save transformation since this function is called recursively
    glPushMatrix();

    // Draw the actual branch 
    int ntri = branch(l,r0);

    // Update origin to the end of the branch
    glTranslatef(0,l,0);

    // leaves at the end of the branch
    if(l < branch_leaf)
    {
        // Initial leaf angle is random
        glRotatef(angle,0,1,0);
        // Draw num leaves equally rotated
        for(int k = 0; k < nBranches; k++)
        {
            glRotatef(360.0/nBranches,0,1,0);
            ntri += leaf();
        }

    }
    else
    {
        float r = branch_shrink*r0;
        for(int  k = 0; k < nBranches; k++)
        {
            glPushMatrix();
            glRotatef(angle+k*360.0/nBranches,0,1,0);
            glRotatef(branch_angle,1,0,0);
            ntri = tree_branch(branch_ratio*l0,r);
            glPopMatrix();
        }
    }

    // Undo transformations
    glPopMatrix();

    //return triangle count
    return ntri;


}



void createTree(double x, double y, double z, double h, double r, float seed)
{
    glTranslated(x,y,z);
    srand(seed); // restart random with same value every frame
    int ntri = tree_branch(h,r);


}


// Mountain 
double tolerance = 0.025;
double lightPos[3] = {50,35,1.95};

double randomNormal (double mu, double sigma)
{
  double sum = 0;
  for(int i = 0; i < 12; i++)
    sum = sum + (double)rand()/RAND_MAX;
  sum -= 6;
  return sigma*sum + mu;
}

void fracMountain(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3, float x4, float y4, float z4)
{
    
 
    double perim = sqrt((x2-x1)*(x2-x1) + (z2-z1)*(z2-z1)) + sqrt((x3-x2)*(x3-x2) + (z3-z2)*(z3-z2))
        + sqrt((x4-x3)*(x4-x3) + (z4-z3)*(z4-z3)) + sqrt((x1-x4)*(x1-x4) + (z1-z4)*(z1-z4));

    // draw polygon
    if(perim <= tolerance)
    {
        double v1=x2-x1, v2=y2-y1, v3=z2-z1;
        double w1=x3-x2, w2=y3-y2, w3=z3-z2;
        double crossx=v2*w3-w2*v3;
        double crossy=v3*w1-w3*v1;
        double crossz=v1*w2-w1*v2;

        double tolightx=lightPos[0]-x1;
        double tolighty=lightPos[1]-y1;
        double tolightz=lightPos[2]-z1;

        double dot =crossx*tolightx +crossy*tolighty +crossz*tolightz;
        double normc=sqrt(crossx*crossx +crossy*crossy + crossz*crossz);
        double norml = sqrt(tolightx*tolightx + tolighty*tolighty + tolightz*tolightz);

        double lightamt=dot/(normc*norml);
        
        
        if(lightamt>1)
            lightamt=1;
        if(lightamt<0)
            lightamt=0.5;

        lightamt=(lightamt*2 +0.15)/2.15;
        
        // add effects to make the mountains look snow-capped.
        glColor3f(lightamt,lightamt,lightamt);
        glBegin(GL_POLYGON);
            glVertex3f(x1,y1,z1);
            glVertex3f(x2,y2,z2);
            glVertex3f(x3,y3,z3);
            glVertex3f(x4,y4,z4);
        glEnd();
    }
    else //recursively call fracMountain()
    {
        double r1 = (double)randomNormal(0.041, 0.1);
        double r2 = (double)randomNormal(0.0,   0.01);
        double r3 = (double)randomNormal(0.0,   0.01);
        // calculate mid points on the X, Y and Z axis.
        double xmid = ((x1+x2+x3+x4)/4)+r2*perim;
        double ymid = ((y1+y2+y3+y4)/4)+r1*perim;
        if(ymid<0)
            ymid=0;
        double zmid = ((z1+z2+z3+z4)/4)+r3*perim;
        fracMountain(x1,y1,z1,(x1+x2)/2,(y1+y2)/2,(z1+z2)/2,xmid,ymid,zmid,(x1+x4)/2,(y1+y4)/2,(z1+z4)/2);
        fracMountain((x1+x4)/2,(y1+y4)/2,(z1+z4)/2,xmid,ymid,zmid,(x4+x3)/2,(y4+y3)/2,(z4+z3)/2,x4,y4,z4);
        fracMountain((x1+x2)/2,(y1+y2)/2,(z1+z2)/2,x2,y2,z2,(x2+x3)/2,(y2+y3)/2,(z2+z3)/2,xmid,ymid,zmid);
        fracMountain(xmid,ymid,zmid,(x2+x3)/2,(y2+y3)/2,(z2+z3)/2,x3,y3,z3,(x3+x4)/2,(y3+y4)/2,(z3+z4)/2);
    }
}


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
   
   glPushMatrix(); //  Save transformation

   //  Offset, scale and rotate
   glTranslatef(x,y,z);
   glScalef(r,r,r);

   //  White ball with yellow specular
   float yellow[]   = {1.0,1.0,0.0,1.0};
   float Emission[] = {0.0,0.0,0.01*emission,1.0};
 
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
   
   glPopMatrix();//  Undo transofrmations
}


/* convenience function to draw the X-Y-Z axes*/
void drawXYZ(void)
{
    glPointSize(10);
    glColor3f(1.0,1.0,1.0); //color - white
    glTranslated(-1.6,0,-1.6);
    glBegin(GL_LINES);
    glVertex3d(0,0,0);
    glVertex3d(len,0,0); //X-axis
    glVertex3d(0,0,0);
    glVertex3d(0,len,0); //Y-axis
    glVertex3d(0,0,0);
    glVertex3d(0,0,len); //Z-axis
    glEnd();

    //Label Axes
    glRasterPos3d(len,0,0);
    Print("X");
    glRasterPos3d(0,len,0);
    Print("Y");
    glRasterPos3d(0,0,len);
    Print("Z");
    
    /* update display */
    glutPostRedisplay();
}


///////////////////////////////////////////////////SHAPES////////////////////////////////////////////////////////////////

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
   // Define material for specular and emission
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white); // specular component as white
  
   
  
   glPushMatrix(); //  Save transformation
   glTranslated(x,y,z); //  Offset
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);

   if(object == DIRTROAD)
   {
       tx_X = 5.0;
       tx_Y = 5.0;
       glEnable(GL_TEXTURE_2D);
       glBindTexture(GL_TEXTURE_2D,roadTexture);
   }
   else if(object == GRASS)
   {
       tx_X = 1.0;
       tx_Y = 1.0;
       glEnable(GL_TEXTURE_2D);
       glBindTexture(GL_TEXTURE_2D,grassTexture);
   }
   else if(object == LAKE)
   {
       tx_X= 1.0;
       tx_Y = 1.0;
       glEnable(GL_TEXTURE_2D);
       glBindTexture(GL_TEXTURE_2D,lakeTexture);
   }
   else if(object == COT)
   {
       tx_X = 1.0;
       tx_Y = 1.0;
       glEnable(GL_TEXTURE_2D);
       glBindTexture(GL_TEXTURE_2D,cotWoodTexture);
   }
   else if(object == MATTRESS)
   {
       tx_X = 1.0;
       tx_Y = 1.0;
       glEnable(GL_TEXTURE_2D);
       glBindTexture(GL_TEXTURE_2D,mattressTexture);
   }
   else if(object == CHAIR)
   {
       tx_X= 1.0;
       tx_Y = 1.0;
       glEnable(GL_TEXTURE_2D);
       glBindTexture(GL_TEXTURE_2D,chairTexture);
   }
   else if(object == BENCH)
   {
       tx_X= 1.0;
       tx_Y = 1.0;
       glEnable(GL_TEXTURE_2D);
       glBindTexture(GL_TEXTURE_2D,benchTexture);

   }
   else if(object == BRIDGE)
   {
       tx_X = 1.0;
       tx_Y = 1.0;
       glEnable(GL_TEXTURE_2D);
       glBindTexture(GL_TEXTURE_2D,bridgeTexture);
   }
   
   glBegin(GL_QUADS);  // cube
   //  Front
   glNormal3f( 0, 0, 1);
   glTexCoord2f(0,0);glVertex3f(-1,-1, 1);
   glTexCoord2f(tx_X,0);glVertex3f(+1,-1, 1);
   glTexCoord2f(tx_X,tx_Y);glVertex3f(+1,+1, 1);
   glTexCoord2f(0,tx_Y);glVertex3f(-1,+1, 1);
   
   //  Back
   glNormal3f( 0, 0,-1);
   glTexCoord2f(0,0);glVertex3f(+1,-1,-1);
   glTexCoord2f(tx_X,0);glVertex3f(-1,-1,-1);
   glTexCoord2f(tx_X,tx_Y);glVertex3f(-1,+1,-1);
   glTexCoord2f(0,tx_Y);glVertex3f(+1,+1,-1);
   
   //  Right
   glNormal3f(+1, 0, 0);
   glTexCoord2f(0,0);glVertex3f(+1,-1,+1);
   glTexCoord2f(tx_X,0);glVertex3f(+1,-1,-1);
   glTexCoord2f(tx_X,tx_Y);glVertex3f(+1,+1,-1);
   glTexCoord2f(0,tx_Y);glVertex3f(+1,+1,+1);
   
   //  Left
   glNormal3f(-1, 0, 0);
   glTexCoord2f(0,0);glVertex3f(-1,-1,-1);
   glTexCoord2f(tx_X,0);glVertex3f(-1,-1,+1);
   glTexCoord2f(tx_X,tx_Y);glVertex3f(-1,+1,+1);
   glTexCoord2f(0,tx_Y);glVertex3f(-1,+1,-1);
  
   //  Top
   glNormal3f( 0,+1, 0);
   glTexCoord2f(0,0);glVertex3f(-1,+1,+1);
   glTexCoord2f(tx_X,0);glVertex3f(+1,+1,+1);
   glTexCoord2f(tx_X,tx_Y);glVertex3f(+1,+1,-1);
   glTexCoord2f(0,tx_Y);glVertex3f(-1,+1,-1);
   
   //  Bottom
   glNormal3f( 0,-1, 0);
   glTexCoord2f(0,0);glVertex3f(-1,-1,-1);
   glTexCoord2f(tx_X,0);glVertex3f(+1,-1,-1);
   glTexCoord2f(tx_X,tx_Y);glVertex3f(+1,-1,+1);
   glTexCoord2f(0,tx_Y);glVertex3f(-1,-1,+1);
   
   //  End
   glEnd();
   //  Undo transformations
   glPopMatrix();
   glDisable(GL_TEXTURE_2D);
}

/* 
* Draw a cylinder
* at (x,y,z)
* with radius r and height h
* rotated th about the y axis and ph about the x axis
* Referred : https://www.youtube.com/watch?v=Kujd0RTsaAQ
*/
static void cylinder(double x, double y, double z,
                     double th, double ph,
                     double r, double h)
{
    const int delta_Deg = 5;

    // Define material for specular and emission
    glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white); // specular component as white
    glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black); // emission component as black


     // Save transformation
    glPushMatrix();

    // Offset, scale
    glTranslated(x,y,z);
    glRotated(th,0,1,0);
    glRotated(ph,0,1,0);
    glScalef(r,h,r);
    
    /*sides*/
    glBegin(GL_QUAD_STRIP);
    for(int j = 0; j <= 360; j+= delta_Deg)
    {

        glNormal3f(Cos(j),Sin(j),0);
        glTexCoord2f(0,0.5*j);
        glVertex3f(Cos(j),+1,Sin(j));
        glTexCoord2f(1,0.5*j);
        glVertex3f(Cos(j),-1,Sin(j));
    }
    glEnd();

    /* top and bottom circles */
    for(int i = 1; i>= -1; i-=2)
    {
        glNormal3f(0,0,i);
        glBegin(GL_TRIANGLE_FAN);
        glTexCoord2f(0.5,0.5);
        glVertex3f(0,0,i);
        for(int k = 0; k <= 360; k+= delta_Deg){
            glTexCoord2f(0.5*Cos(k)+0.5,0.5*Sin(k)+0.5);
            glVertex3f(i*Cos(k),Sin(k),i);   
        }
        glEnd();
    }    
      //  Undo transformations
   glPopMatrix();
}


/*
* Draw a cone
* at (x,y,z)
* with radius r and height h
* rotated th about the y-axis and ph about the x-axis
*  Referred : https://www.youtube.com/watch?v=Kujd0RTsaAQ
*/
static void cone(double x, double y, double z, double th, double ph, double r, double h)
{
    const int delta_Deg = 5;
    
    // Define material for specular and emission
    glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white); // specular component as white
    glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black); // emission component as black
    
    
    glPushMatrix();
    glTranslated(x,y,z);
    glRotated(th,0,1,0);
    glRotated(ph,1,0,0);
    glScalef(r,r,h);

    /* sides */
    
    glBegin(GL_TRIANGLES);
    for (th=0;th<=360;th+=delta_Deg){
        glNormal3f(Cos(th-inc),Sin(th-inc),1);
		glTexCoord2f(0,0);
	    glVertex3f(Cos(th-inc),Sin(th-inc),0);

		glNormal3f(Cos(th),Sin(th),1);
		glTexCoord2f(0.5,h);
	    glVertex3f(0,0,1);

		glNormal3f(Cos(th+inc),Sin(th+inc),1);
		glTexCoord2f(1,0);
	    glVertex3f(Cos(th+inc),Sin(th+inc),0);        
    }
    glEnd();
 

  /* bottom circle */ 
  glBegin(GL_TRIANGLES);
  glNormal3f(0,0,-1);
  glTexCoord2f(0.5,0.5);
  glVertex3f(0,0,0);
  for(th = -inc; th <= 360; th+= 2*inc)
  {
      glTexCoord2f(0.5*Cos(th)+0.5,0.5*Sin(th)+0.5);
      glVertex3f(Cos(th),Sin(th),0);
  }
  
  glEnd();
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
   glTexCoord2f(0,0);glVertex3f(A.x,A.y,A.z);
   glTexCoord2f(3,0);glVertex3f(B.x,B.y,B.z);
   glTexCoord2f(0,5);glVertex3f(C.x,C.y,C.z);
   glEnd();
}


/////////////////////////////////////////////////////OBJECTS///////////////////////////////////////////////////////////////////////////////////

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

    A.x = 1.0; A.y = 0; A.z = +1.0;
    B.x = -1.0; B.y = 0; B.z = +1.0;
    C.x = 0; C.y = +1.5; C.z = 0;
    
    triangle(A,B,C);

    A.x = -1.0; A.y = 0; A.z = -1.0;
    B.x = +1.0; B.y = 0; B.z = -1.0;
    C.x = 0; C.y = +1.5; C.z = 0;
    
    triangle(A,B,C);

    A.x = -1.0; A.y = 0; A.z = +1.0;
    B.x = -1.0; B.y = 0; B.z = -1.0;
    C.x = 0; C.y = +1.5; C.z = 0;
    
    triangle(A,B,C);

   
    A.x = +1.0; A.y = 0; A.z = -1.0;
    B.x = +1.0; B.y = 0; B.z = +1.0;
    C.x = 0; C.y = +1.5; C.z = 0;
    
    triangle(A,B,C);

  
   //  Undo transformations
   glPopMatrix();
}



/*
* Draw a tree
* at (x,y,z)
* with radius r and height h
* rotated th about the y-axis and ph about the x-axis
*/
static void tree(double x, double y, double z,
                 double radius, double height)
{
    //trunk
    
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,woodTexture);
    cylinder(x,y,z,0,90,radius/5,height);
    glDisable(GL_TEXTURE_2D);

    //leaves
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,leafTexture);
    cone(x,y+height/4,z,0,-90,radius,height*1.5);
    glDisable(GL_TEXTURE_2D);
    
    
}

/**
 * @brief  create a cabin at (x,y,z)
 *         of length dx, height dy and width dz
 *         rotated th about the y-axis
 * 
 */
static void drawCabin(double x, double y, double z, 
                 double dx, double dy, double dz,
                 double th)
{
    // X-Y coordinates for texture mapping
    int tex_x = 4, tex_y = 2; 
    
    // save transformation
    glPushMatrix();
    // offset
    glTranslated(x,y,z);
    glRotated(th,0,1,0);
    glScaled(dx,dy,dz);

    // Define material for specular and emission
    glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white); // specular component as white
    glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black); // emission component as black



   // Add wooden wall texture to outside walls
   //OUTSIDE : front wall of the cabin  (along X-axis)
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D,exteriorWall);


   // side walls of the cabin along X-axis
   glBegin(GL_QUADS);
                            
   glNormal3f(0,0,1); // positive Z-axis
   glTexCoord2f(0,0);glVertex3f(-1,y,1);
   glTexCoord2f(tex_x,0);glVertex3f(1,y, 1);
   glTexCoord2f(tex_x,tex_y);glVertex3f(1,+1, 1);
   glTexCoord2f(0,tex_y);glVertex3f(-1,+1, 1);
   
   //OUTSIDE : side wall of the cabin (along X-axis)
   glNormal3f( 0, 0, -1); // negative z-axis
   glTexCoord2f(0,0);glVertex3f(+1,y,-1);
   glTexCoord2f(tex_x,0);glVertex3f(-1,y,-1);
   glTexCoord2f(tex_x,tex_y);glVertex3f(-1,+1,-1);
   glTexCoord2f(0,tex_y);glVertex3f(+1,+1,-1);

   //OUTSIDE : back side of the wall (along Z-axis)
   glNormal3f(-1, 0, 0);
   glTexCoord2f(0,0);glVertex3f(-1,y,-1);
   glTexCoord2f(tex_x,0);glVertex3f(-1,y,+1);
   glTexCoord2f(tex_x,tex_y);glVertex3f(-1,+1,+1);
   glTexCoord2f(0,tex_y);glVertex3f(-1,+1,-1);


   glEnd();

  
  
   // Outside wall section without door (along Z-axis)

   glBegin(GL_QUADS);
   glNormal3f(+1, 0, 0); //positive x-axis
   glTexCoord2f(0,0);glVertex3f(+1,y,+1);
   glTexCoord2f(tex_x,0);glVertex3f(+1,y,-0.3);
   glTexCoord2f(tex_x,tex_y);glVertex3f(+1,+1,-0.3);
   glTexCoord2f(0,tex_y);glVertex3f(+1,+1,+1);
   glEnd();
   glDisable(GL_TEXTURE_2D);


   // Draw closed door
   if(!doorOpen){
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,doorTexture);
    glBegin(GL_QUADS);
    glNormal3f(+1,0,0); // positive x-axis
    glTexCoord2f(0,0);glVertex3f(+1,y,-0.29);
    glTexCoord2f(1,0);glVertex3f(+1,y,-1.0);
    glTexCoord2f(1,1);glVertex3f(+1,1,-1.0);
    glTexCoord2f(0,1);glVertex3f(+1,+1,-0.29); //length of door - -0.29 + 1.0 = 0.71
    glEnd();
    glDisable(GL_TEXTURE_2D);
   }
   else
   {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D,doorTexture);
        glBegin(GL_QUADS);
        // glColor3f(1,1,0);
        glNormal3f(0,0,-1); // negative z-axis
        glTexCoord2f(0,0);glVertex3f(+1,y,-0.29);
        glTexCoord2f(1,0);glVertex3f(+1,1,-0.29);
        glTexCoord2f(1,1);glVertex3f(0.29,1,-0.29);     // 1-x = 0.71 // x = 0.29
        glTexCoord2f(0,1);glVertex3f(0.29,y,-0.29);
        glEnd();
        glDisable(GL_TEXTURE_2D);
   }


 // draw floor 
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D,floorTexture);
   glBegin(GL_QUADS);
   glNormal3f(0,-1,0); //positive y-axis
   glTexCoord2f(0,0);glVertex3f(-1,y+0.13,+1);
   glTexCoord2f(tex_x,0);glVertex3f(+1,y+0.13,+1);
   glTexCoord2f(tex_x,tex_y);glVertex3f(+1,y+0.13,-1);
   glTexCoord2f(0,tex_y);glVertex3f(-1,y+0.13,-1);
   glEnd();
   glDisable(GL_TEXTURE_2D);


   
   
// Inner walls - adding inner walls to add different texture to interior

   
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D,interiorWall);
   
   // Inside wall (front) of the cabin along X-axis
   glBegin(GL_QUADS);

   // interior back wall along X-axis
   glTexCoord2f(0,0);glVertex3f(+1,y,+0.99);
   glTexCoord2f(tex_x,0);glVertex3f(-1,y,+0.99);
   glTexCoord2f(tex_x,tex_y);glVertex3f(-1,+1,+0.99);
   glTexCoord2f(0,tex_y);glVertex3f(+1,+1,+0.99);

   // interior back wall along X-axis
   glTexCoord2f(0,0);glVertex3f(+1,y,-0.99);
   glTexCoord2f(tex_x,0);glVertex3f(-1,y,-0.99);
   glTexCoord2f(tex_x,tex_y);glVertex3f(-1,+1,-0.99);
   glTexCoord2f(0,tex_y);glVertex3f(+1,+1,-0.99);


   // Inside wall section without door (along Z-axis)
   glTexCoord2f(0,0);glVertex3f(-0.99,y,-1);
   glTexCoord2f(tex_x,0);glVertex3f(-0.99,y,+1);
   glTexCoord2f(tex_x,tex_y);glVertex3f(-0.99,+1,+1);
   glTexCoord2f(0,tex_y);glVertex3f(-0.99,+1,-1);

    // Inside wall section with the door (along Z-axis)
   glTexCoord2f(0,0);glVertex3f(+0.99,y,+1);
   glTexCoord2f(tex_x,0);glVertex3f(+0.99,y,-0.3);
   glTexCoord2f(tex_x,tex_y);glVertex3f(+0.99,+1,-0.3);
   glTexCoord2f(0,tex_y);glVertex3f(+0.99,+1,+1);

   

   glEnd();
   glDisable(GL_TEXTURE_2D);

    object = COT;
    // Bedframe inside cabin base
    glPushMatrix();
	glTranslated(x,y,z);
	glScaled(dx, dy, dz);
	cube(x+5.3,y+0.3,z-0.3,0.23,0.03,0.95,0);
	glPopMatrix();
	

    object = MATTRESS;
    //Mattress inside cabin
    glPushMatrix();
	glTranslated(x,y,z);
	glScaled(dx, dy, dz);
	cube(x+5.3,y+0.40,z-0.3,0.2,0.04,0.8,0);
	glPopMatrix();
	

	//chair 
    object = CHAIR;
	glPushMatrix();
	glTranslated(x,y,z);
	glScaled(dx, dy, dz);
	cube(x+5.7,y+0.5,z+0.8,0.05,0.05,0.4,0);
	cube(x+5.7,+0.7,z+1.1,0.05,0.2,0.1,0);
    //chair legs
    cube(x+5.745,y+0.3,z+1.15,0.01,0.2,0.05,0);
    cube(x+5.655,y+0.3,z+1.15,0.01,0.2,0.05,0);
    cube(x+5.745,y+0.3,z+0.45,0.01,0.2,0.05,0);
    cube(x+5.655,y+0.3,z+0.45,0.01,0.2,0.05,0);
   	glPopMatrix();
   
   
   glPopMatrix();
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D,roofTexture);
   drawRoof(x,y+dy,z,dx,dy,dz,th);
   glDisable(GL_TEXTURE_2D);



   
   cylinder(x,y+1.45,z,0,90,0.003,0.45);
   // cabin light
   glColor3f(1.0, 1.0, 0);
   ball(x,y+1,z,0.05);

}

/**
 * @brief convenience function to draw a picnic bench
 *        at (x,y,z) of size dx,dy,dz rotated
 *        th about the Y-axis
 */

static void drawViewBench(double x, double y, double z, 
                 double dx, double dy, double dz,
                 double th)
{
    glPushMatrix();
    glTranslated(x,y,z);
    glRotated(th,0,1,0);
    glScaled(dx,dy,dz);
    // glColor3f(1,1,1);
    //bench base
    object = BENCH;
    cube(x,y,z,0.25,0.1,0.4,0);
    //bench legs
    cube(x+0.05,y-0.23,z+0.18,0.05,0.35,0.05,0);
    cube(x+0.05,y-0.23,z-0.18,0.05,0.35,0.05,0);
   	glPopMatrix();
}

/**
 * @brief convenience function to draw a forest.
 * 
 */
static void drawforest(void)
{

    float x, z; // x - to traverse along the X-axis and z- to traverse along the Y-axis
    const float y = 0.6;
    for(z = 2.0; z >= -1.7; z = z-0.25)
    {
        for(x = 1.8; x <= 4; x = x+0.2)
        {
            if(z >= 0.25)
                continue;
            tree(x,y,z,0.1,0.5);
        }
        for(x = -4; x <= -1.4; x = x+0.2)
        {
            if((z <= 0.6) && (z >= -0.5))
                continue;
            tree(x,y,z,0.1,0.5);
        }
    }

}

/**
 * @brief convenience function to generate 
 *        circles.
 * https://stackoverflow.com/questions/22444450/drawing-circle-with-opengl
 */
void generateCircles()
{ 
	double x_vertex, y_vertex;
    double waves_origin = 0, sine_value, cosine_value;
	waves_origin = waves_origin - NUM_CIRCLES/2.0;
	for(int i=0; i < NUM_CIRCLES; i++)
	{
		y_vertex = waves_origin+i;
        
		for(int j=0; j < NUM_CIRCLES; j++)
		{
			x_vertex = waves_origin+j;
			sine_value = (x_vertex*x_vertex) + (y_vertex*y_vertex)- angle;
			cosine_value = (x_vertex*x_vertex) + (y_vertex*y_vertex) - angle;

            //generate vertices
			vertex[i][j][0] = x_vertex;
			vertex[i][j][1] = y_vertex;
			vertex[i][j][2] = sin(sine_value/120);

            //generate normals
			normal[i][j][0] = x_vertex * cos(cosine_value/120);
			normal[i][j][1] = y_vertex * cos(cosine_value/120);
			normal[i][j][2] = 1;
		}
	}
}

void placeNumWaves()
{ 
	for(int i=0; i<NUM_CIRCLES-1; i++)
    {
		for(int j=0; j<NUM_CIRCLES-1; j++)
		{ 
			glBegin(GL_QUADS);

            //draw based on normals
            glNormal3dv(normal[i][j]);
            glNormal3dv(normal[i][1+j]);
            glNormal3dv(normal[1+i][1+j]);
            glNormal3dv(normal[1+i][j]);

            //draw based on vertices
			glVertex3dv(vertex[i][j]);
			glVertex3dv(vertex[i][1+j]);
			glVertex3dv(vertex[1+i][1+j]);
			glVertex3dv(vertex[1+i][j]);

			glEnd();
		}	
	}
}



/*
* Draw a fence at x,y,z 
* of length dx, height dy and width dz
* rotated th about the Y-axis
*
*/
void drawFence(double x, double y, double z,
               double dx, double dy, double dz,
               double th)
{
    glPushMatrix();
    glTranslated(x,y,z);
    glRotated(th,0,1,0);
    glScaled(dx,dy,dz);
     // Define material for specular and emission
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white); // specular component as white
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black); // emission component as black

   glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,fenceTexture);
   glBegin(GL_QUADS);

   glColor3f(1,1,1);
   // Front of lake(along X)
   glNormal3f(0,0,+1);
   glTexCoord2f(0,0); glVertex3f(-1,-1,1.5);
   glTexCoord2f(8,0); glVertex3f(+1,-1,1.5);
   glTexCoord2f(8,6); glVertex3f(+1,+1,1.5);
   glTexCoord2f(0,6); glVertex3f(-1,+1,1.5);
   
    // Back of lake(along X)
   glNormal3f(0,0,-1);
   glTexCoord2f(0,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(8,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(8,6); glVertex3f(-1,+1,-1);
   glTexCoord2f(0,6); glVertex3f(+1,+1,-1);
   
   // Right side of lake
   glNormal3f(+1,0,0);
   glTexCoord2f(0,0); glVertex3f(+1,-1,+1.5);
   glTexCoord2f(8,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(8,6); glVertex3f(+1,+1,-1);
   glTexCoord2f(0,6); glVertex3f(+1,+1,+1.5);
   
   // Left side of lake
   glNormal3f(-1,0,0);
   glTexCoord2f(0,0); glVertex3f(-1,-1,+1.5);
   glTexCoord2f(8,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(8,6); glVertex3f(-1,+1,-1);
   glTexCoord2f(0,6); glVertex3f(-1,+1,+1.5);
   glEnd();

   glDisable(GL_TEXTURE_2D);
   
   glPopMatrix();
}

/*
* Draw a fence at x,y,z 
* of length dx, height dy and width dz
* rotated th about the Y-axis
*
*/
void drawBridge(double x, double y, double z,
               double dx,double dy, double dz,
               double th)
{

    glPushMatrix();
    glTranslated(x,y,z);
    glRotated(th,0,1,0);
    glScaled(dx,dy,dz);


     // Define material for specular and emission
    glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white); // specular component as white
    glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black); // emission component as black


    object = BRIDGE;
    cube(x,y,z,0.9,0.2,0.3,0);

    glPopMatrix();

      //right side
    cube(1.3,0.13,0,0.05,0.04,0.2,0);
    cube(1.2,0.21,0,0.05,0.04,0.2,0);
    cube(1.1,0.29,0,0.05,0.04,0.2,0);
    cube(1.0,0.37,0,0.05,0.04,0.2,0);
    cube(0.9,0.45,0,0.06,0.04,0.2,0);
    cube(0.8,0.533,0,0.05,0.05,0.2,0);

    //left side
    cube(-1.3,0.13,0,0.05,0.04,0.2,0);
    cube(-1.2,0.21,0,0.05,0.04,0.2,0);
    cube(-1.1,0.29,0,0.05,0.04,0.2,0);
    cube(-1.0,0.37,0,0.05,0.04,0.2,0);
    cube(-0.9,0.45,0,0.06,0.04,0.2,0);
    cube(-0.8,0.533,0,0.05,0.05,0.2,0);
}

/**
 * @brief convenience function to draw waves in the lake
 *        at (x,y,z) of size dx,dy,dz. 
 *  
 */
void drawWaves(double x, double y, double z,
               double dx,double dy, double dz,
               double th)
{
    float diffuse[] = {0.18,0.16,0.9,1.0};
	float specular[] = {1.0,1.0,1.0,1.0};
	float shininess[] = {10.0};   
	glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,diffuse);
	glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,specular);
	glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shininess);
	
	glPushMatrix();
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glTranslated(x, y, z);
	glScaled(dx, dy, dz);
	glRotatef(th, 1, 0.0, 0.0);	 
	generateCircles();
	placeNumWaves();
	glDisable(GL_LIGHTING);
	glPopMatrix();

}


/**
 * @brief convience function to create
 *        the entire scence
 * 
 */
void renderScene(void)
{
 
    // Draw a dirt road as base
    object = DIRTROAD;
    cube(-0.25,0,0,4.6,0.1,2,0); 


    // Draw forest trees on L and R side of the scene
    drawforest();

    //Draw a water like body in the center of the scene
    object = LAKE;
    cube(0,0.1,0,0.8,0.05,1.75,0);

   // fence around the lake
   drawFence(0,0.2,-0.35,0.85,0.05,1.5,0);

   
    fracMountain(-3,0.0,-3,  1.8,0.0,-3,  2.0,0.0,-3,  3.0,0.0,-3);

    // cabin house along with interior
    glColor3f(1,1,1);
    drawCabin(-3.4,0,0,1.5,0.8,0.55,0);

    // bridge across the lake
    glColor3f(1.0,1.0,1.0);
    drawBridge(0,0.5,0,0.85,0.1,0.75,0);

    //bench for view point
    drawViewBench(1.2,0.22,0.35,0.4,0.3,0.7,-30);

    // trees around the bench view point
    createTree(3.8,0,1,0.5,0.05,0.1);
    createTree(-1.1,0,0.3,0.5,0.05,0.2);
    createTree(-1.1,0,0.3,0.4,0.05,0.3);

    //waves
    drawWaves(-1.6,0.145,-1.6,0.0053,0.013,0.0116,90);

}



/* function called by GLUT to display the scene */
void display(void)
{
    double Ex, Ey, Ez;
    
    /* clear screen and Z-Buffer*/
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Enable Z-buffering in OpenGL*/
    glEnable(GL_DEPTH_TEST);

    /* reset transformation - clears changes made earlier */
    glLoadIdentity();

   
    switch(mode)
    {
        case ORTHOGONAL: // world orientation
            glRotatef(ph,1,0,0);
            glRotatef(th,0,1,0);
            //  Display parameters
            glColor3f(1.0,1.0,1.0);
            glWindowPos2i(5,5);
            Print("Angle= x: %d,y :%d   Projection=%s",th,ph,text[mode]);
            break;
        case PERSPECTIVE: // eye position
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
            
            MX = +2*dim*Sin(th_FP)*Cos(ph_FP);
            MY = -2*dim*Sin(ph_FP);
            MZ = -2*dim*Cos(th_FP)*Cos(ph_FP);
            // imitate first person movement
            gluLookAt(EX,EY,EZ,EX+MX,EY+MY,EZ+MZ,
                    0,Cos(ph_FP),0);                    
            glColor3f(1.0,1.0,1.0);
            glWindowPos2i(5,5);
            Print("Angle=%d,%d  Position=%.1f,%.1f  Dim:%.1f Projection=%s",th_FP,ph_FP,EX,EZ,dim,text[mode]);
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
        ball(Position[0],Position[1],Position[2],0.1); //radius 0.1

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


    
    renderScene();

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

     angle += angle_increment;
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
                if(mode == FIRSTPERSON)
                {
                    th_FP = (th_FP + 1)%360;
                }
                else{
                    th = (th + 5) % 360;
                }
                break;
            //  Left arrow key - decrease angle by 5 degrees
            case GLUT_KEY_LEFT:
                if(mode == FIRSTPERSON)
                {
                   th_FP = (th_FP - 1) % 360;
                }
                else {
                    th = (th - 5) % 360;
                }
                break;
            //  Up arrow key - increase elevation by 5 degrees
            case GLUT_KEY_UP:
                if(mode == FIRSTPERSON)
                {
                     EX -= -Sin(th_FP)*Cos(ph_FP); // Update Coordinates
			         EZ -= Cos(th_FP)*Cos(ph_FP);
                }
                else{
                ph = (ph - 5) % 360;
                }
                break;
            //  Down arrow key - decrease elevation by 5 degrees
            case GLUT_KEY_DOWN:
                if(mode == FIRSTPERSON)
                {
                     EX += -Sin(th_FP)*Cos(ph_FP); // Update Coordinates
			         EZ += Cos(th_FP)*Cos(ph_FP);
                }
                else{
                ph = (ph + 5) % 360;
                }
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

    if(mode == ORTHOGONAL)
    {
        Project(0,asp,dim);
    }
    else
    {
        Project(fov,asp,dim);
    }
    /* update the display */
    glutPostRedisplay();
}


/* function called by GLUT when a key is pressed*/
void key(unsigned char ch,int x, int y)
{
   //  Exit on ESC
  if (ch == 27)
  {
    exit(0);
  }
  else if(ch == '1')// go to bench view
  {
      mode = FIRSTPERSON;
      th_FP = -30;
      ph_FP = 0;
      EX = 2.0;
      EZ = 1.8;
      dim = 5.0;
  }
  else if(ch == '2') // press for cabin view
  {
      mode = FIRSTPERSON;
      th_FP = -107.0;
      ph_FP = 0;
      EX = -2.6;
      EZ = -0.3;
      dim = 5.0;
  }

  else if(ch == 'x' || ch == 'X') // toggle Axes
  {
      axes = 1-axes;
  }
  else if(ch == 'q')// open the door
  {
      doorOpen = 1-doorOpen;
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
        th = -25; ph = 15;
        fov = 60; 
        asp = 1.0;
        dim = 5.0;


    }
    //  Change field of view angle
    else if (ch == '-' && ch>1)
    {
      fov--;
    }
    else if (ch == '+' && ch<179) //1 FP 2 PERSPECTIVE // orthogonal
    {
      fov++;
    }
  }
  else //first person
  {
    if (ch == '0')
    {
        th_FP = -30; ph_FP =0;
        dim = 5.0;
        EX = 2.0;
        EZ = 1.8;
    }
    else if(ch == 'w' || ch == 'W')
    {
        EX += Cos(ph_FP)/2;
        EY += Cos(ph_FP)/2;
    }
    else if(ch == 's' || ch == 'S')
    {
        EX -= Cos(ph_FP)/2;
        EY -= Cos(ph_FP)/2;
    }
  }
    //  Translate shininess power to value (-1 => 0)
    shiny = shininess<0 ? 0 : pow(2.0,shininess);

    if(mode == ORTHOGONAL)
    {
        Project(0,asp,dim);
    }
    else
    {
        Project(fov,asp,dim);
    }
    /* update the display */
    glutIdleFunc(move?idle:NULL);
    glutPostRedisplay();
}


/* function called by GLUT when window is resized */
void reshape(int width,int height)
{
   //  Ratio of the width to the height of the window
   asp = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //  Set projection
   if(mode == 0)
		Project(0,asp,dim);
   else
	   Project(fov,asp,dim);
}



int main(int argc,char* argv[]) 
{
 
    /* initialize OpenGL Utility Tool */
    glutInit(&argc,argv);

    /* Request double bufferred true color window with Z-buffering */
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH); 

    glutInitWindowPosition(300,300);

    /* Request 840 x 840 pixel window */
    glutInitWindowSize(840,840);

    /* Create Window */
    glutCreateWindow("Final Project : Cabin in the Woods");

#ifdef USEGLEW
   //  Initialize GLEW
   if (glewInit()!=GLEW_OK) Fatal("Error initializing GLEW\n");
#endif
    /* register callback for displaying scene*/
    glutDisplayFunc(display);
    /* register callback for reshaping window*/
    glutReshapeFunc(reshape);
    /* register callback for idle */
    glutIdleFunc(idle);
    /* register callback for special key presses */
    glutSpecialFunc(special);
    /* register callback for key presses*/
    glutKeyboardFunc(key);

    // Load the textures
   
    roadTexture = LoadTexBMP("images/soil.bmp");
    woodTexture = LoadTexBMP("images/bark.bmp");
    leafTexture = LoadTexBMP("images/treeleaves.bmp");
    lakeTexture = LoadTexBMP("images/lake.bmp");
    fenceTexture = LoadTexBMP("images/fence.bmp");

    roofTexture = LoadTexBMP("images/roof.bmp");
    exteriorWall = LoadTexBMP("images/brick.bmp");
    interiorWall = LoadTexBMP("images/housewall.bmp");
    cotWoodTexture = LoadTexBMP("images/cotwood.bmp");
    mattressTexture = LoadTexBMP("images/mattress.bmp");
    chairTexture = LoadTexBMP("images/chair.bmp");
    floorTexture = LoadTexBMP("images/floor.bmp");
    doorTexture = LoadTexBMP("images/door.bmp");

    benchTexture = LoadTexBMP("images/parkbench.bmp");
    bridgeTexture = LoadTexBMP("images/bridge.bmp");

     
    
    /* pass control for GLUT for events */
    glutMainLoop();

    /* return to OS */
    return 0;

}
