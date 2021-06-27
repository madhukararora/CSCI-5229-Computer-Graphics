/* CSCI 5229 Computer Graphics
* University of Colorado Boulder
* @author : Madhukar Arora
* Final Project Summer 2021
*/

/*
* Disclaimer:
  I've borrowed most of the source code from the references provided in the class and other resource online.
  Links to referred material:

*/


/* Includes */
#include "CSCIx229.h"
#include <time.h>
#include <limits.h>
#include <float.h>

//Globals
int th = 0;          //  Azimuth of view angle
int ph = 30;          //  Elevation of view angle
int axes=1;        //  Display axes
int fov = 58;      //  field of view for perspective

double asp = 1;    //  aspect ratio
double dim = 10;  // dimension of orthogonal box
double len = 2.0;  //length of axes

//parameters for first person view

/* where the camera is looking*/
double MX = 0;
double MY = 0;
double MZ = 0;

/* camera position*/
double EX = 0; 
double EY = -2.0;
double EZ = -2.0;

/* different modes*/
const char* text[] = {"Orthogonal","Perspective","First Person"};

typedef enum{ ORTHOGONAL = 0, PERSPECTIVE,FIRSTPERSON}MODE_T;
MODE_T mode = ORTHOGONAL;

// lighting parameters 
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

const float white[] = {1,1,1,1};
const float black[] = {0,0,0,1};



// texture parameters
unsigned int exteriorWall;
unsigned int interiorWall;
unsigned int roadTexture;
unsigned int roofTexture;
unsigned int leafTexture;
unsigned int woodTexture;








// //           Math

// typedef enum { NOTALLOWED, MOUNTAIN, TREE, ISLAND, BIGMTN, STEM, LEAF, 
//                MOUNTAIN_MAT, WATER_MAT, LEAF_MAT, TREE_MAT, STEMANDLEAVES,
//                AXES } DisplayLists;


// #define MAXLEVEL 8
// #define NUMRANDS 191
// int Level = 4;


//   /* normalizes v */
// void normalize(GLfloat v[3])
// {
//   GLfloat d = sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);

//   if (d == 0)
//     fprintf(stderr, "Zero length vector in normalize\n");
//   else{
//     v[0] /= d; 
//     v[1] /= d; 
//     v[2] /= d;
//   }
// }

// float xzlength(float v1[3], float v2[3])
// {
//   return sqrt((v1[0] - v2[0])*(v1[0] - v2[0]) +
//               (v1[2] - v2[2])*(v1[2] - v2[2]));
// }
//   /* calculates a normalized crossproduct to v1, v2 */
// void ncrossprod(float v1[3], float v2[3], float cp[3])
// {
//   cp[0] = v1[1]*v2[2] - v1[2]*v2[1];
//   cp[1] = v1[2]*v2[0] - v1[0]*v2[2];
//   cp[2] = v1[0]*v2[1] - v1[1]*v2[0];
//   normalize(cp);
// }


//   /* calculates normal to the triangle designated by v1, v2, v3 */
// void triagnormal(float v1[3], float v2[3], float v3[3], float norm[3])
// {
//   float vec1[3], vec2[3];

//   vec1[0] = v3[0] - v1[0];  vec2[0] = v2[0] - v1[0];
//   vec1[1] = v3[1] - v1[1];  vec2[1] = v2[1] - v1[1];
//   vec1[2] = v3[2] - v1[2];  vec2[2] = v2[2] - v1[2];

//   ncrossprod(vec2, vec1, norm);
// }


// float xzslope(float v1[3], float v2[3])
// {
//   return ((v1[0] != v2[0]) ? ((v1[2] - v2[2]) / (v1[0] - v2[0]))
// 	                   : FLT_MAX);
// }


// //                           MOUNTAIN STUFF
// GLfloat DispFactor[MAXLEVEL];  /* Array of what to multiply random number
// 				  by for a given level to get midpoint
// 				  displacement  */

// GLfloat DispBias[MAXLEVEL];  /* Array of what to add to random number
// 				before multiplying it by DispFactor */


// float RandTable[NUMRANDS];  /* hash table of random numbers so we can
// 			       raise the same midpoints by the same amount */ 



//          /* The following are for permitting an edge of a moutain to be   */
//          /* pegged so it won't be displaced up or down.  This makes it    */
//          /* easier to setup scenes and makes a single moutain look better */

// GLfloat Verts[3][3],    /* Vertices of outside edges of mountain */
//         Slopes[3];      /* Slopes between these outside edges */
// int     Pegged[3];      /* Is this edge pegged or not */   





//  /*
//   * Comes up with a new table of random numbers [0,1)
//   */
// void InitRandTable(unsigned int seed)
// {
//   int i;

//   srand48((long) seed);
//   for (i = 0; i < NUMRANDS; i++)
//     RandTable[i] = drand48() - 0.5;
// }

//   /* calculate midpoint and displace it if required */
// void Midpoint(GLfloat mid[3], GLfloat v1[3], GLfloat v2[3],
// 	      int edge, int level)
// {
//   unsigned hash;

//   mid[0] = (v1[0] + v2[0]) / 2;
//   mid[1] = (v1[1] + v2[1]) / 2;
//   mid[2] = (v1[2] + v2[2]) / 2;
//   if (!Pegged[edge] || (fabs(xzslope(Verts[edge], mid) 
//                         - Slopes[edge]) > 0.00001)) {
//     srand48((int)((v1[0]+v2[0])*23344));
//     hash = drand48() * 7334334;
//     srand48((int)((v2[2]+v1[2])*43433));
//     hash = (unsigned)(drand48() * 634344 + hash) % NUMRANDS;
//     mid[1] += ((RandTable[hash] + DispBias[level]) * DispFactor[level]);
//   }
// }


//   /*
//    * Recursive moutain drawing routine -- from lecture with addition of 
//    * allowing an edge to be pegged.  This function requires the above
//    * globals to be set, as well as the Level global for fractal level 
//    */
// void FMR(GLfloat v1[3], GLfloat v2[3], GLfloat v3[3], int level)
// {
//   printf("levl %d\n",level);
//   if (level == Level) {
//     GLfloat norm[3];

//     triagnormal(v1, v2, v3, norm);
//     glNormal3fv(norm);
//     glVertex3fv(v1);
//     glVertex3fv(v2);
//     glVertex3fv(v3);

//   } else {
//     GLfloat m1[3], m2[3], m3[3];

//     Midpoint(m1, v1, v2, 0, level);
//     Midpoint(m2, v2, v3, 1, level);
//     Midpoint(m3, v3, v1, 2, level);

//     FMR(v1, m1, m3, level + 1);
//     FMR(m1, v2, m2, level + 1);
//     FMR(m3, m2, v3, level + 1);
//     FMR(m1, m2, m3, level + 1);
//   }
// }

// /*
//   * sets up lookup tables and calls recursive mountain function
//   */
// void FractalMountain(GLfloat v1[3], GLfloat v2[3], GLfloat v3[3],
//                      int pegged[3])
// {
//   GLfloat lengths[MAXLEVEL];
//   GLfloat fraction[8] = { 0.3, 0.3, 0.4, 0.2, 0.3, 0.2, 0.4, 0.4  };
//   GLfloat bias[8]     = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1  };
//   int i;
//   float avglen = (xzlength(v1, v2) + 
//                   xzlength(v2, v3) +
// 		  xzlength(v3, v1) / 3);

//   for (i = 0; i < 3; i++) {
//     Verts[0][i] = v1[i];      /* set mountain vertex globals */
//     Verts[1][i] = v2[i];
//     Verts[2][i] = v3[i];
//     Pegged[i] = pegged[i];
//   }

//   Slopes[0] = xzslope(Verts[0], Verts[1]);   /* set edge slope globals */
//   Slopes[1] = xzslope(Verts[1], Verts[2]);
//   Slopes[2] = xzslope(Verts[2], Verts[0]);

//   lengths[0] = avglen;          
//   for (i = 1; i < Level; i++) {   
//     lengths[i] = lengths[i-1]/2;     /* compute edge length for each level */
//   }

//   for (i = 0; i < Level; i++) {     /* DispFactor and DispBias arrays */      
//     DispFactor[i] = (lengths[i] * ((i <= 7) ? fraction[i] : fraction[7]));
//     DispBias[i]   = ((i <= 7) ? bias[i] : bias[7]);
//   } 

//   glBegin(GL_TRIANGLES);
//   glColor3f(1.0,1.0,1.0);
//     FMR(v1, v2, v3, 0);    /* issues no GL but vertex calls */
//   glEnd();
// }


//  /*
//   * draw a mountain and build the display list
//   */
// void CreateMountain(void)
// {
//   GLfloat v1[3] = { 0, 0, -2 }, v2[3] = { -2, 0, 1 }, v3[3] = { 2, 0, 1 };
//   int pegged[3] = { 1, 1, 1 };

//   glNewList(MOUNTAIN, GL_COMPILE);
//   glPushAttrib(GL_LIGHTING_BIT);
//     glCallList(MOUNTAIN_MAT);
//     FractalMountain(v1, v2, v3, pegged);
//   glPopAttrib();
//   glEndList();
// }

//   /*
//    * new random numbers to make a different moutain
//    */
// void NewMountain(void)
// {
//   InitRandTable(time(NULL));
// }


// void NewFractals(void)
// {
//   NewMountain();
// }


// void SetupMaterials(void)
// {
//   GLfloat mtn_ambuse[] =   { 0.426, 0.256, 0.108, 1.0 };
//   GLfloat mtn_specular[] = { 0.394, 0.272, 0.167, 1.0 };
//   GLfloat mtn_shininess[] = { 10 };

//   GLfloat water_ambuse[] =   { 0.0, 0.1, 0.5, 1.0 };
//   GLfloat water_specular[] = { 0.0, 0.1, 0.5, 1.0 };
//   GLfloat water_shininess[] = { 10 };

//   GLfloat tree_ambuse[] =   { 0.4, 0.25, 0.1, 1.0 };
//   GLfloat tree_specular[] = { 0.0, 0.0, 0.0, 1.0 };
//   GLfloat tree_shininess[] = { 0 };

//   GLfloat leaf_ambuse[] =   { 0.0, 0.8, 0.0, 1.0 };
//   GLfloat leaf_specular[] = { 0.0, 0.8, 0.0, 1.0 };
//   GLfloat leaf_shininess[] = { 10 };

//   glNewList(MOUNTAIN_MAT, GL_COMPILE);
//     glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mtn_ambuse);
//     glMaterialfv(GL_FRONT, GL_SPECULAR, mtn_specular);
//     glMaterialfv(GL_FRONT, GL_SHININESS, mtn_shininess);
//   glEndList();

//   glNewList(WATER_MAT, GL_COMPILE);
//     glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, water_ambuse);
//     glMaterialfv(GL_FRONT, GL_SPECULAR, water_specular);
//     glMaterialfv(GL_FRONT, GL_SHININESS, water_shininess);
//   glEndList();

//   glNewList(TREE_MAT, GL_COMPILE);
//     glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, tree_ambuse);
//     glMaterialfv(GL_FRONT, GL_SPECULAR, tree_specular);
//     glMaterialfv(GL_FRONT, GL_SHININESS, tree_shininess);
//   glEndList();

//   glNewList(LEAF_MAT, GL_COMPILE);
//     glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, leaf_ambuse);
//     glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, leaf_specular);
//     glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, leaf_shininess);
//   glEndList();
// }

// void myGLInit(void)
// {
//   GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
//   GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
//   GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
//   GLfloat light_position[] = { 0.0, 0.3, 0.3, 0.0 };

//   GLfloat lmodel_ambient[] = { 0.4, 0.4, 0.4, 1.0 };

//   glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
//   glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
//   glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
//   glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    
//   glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);

//   glEnable(GL_LIGHTING);
//   glEnable(GL_LIGHT0);

//   glDepthFunc(GL_LEQUAL);
//   glEnable(GL_DEPTH_TEST);

//   glEnable(GL_NORMALIZE);
// #if 0
//   glEnable(GL_CULL_FACE);
//   glCullFace(GL_BACK);
// #endif

//   glShadeModel(GL_SMOOTH);
// #if 0
//   glEnable(GL_BLEND);
//   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
// #endif

//   SetupMaterials();
 

//   glFlush();
// } 







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
   
   glPopMatrix();//  Undo transofrmations
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


///////////////////////////////////////////////////SHAPES////////////////////////////////////////////////////////////////

// TO DO : add textures based on different objects that depend on cube as primary.
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
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black); // emission component as black
   
  
   glPushMatrix(); //  Save transformation
   glTranslated(x,y,z); //  Offset
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);

   
   glBegin(GL_QUADS);  // cube
   //  Front
   glNormal3f( 0, 0, 1);
   glTexCoord2f(0,0);glVertex3f(-1,-1, 1);
   glTexCoord2f(1,0);glVertex3f(+1,-1, 1);
   glTexCoord2f(1,1);glVertex3f(+1,+1, 1);
   glTexCoord2f(0,1);glVertex3f(-1,+1, 1);
   
   //  Back
   glNormal3f( 0, 0,-1);
   glTexCoord2f(0,0);glVertex3f(+1,-1,-1);
   glTexCoord2f(1,0);glVertex3f(-1,-1,-1);
   glTexCoord2f(1,1);glVertex3f(-1,+1,-1);
   glTexCoord2f(0,1);glVertex3f(+1,+1,-1);
   
   //  Right
   glNormal3f(+1, 0, 0);
   glTexCoord2f(0,0);glVertex3f(+1,-1,+1);
   glTexCoord2f(1,0);glVertex3f(+1,-1,-1);
   glTexCoord2f(1,1);glVertex3f(+1,+1,-1);
   glTexCoord2f(0,1);glVertex3f(+1,+1,+1);
   
   //  Left
   glNormal3f(-1, 0, 0);
   glTexCoord2f(0,0);glVertex3f(-1,-1,-1);
   glTexCoord2f(1,0);glVertex3f(-1,-1,+1);
   glTexCoord2f(1,1);glVertex3f(-1,+1,+1);
   glTexCoord2f(0,1);glVertex3f(-1,+1,-1);
  
   //  Top
   glNormal3f( 0,+1, 0);
   glTexCoord2f(0,0);glVertex3f(-1,+1,+1);
   glTexCoord2f(1,0);glVertex3f(+1,+1,+1);
   glTexCoord2f(1,1);glVertex3f(+1,+1,-1);
   glTexCoord2f(0,1);glVertex3f(-1,+1,-1);
   
   //  Bottom
   glNormal3f( 0,-1, 0);
   glTexCoord2f(0,0);glVertex3f(-1,-1,-1);
   glTexCoord2f(1,0);glVertex3f(+1,-1,-1);
   glTexCoord2f(1,1);glVertex3f(+1,-1,+1);
   glTexCoord2f(0,1);glVertex3f(-1,-1,+1);
   
   //  End
   glEnd();
   //  Undo transformations
   glPopMatrix();
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
   glTexCoord2f(0.5,0);glVertex3f(B.x,B.y,B.z);
   glTexCoord2f(0,1.5);glVertex3f(C.x,C.y,C.z);
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

/**
 * @brief draw a house at (x,y,z) of width, height and length
 *        rotate th about the y-axis
 * @param x 
 * @param y 
 * @param z 
 * @param width 
 * @param height 
 * @param length 
 * @param th 
 */
static void house(double x,double y, double z, 
                  double width, double height, double length,
                  double th)
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,exteriorWall);
    cube(x,y,z,width,height,length,th);
    glDisable(GL_TEXTURE_2D);
    
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,roofTexture);
    drawRoof(x,y+height,z,width,height/2,length+0.1,th);
    glDisable(GL_TEXTURE_2D);
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
    glColor3f(0.53,0.16,0.02); //brown
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,woodTexture);
    cylinder(x,y,z,0,90,radius/5,height);
    glDisable(GL_TEXTURE_2D);

    //leaves
    glColor3f(0,0.5,0);  // green
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,leafTexture);
    cone(x,y+height/2,z,0,-90,radius,height);
    glDisable(GL_TEXTURE_2D);
    
    
}


/**
 * @brief  create  a dirt road path as base
 *         at (x,y,z) of width dx, height dy, and length dx
 *         rotated th about the y-axis
 * 
 */
static void path(double x, double y, double z, 
                 double dx, double dy, double dz,
                 double th)
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,roadTexture);
    cube(x,y,z,dx,dy,dz,th);
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
    int tex_x = 3, tex_y = 1;
    
    glPushMatrix();
    glTranslated(x,y,z);
    glRotated(th,0,1,0);
    glScaled(dx,dy,dz);

   // Define material for specular and emission
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white); // specular component as white
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black); // emission component as black



   // TO DO: add textures to the cabin

   // Add wooden wall texture to outside walls
   //OUTSIDE : front wall of the cabin  (along X-axis)
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D,exteriorWall);


   // front
   glBegin(GL_QUADS);
   //glColor3f(0.85,0.53,0.1);

   //L of window                             
   glNormal3f(0,0,1);
   glTexCoord2f(0,0);glVertex3f(-1,y,1);
   glTexCoord2f(-0.6,0);glVertex3f(-0.6,y, 1);
   glTexCoord2f(-0.6,1);glVertex3f(-0.6,+1, 1);
   glTexCoord2f(0,1);glVertex3f(-1,+1, 1);

   // R of window
   glNormal3f(0,0,1);
   glTexCoord2f(0,0);glVertex3f(0.6,y,1);
   glTexCoord2f(tex_x,0);glVertex3f(+1,y, 1);
   glTexCoord2f(tex_x,tex_y);glVertex3f(+1,+1, 1);
   glTexCoord2f(0,tex_y);glVertex3f(0.6,+1, 1);

   // Below window section of wall
   glNormal3f(0,0,1);
   glTexCoord2f(0,0);glVertex3f(-0.6,y,1);
   glTexCoord2f(tex_x,0);glVertex3f(+0.6,y, 1);
   glTexCoord2f(tex_x,tex_y);glVertex3f(+0.6,-0.55,1);
   glTexCoord2f(0,tex_y);glVertex3f(-0.6,-0.55,1);

   // above window section of wall
   glNormal3f(0,0,1);
   glTexCoord2f(0,0);glVertex3f(-0.6,0.5,1);
   glTexCoord2f(tex_x,0);glVertex3f(+0.6,0.5,1);
   glTexCoord2f(tex_x,tex_y);glVertex3f(+0.6,+1, 1);
   glTexCoord2f(0,tex_y);glVertex3f(-0.6,+1,1);

   
   //OUTSIDE : back wall of the cabin (along X-axis)
   glNormal3f( 0, 0, -1);
   glTexCoord2f(0,0);glVertex3f(+1,y,-1);
   glTexCoord2f(tex_x,0);glVertex3f(-1,y,-1);
   glTexCoord2f(tex_x,tex_y);glVertex3f(-1,+1,-1);
   glTexCoord2f(0,tex_y);glVertex3f(+1,+1,-1);

   //OUTSIDE : right side of the wall (along Z-axis)
   glNormal3f( 1, 0, 0);
   glTexCoord2f(0,0);glVertex3f(+1,y,+1);
   glTexCoord2f(tex_x,0);glVertex3f(+1,y,-1);
   glTexCoord2f(tex_x,tex_y);glVertex3f(+1,+1,-1);
   glTexCoord2f(0,tex_y);glVertex3f(+1,+1,+1);
   
   glEnd();


   // Outside wall section without door (along Z-axis)
   glBegin(GL_QUADS);
   glColor3f(0.85,0.53,0.1);
   glNormal3f(-1, 0, 0);
   glTexCoord2f(0,0);glVertex3f(-1,y,-1);
   glTexCoord2f(tex_x,0);glVertex3f(-1,y,0.5);
   glTexCoord2f(tex_x,tex_y);glVertex3f(-1,+1,0.5);
   glTexCoord2f(0,tex_y);glVertex3f(-1,+1,-1);

   // Outside wall section above the door (along Z-axis)
   glNormal3f(-1, 0, 0);
   glTexCoord2f(0,0);glVertex3f(-1,0.5,0.5);
   glTexCoord2f(tex_x,0);glVertex3f(-1,0.5,+1);
   glTexCoord2f(tex_x,tex_y);glVertex3f(-1,+1,+1);
   glTexCoord2f(0,tex_y);glVertex3f(-1,+1,0.5);

   glEnd();
   glDisable(GL_TEXTURE_2D);
   
   
// Inner walls - adding inner walls to add different texture to interior
// add stone wall like texture for interior 
   
  //  glEnable(GL_TEXTURE_2D);
  //  glBindTexture(GL_TEXTURE_2D,interiorWall);
   
   // Inside wall (front) of the cabin along X-axis
   glBegin(GL_QUADS);
   glColor3f(1,1,1);


   // interior back wall along X-axis
   glNormal3f( 0, 0, +1);
   glTexCoord2f(0,0);glVertex3f(+1,y,-0.99);
   glTexCoord2f(tex_x,0);glVertex3f(-1,y,-0.99);
   glTexCoord2f(tex_x,tex_y);glVertex3f(-1,+1,-0.99);
   glTexCoord2f(0,tex_y);glVertex3f(+1,+1,-0.99);

   // Inside right side of the wall (along Z-axis)
   glNormal3f(-1, 0, 0);
   glTexCoord2f(0,0);glVertex3f(+0.99,y,+1);
   glTexCoord2f(tex_x,0);glVertex3f(+0.99,y,-1);
   glTexCoord2f(tex_x,tex_y);glVertex3f(+0.99,+1,-1);
   glTexCoord2f(0,tex_y);glVertex3f(+0.99,+1,+1);


   // Inside wall section without door (along Z-axis)
   glNormal3f(+1, 0, 0);
   glTexCoord2f(0,0);glVertex3f(-0.99,y,-1);
   glTexCoord2f(tex_x,0);glVertex3f(-0.99,y,0.5);
   glTexCoord2f(tex_x,tex_y);glVertex3f(-0.99,+1,0.5);
   glTexCoord2f(0,tex_y);glVertex3f(-0.99,+1,-1);



   // Inside wall section above the door (along Z-axis)
   glNormal3f(+1, 0, 0);
   glTexCoord2f(0,0);glVertex3f(-0.99,0.5,0.5);
   glTexCoord2f(tex_x,0);glVertex3f(-0.99,0.5,+1);
   glTexCoord2f(tex_x,tex_y);glVertex3f(-0.99,+1,+1);
   glTexCoord2f(0,tex_y);glVertex3f(-0.99,+1,0.5);
   


   //L of window                             
   glNormal3f(0,0,-1);
   glTexCoord2f(0,0);glVertex3f(-1,y,+0.99);
   glTexCoord2f(tex_x,0);glVertex3f(-0.6,y,+0.99);
   glTexCoord2f(tex_x,tex_y);glVertex3f(-0.6,+1,+0.99);
   glTexCoord2f(0,tex_y);glVertex3f(-1,+1,+0.99);

   // R of window
   glNormal3f(0,0,-1);
   glTexCoord2f(0,0);glVertex3f(0.6,y,+0.99);
   glTexCoord2f(tex_x,0);glVertex3f(+1,y,+0.99);
   glTexCoord2f(tex_x,tex_y);glVertex3f(+1,+1,+0.99);
   glTexCoord2f(0,tex_y);glVertex3f(0.6,+1,+0.99);

   // Below window section of wall
   glNormal3f(0,0,-1);
   glTexCoord2f(0,0);glVertex3f(-0.6,y,+0.99);
   glTexCoord2f(tex_x,0);glVertex3f(+0.6,y,+0.99);
   glTexCoord2f(tex_x,tex_y);glVertex3f(+0.6,-0.55,+0.99);
   glTexCoord2f(0,tex_y);glVertex3f(-0.6,-0.55,+0.99);

   // above window section of wall
   glNormal3f(0,0,-1);
   glTexCoord2f(0,0);glVertex3f(-0.6,0.5,+0.99);
   glTexCoord2f(tex_x,0);glVertex3f(+0.6,0.5,+0.99);
   glTexCoord2f(tex_x,tex_y);glVertex3f(+0.6,+1,+0.99);
   glTexCoord2f(0,tex_y);glVertex3f(-0.6,+1,+0.99);

   glEnd();
  //  glDisable(GL_TEXTURE_2D);




   // Exterior top of the house
   glBegin(GL_QUADS);
   glColor3f(1.0,0,0);   
   glNormal3f( 0, 1, 0);
   glVertex3f(-1,+1,+1);
   glVertex3f(+1,+1,+1);
   glVertex3f(+1,+1,-1);
   glVertex3f(-1,+1,-1);
   glEnd();

   // Exterior top of the house
   glBegin(GL_QUADS);
   glColor3f(1.0,1.0,1.0);   
   glNormal3f( 0, -1, 0);
   glVertex3f(-1,+0.99,+1);
   glVertex3f(+1,+0.99,+1);
   glVertex3f(+1,+0.99,-1);
   glVertex3f(-1,+0.99,-1);
   




   glEnd();
   
   
   glPopMatrix();
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D,roofTexture);
   drawRoof(x,y+dy,z,dx,dy,dz,th);
   glDisable(GL_TEXTURE_2D);

}



void renderScene(void)
{
    const float base = -0.7;
   
    path (0,-1,0,2,0.01,3,0);
    //house(-1,-0.7,-1,0.3,0.3,0.3,0);
    // tree(0,-0.7,0, 0.2, 0.3);

    drawCabin(-1,base,-1,0.3,0.4,0.5,0);
    //CreateMountain();
   
    
    
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

   
    switch(mode)
    {
        case ORTHOGONAL:
            // glTranslatef(0,0,8.0);
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

    glutInitWindowPosition(100,100);

    /* Request 600 x 600 pixel window */
    glutInitWindowSize(600,600);

    /* Create Window */
    glutCreateWindow("Madhukar's Cabin in the Woods");

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

    glutVisibilityFunc(NULL);

    glutIdleFunc(NULL);

   

    exteriorWall = LoadTexBMP("images/brick.bmp");
    interiorWall = LoadTexBMP("images/housewall.bmp");
    roadTexture = LoadTexBMP("images/mud.bmp");
    roofTexture = LoadTexBMP("images/roof.bmp");
    // woodTexture = LoadTexBMP("images/wood_1.bmp");
    // leafTexture = LoadTexBMP("images/treeleaf.bmp");
    
    /* pass control for GLUT for events */
    glutMainLoop();

    /* return to OS */
    return 0;

}
