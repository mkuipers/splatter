/**************************************************************
* File:   splatterer.c
* Author: M. Kuipers and T. Miller
* Date  : 05/06/2009
**************************************************************
* Splatter painter
**************************************************************/
// Makes it work on OSX 10.5
#ifdef __APPLE__
  #include <GLUT/glut.h>
#else
  #include <GL/glut.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>

// Structures ------------------------------------------------
typedef struct {
  int x,y;
} point_t;

typedef struct {
  int x,y;
} vect_t;

typedef struct {
  point_t p;
  double r;
} circle_t;

typedef struct {
  double r, t0, t1;
} blot_t;

typedef struct {
  float r, g, b;
} color_t;

typedef struct {
  double r, theta;
} rpoint_t;


// Colors ----------------------------------------------------
color_t red = {1.0,0.0,0.0};
color_t orange = {1.0,.647,0.0};
color_t yellow = {1.0,1.0,0.0};
color_t green = {0.498,1.0,0.0};
color_t blue = {0.0,0.0,1.0};
color_t indigo = {0.0,0.749,1.0};
color_t violet = {0.5,0.0,0.5};
color_t white = {1.0,1.0,1.0};
color_t black = {0.0,0.0,0.0};

// Function prototypes ---------------------------------------

// User interaction functions
void backupPixels();
void restorePixels();
void writeImage();
void thicker();
void thinner();
void moreRight();
void moreLeft();
void increaseNumBigHairs();
void decreaseNumBigHairs();
void increaseNumMediumHairs();
void decreaseNumMediumHairs();
void increaseNumSmallHairs();
void decreaseNumSmallHairs();

// Colors
color_t randomColor();
void switchBG();
double dist(point_t a, point_t b);
double point_on_parabola(double x, double factor, 
                         double offset, double displacement);
// Polar coordinates
rpoint_t toRPoint(point_t p);
point_t toCPoint(rpoint_t p);

// Random numbers
void initialize_rand();
float random_float (float maximum);	
float ranf();
int rani(int maximum);
point_t random_point();
double box_muller(double m, double s);

// OpenGL functions and callbacks
void mouseclick(int button, int state, int x, int y);
void mousemotion(int x, int y);
void keyboard(unsigned char key, int x, int y);
void display();
void init();
void cleanup();

// Shape rasterization
void splatter(point_t a, point_t b, blot_t blot, int num, 
              double factor, double offset, double disp);
void hairy_splatter(point_t a, point_t b, blot_t blot, int num, 
                    double factor, double offset, double disp);
void make_wallpaper(int num);
void fillHorizLine(point_t a, point_t b);
blot_t makeBlot(double r, double t0, double t1); 
void rasterizeCircle(circle_t c);

// Other utility functions
bool insideCircle(int x, int y, circle_t c);
void clearScreen();

// Declare global variables (ick)- ---------------------------
char * magic = "P6";
const int winWid = 1920; //window size, measured in pixels
const int winHgt = 1920;
const double worldW = 100.0; //view region extent, in world coordinates
const double worldH = 100.0;
int nPts = 0;
const int MAXPTS = 2;
point_t vertices[MAXPTS];
point_t mousePos;
float pixels[winWid][winHgt][3];
float oldPixels[winWid][winHgt][3];
color_t bg = white;
color_t fg = black;
int min_radius = 50;
int numSplatters = 0;
double global_factor = 2;
double global_offset = .5;
double global_disp = 0;
int numImages = 0;
int max_images = 100;

// Hairy stuff
bool hairy = false;
int num_big_hairs = 2;
int num_medium_hairs = 3;
int num_small_hairs = 7;

// Utility functions -----------------------------------------

// The following backs up the currently displayed pixels so that we can undo
void backupPixels() {
  int i,j,k;
  for(i = winHgt -1; i >= 0; i--) {
    for (j = 0; j < winWid; j++) {
      for(k = 0; k < 3; k++)
        oldPixels[i][j][k] = pixels[i][j][k];
    }
  }
  //printf("backed up pixels.\n");
}

// Undo
void restorePixels() {
  int i,j,k;
  for(i = winHgt -1; i >= 0; i--) {
    for (j = 0; j < winWid; j++) {
      for(k = 0; k < 3; k++)
        pixels[i][j][k] = oldPixels[i][j][k];
    }
  }
  //printf("restored pixels.\n");
  glutPostRedisplay();
}

// writeImage will write out to a file starting with image00.ppm and increment
// the count each time the image is written out 
void writeImage() {
  char outfilename[12] = "image00.ppm";
  int firstDigit = numImages / 10;
  int secondDigit = numImages - (firstDigit * 10);
  outfilename[5] = firstDigit+48;
  outfilename[6] = secondDigit+48;
  printf("Writing image %s\n",outfilename);
  FILE* outfile = fopen(outfilename,"wb");
  int writecheck = fprintf(outfile,"%s\n%d %d\n255\n",magic,winWid,winHgt);
  if(!writecheck) {
    perror("File write error.\n");
    exit(1);
  }
  numImages++;
  int i,j,k;
  for(i = winHgt -1; i >= 0; i--) {
    for (j = 0; j < winWid; j++) {
      for(k = 0; k < 3; k++)
        fputc((int)(255 * pixels[i][j][k]),outfile);
    }
  }
  fclose(outfile);
  outfile = NULL;
}

// User procedures to modify parameters to point_on_parabola
void thicker() {
  global_offset += .1;
}

void thinner() {
  global_offset -= .1;
}

void moreRight() {
  global_disp += .1;
}

void moreLeft() {
  global_disp -= .1;
}

// The following are for editing the hairy splatters, changing the number of
// "hairs"
void increaseNumBigHairs(){
  if (num_big_hairs < 100){
    num_big_hairs ++;
  }
}

void decreaseNumBigHairs(){
  if (num_big_hairs > 0){
    num_big_hairs --;
  }
}

void increaseNumMediumHairs(){
  if (num_medium_hairs < 100){
    num_medium_hairs ++;
  }
}

void decreaseNumMediumHairs(){
  if (num_medium_hairs > 0){
    num_medium_hairs --;
  }
}

void increaseNumSmallHairs(){
  if (num_small_hairs < 100){
    num_small_hairs ++;
  }
}

void decreaseNumSmallHairs(){
  if (num_small_hairs > 0){
    num_small_hairs --;
  }
}

color_t randomColor() {
  color_t a = {drand48(),drand48(),drand48()};
  return a;
}

void switchBG() {
  bg = fg;
  clearScreen();
}

double dist(point_t a, point_t b) {
  return sqrt(pow(fabs((double) a.x - (double) b.x), 2) + 
              pow(fabs((double) a.y - (double) b.y), 2));
}

/* Take an interpolation value and return a radius scale factor. */
double point_on_parabola(double x, double factor, 
                         double offset, double displacement) {
  return (factor * ((x + displacement) * 
                    (x + displacement))) + offset;
}

/* Take a cartesian point and return a polar coordinate */
rpoint_t toRPoint(point_t p){
  point_t origin = {0,0};
  rpoint_t rp;
  rp.r = dist(origin, p);
  if (p.x == 0 && p.y == 0){
    rp.theta = 0;
  } else if (p.x >= 0){
    rp.theta = asin((double) p.y / (double) rp.r);
  } else if (p.x < 0){
    rp.theta = asin((double) p.y / (double) rp.r);
  } else {
    perror("Wtf\n");
  }
  return rp;
}

/* Take a polar coordinate and return a cartesian point */
point_t toCPoint(rpoint_t rp){
  point_t p;
  p.x = (int) round(rp.r * cos(rp.theta));
  p.y = (int) round(rp.r * sin(rp.theta));
  return p;

}

/* seeds the random number generator */
void initialize_rand(){
	srand(((unsigned)(time(0) * 10000)));
}

/* Returns a random float between 0 and maximum. Maximum included. 
 * (0, maximum] */ 
float random_float (float maximum){	
	float i = rand();
	float j = (float) RAND_MAX;
	return ((i/j) * maximum);
}

/* Returns a random float (0, 1] */
float ranf(){
	return random_float(1);
}

int rani(int maximum) {
	long a = rand();
	long b = RAND_MAX;
	return (((double)a / (double)b) * maximum);
}

point_t random_point() {
	point_t a;
	a.x = rani(winWid);
	a.y = rani(winHgt);
	return a;
}

/*The box_muller function is used for modeling normal distributions */
double box_muller(double m, double s)	/* normal random variate generator */
{				        /* mean m, standard deviation s */
	double x1, x2, w, y1;
	static double y2;
	static int use_last = 0;

	if (use_last)		        /* use value from previous call */
	{
		y1 = y2;
		use_last = 0;
	}
	else
	{
		do {
			x1 = 2.0 * ranf() - 1.0;
			x2 = 2.0 * ranf() - 1.0;
			w = x1 * x1 + x2 * x2;
		} while ( w >= 1.0 );

		w = sqrt( (-2.0 * log( w ) ) / w );
		y1 = x1 * w;
		y2 = x2 * w;
		use_last = 1;
	}

	return( m + y1 * s );
}

/*box_muller was originally written by Everett F. Carter Jr. and I display
 * the following information as per request*/
/* boxmuller.c           Implements the Polar form of the Box-Muller
                         Transformation

                      (c) Copyright 1994, Everett F. Carter Jr.
                          Permission is granted by the author to use
			  this software for any application provided this
			  copyright notice is preserved.

*/

// Main ------------------------------------------------------
int main(int argc, char* argv[]) {
  //initialize window
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowSize(winWid, winHgt);
  glutInitWindowPosition(100,150);
  glutCreateWindow("Basic 2D Example Code");

  //register callback functions for glut (other events exist too, if needed...)
  glutDisplayFunc(display);
  glutMouseFunc(mouseclick);
  glutPassiveMotionFunc(mousemotion);
  glutKeyboardFunc(keyboard);

  //register a callback to be executed on exit (this is ANSI C, not OpenGL)
  atexit(cleanup);

  init();  //OpenGL initializations (2D)
	make_wallpaper(100);
//  glutMainLoop(); //infinite event-driven loop (managed by glut)
  return 0;
}

//--------------------------
// basic OpenGL initializations
//--------------------------
void init() {
  initialize_rand();
  glPointSize(3.0); //specify size of rendered points (in pixels)
  glClearColor(0.9, 0.9, 0.9, 1.0); //light gray background (RGBA)
  glViewport(0, 0, winWid, winHgt); //set size of viewport (in pixels)

  //specify extent of view area (In 2D we specify view area in world
  // coordinates since we use a default camera -- at the origin.)
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0, worldW, 0.0, worldH);

  //clear modelview matrix and set it to be the current matrix (so it is
  // current when the display routines execute).
  glMatrixMode(GL_MODELVIEW);
  clearScreen();
  glLoadIdentity();
}

//--------------------------
// called when program exits
//--------------------------
void cleanup() {
  //Put any code that should execute when the program exits here. 
  //For example, you could free dynamically allocated memory here.
}

//--------------------------
// function: display
//--------------------------
void display(void) {
  if (nPts == 2) {
    nPts = 0;
  }
  
  glDrawPixels(winWid, winHgt, GL_RGB, GL_FLOAT, pixels);

  if (nPts >= 1) {
    GLdouble x1, y1, x2, y2;
    x1 = ((double)vertices[0].x / winWid)*worldW;
    y1 = ((double)vertices[0].y / winHgt)*worldH;
    x2 = ((double)mousePos.x / winWid)*worldW;
    y2 = ((double)(winHgt - mousePos.y) / winHgt)*worldH;
    glColor3f(1.0,1.0,0.0);
    glBegin(GL_LINE);
    {
      glVertex2d(x1,y1);
      glVertex2d(x2,y2);
    }
    glEnd();
  }
  glutSwapBuffers();

  //detect rendering errors (note that the call to glGetError() clears
  // the error condition too, so call it once and save the error code).
  int error = glGetError();
  if (error != GL_NO_ERROR)
    printf("OpenGL Error: %s\n", gluErrorString(error));
}

//--------------------------
// function: mouseclick
// called on mouse click events
//--------------------------
void mouseclick(int button, int state, int x, int y) {
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
    y = winHgt - y;
    //assumes world and viewport origins at (0,0)
    point_t click;
    click.x = x;
    click.y = y;

    //add point to array of points
    if (nPts < MAXPTS) {
      vertices[nPts] = click;
      nPts++;
    }

    if (2 == nPts) {
      blot_t bl = makeBlot(min_radius,-.75,.5);
      ( hairy ? hairy_splatter(vertices[0],vertices[1],bl,400, 
                               global_factor, global_offset, global_disp):
                splatter(vertices[0],vertices[1],bl,400, 
                         global_factor, global_offset, global_disp));



    }

  }//if left mouse down event...


  //mark display as needing a refresh (glut will call display callback)
  glutPostRedisplay(); 
}

void mousemotion(int x, int y) {
  point_t pos;
  pos.x = x;
  pos.y = y;
  mousePos = pos;
  glutPostRedisplay();
}

//--------------------------
// function: keyboard
// called on ascii keyboard events
//--------------------------
void keyboard(unsigned char key, int mouseX, int mouseY) {
  if (key == '$' && numImages < max_images)
    writeImage();
  if (key == '+' || key == '=')
    thicker();
  if (key == '-' || key == '_')
    thinner();
  if (key == '<' || key == ',')
    moreLeft();
  if (key == '>' || key == '.')
    moreRight();
  if (key == '!') {
    global_factor = 2;
    global_offset = 0.5;
    global_disp = 0;
    
    num_big_hairs = 2;
    num_medium_hairs = 3;
    num_small_hairs = 7;

  }
  if (key == 'Z' || key == 'z')
    restorePixels(); //undo
  if (key == 'Q' || key == 'q')
    exit(0);  //exit application
  if (key == 'C' || key == 'c')
    clearScreen();
  if (key == '`' || key == '~')
    fg = randomColor();
  if (key == '1')
    fg = red;
  if (key == '2')
    fg = orange;
  if (key == '3')
    fg = yellow;
  if (key == '4')
    fg = green;
  if (key == '5')
    fg = blue;
  if (key == '6')
    fg = indigo;
  if (key == '7')
    fg = violet;
  if (key == '8')
    fg = white;
  if (key == '9')
    fg = black;
  if (key == 'h' || key == 'H')
    hairy = !hairy;
  if (key == 'a')
    decreaseNumBigHairs();
  if (key == 'A')
    increaseNumBigHairs();
  if (key == 's')
    decreaseNumMediumHairs();
  if (key == 'S')
    increaseNumMediumHairs();
  if (key == 'd')
    decreaseNumSmallHairs();
  if (key == 'D')
    increaseNumSmallHairs();
  if (key == 'l' || key == 'L')
    switchBG();

}

/*  This function draws our simplest splatter shape.
 *  It uses only one circle that it translates and resizes and rasterizes
 *  until the distance between the two is within a certain threshold.
 */
void splatter(point_t a, point_t b, blot_t blot, int num, 
              double factor, double offset, double disp) {
  if (!hairy) {
    backupPixels();
  }
  numSplatters++;
  double len = dist(a,b);
  double deltaR = (blot.t1 - blot.t0) / (double)num;
  double deltaLx = (double)(b.x - a.x) / (double)num;
  double deltaLy = (double)(b.y - a.y) / (double)num;
  int i;
  circle_t c;
  for (i = 0; i < num; i++) {
    c.p.x = (int)(i * deltaLx) + a.x;
    c.p.y = (int)(i * deltaLy) + a.y;
    c.r = point_on_parabola(blot.t0 + (i * deltaR), factor, offset, disp)
          * blot.r;
    rasterizeCircle(c);
  }
}


/* This function will make a a more hairy-looking splatter
 */
void hairy_splatter(point_t a, point_t b, blot_t blot, int num, 
              double factor, double offset, double disp) {
  if (hairy) {
    backupPixels();
  }
  splatter(a, b, blot, num, factor, offset/(box_muller(2,1)), disp);
  // establish polar coordinates around the first point
  int i;
  //First make the bigger hairs
  for (i = 0; i < box_muller(num_big_hairs, 1); i++){
    point_t relativeP = {b.x - a.x, b.y - a.y};
    rpoint_t rp = toRPoint(relativeP);
    rp.r *= box_muller((3.0 / 4.0), .1);
    rp.theta = box_muller(rp.theta, 1);
    relativeP = toCPoint(rp);
    point_t newB = {relativeP.x + a.x, relativeP.y + a.y};
    double newOffset = offset/(box_muller(8, 3));
    splatter(a, newB, blot, num, factor, newOffset, disp);
  }

  // Then make the medium hairs
  for (i = 0; i < box_muller(num_medium_hairs, 1); i++){
    point_t relativeP = {b.x - a.x, b.y - a.y};
    rpoint_t rp = toRPoint(relativeP);
    rp.r *= box_muller((2.0 / 4.0), .1);
    rp.theta = box_muller(rp.theta, 1);
    relativeP = toCPoint(rp);
    point_t newB = {relativeP.x + a.x, relativeP.y + a.y};
    double newOffset = offset/(box_muller(10, 4));
    splatter(a, newB, blot, num, factor/2, newOffset, disp);
  }

  // Then make the smaller hairs
  for (i = 0; i < box_muller(num_small_hairs, 1); i++){
	  point_t relativeP = {b.x - a.x, b.y - a.y};
	  rpoint_t rp = toRPoint(relativeP);
	  rp.r *= box_muller((1.0 / 4.0), .1);
	  rp.theta = box_muller(rp.theta, 1);
	  relativeP = toCPoint(rp);
	  point_t newB = {relativeP.x + a.x, relativeP.y + a.y};
	  double newOffset = offset/(box_muller(12, 5));
	  splatter(a, newB, blot, num, factor/4, newOffset, disp);
	}	
}

void make_wallpaper(int num) {
	int i, j, blots;
	point_t a, b;
	for (i = 0; i < num; i++) { // Make multiple wallpapers
		blots = rani(10) + 3;
		for (j = 0; j < blots; j++) { // Make multiple splatters in each wallpaper
			fg = randomColor();
			blot_t blot = makeBlot(min_radius,-.75,.5);
			a = random_point();
			b = random_point();
			hairy_splatter(a,b,blot,400,
										 (drand48() + global_factor), 
										 (drand48() + global_offset), 
										 (drand48() + global_disp));
			printf("Made a splatter from %d %d to %d %d\n",a.x,a.y,b.x,b.y);
		}
		writeImage();
		clearScreen();
	}
}

/* Make our blot_t struct. We use this in our splatter function. */
blot_t makeBlot(double r, double t0, double t1){
  blot_t a;
  a.r = r;
  a.t0 = t0;
  a.t1 = t1;
  return a;
}

/* Rasterize a circle.
 * Instead of iterating through all pixels in an image, we iterate through
 * the 2r x 2r box around the circle. Particularly, we iterate vertically
 * and in each iteration we iterate to the right until we detect that the
 * current pixel is inside the circle. Then we call fillHorizLine, which
 * rasterizes a line twice as long as the distance between our current point
 * and the middle vertical line of our circle.
 */
void rasterizeCircle(circle_t c){
  int height, alpha, disp;
  for (height = (c.p.y - (int)c.r); height < (c.p.y + (int)c.r); height++) {
    // after this loop, alpha is where we need to start filling.
    for (alpha = (c.p.x - (int)c.r); alpha < c.p.x && 
         !insideCircle(alpha,height,c); alpha++);
    disp = c.p.x - alpha;
    point_t start = {alpha, height};
    point_t end = {alpha + (2 * disp), height};
    fillHorizLine(start,end);
    glutPostRedisplay();
  }
}

/* Rasterize the line (a.x, a.y) to (b.x, a.y). Notice that this only rasterizes
 * horizontal lines!
 * Used by rasterizeCircle.
 */
void fillHorizLine(point_t a, point_t b) {
  int i;
  for (i = a.x; i < b.x; i++) {
    if (!(i < 0) && !(a.y < 0) && !(i >= winWid) && !(a.y >= winHgt)) {
      //printf("Filling %d,%d\n",i, a.y);
      pixels[a.y][i][0] = fg.r;
      pixels[a.y][i][1] = fg.g;
      pixels[a.y][i][2] = fg.b;
    }
  }
}

/* Returns true if the point (x, y) is inside circle c. */
bool insideCircle(int x, int y, circle_t c) {
  point_t a = {x,y};
  double len = dist(a, c.p);
  return len < c.r;
}

/* Fill our pixel array with color_t bg. */
void clearScreen() {
  int i, j;
  for(i = 0; i < winWid; i++) {
    for(j = 0; j < winHgt; j++) {
      pixels[j][i][0] = bg.r;
      pixels[j][i][1] = bg.g;
      pixels[j][i][2] = bg.b;
    }
  }
  glutPostRedisplay();
}
