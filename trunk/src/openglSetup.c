#include "slicer.h"

void setupOpenGLEnvironment()
{
	GLfloat  	ambient_light[4];
	GLfloat   	source_light[4];
	GLfloat     light_pos[4];

	// set up initial values for the E struct
	E->DRAW_ALL=78;
	
	E->viewer[0] = 0.0; 
	E->viewer[1] = 0.0; 
	
	ambient_light[0] = 0.5;
	ambient_light[1] = 0.55;
	ambient_light[2] = 0.75;
	ambient_light[3] = 1.0;
	
	source_light[0] = 1.0;
	source_light[1] = 1.0;
	source_light[2] = 1.0;
	source_light[3] = 1.0;
	
	light_pos[0] = -2.0;
	light_pos[1] = 2.0;
	light_pos[2] = -1.0;
	light_pos[3] = 1.0;

	//GLfloat mat_red_diffuse[] = { 0.7, 0.0, 0.1, 1.0 };
	//GLfloat mat_green_diffuse[] = { 0.0, 0.7, 0.1, 1.0 };
	//GLfloat mat_blue_diffuse[] = { 0.0, 0.1, 0.7, 1.0 };
	//GLfloat mat_yellow_diffuse[] = { 0.7, 0.8, 0.1, 1.0 };
	//GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	//GLfloat mat_shininess[] = { 100.0 };

	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_ACCUM | GLUT_DEPTH );
	
	//set initial window size
	glutInitWindowSize(E->xWinSize,E->yWinSize);    
	glutCreateWindow("slicer");
	
	// Initialize the window
	InitGL(E->xWinSize,E->yWinSize);             

	// Enable Smooth Shading
    glShadeModel(GL_SMOOTH);            
    
    // Depth Buffer Setup
	glClearDepth(1.0f);          
	
	// Enables Depth Testing
    glEnable(GL_DEPTH_TEST);           
    
    // The Type Of Depth Testing To Do
    glDepthFunc(GL_LEQUAL);                 
    
    // Perspective Calculations
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);      
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	
	glEnable( GL_LINE_SMOOTH);
	glEnable( GL_POINT_SMOOTH);
	glEnable( GL_POLYGON_SMOOTH);
	
	//lighting model
	glEnable ( GL_LIGHTING );
   	glLightModelfv ( GL_LIGHT_MODEL_AMBIENT, ambient_light );
   	glLightfv (GL_LIGHT0,GL_DIFFUSE, source_light );
   	glLightfv ( GL_LIGHT0,GL_POSITION, light_pos  );
   	glEnable  ( GL_LIGHT0 );
   	
   	// Enable material properties for lighting
   	glEnable ( GL_COLOR_MATERIAL );
   	glColorMaterial ( GL_FRONT, GL_AMBIENT_AND_DIFFUSE );

	//Enable texture mapping
	glEnable(GL_TEXTURE_2D);		
	glTexEnvf ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	glEnable(GL_CULL_FACE); 
	glColor3f(1.0, 1.0, 1.0);
	
	// backgorund color - if not present int he input file
	// defaults to black background
   	//glClearColor (0.0, 0.0, 0.0, 0.0);  // black
   	glClearColor(E->bg.red,E->bg.green, E->bg.blue, 0.0);
   	//glClearColor (1.0, 1.0, 1.0, 0.0);  // white
   	glClearAccum(0.0, 0.0, 0.0, 0.0);
   	
   	// set up the GLUT callback functions
	glutDisplayFunc( display );
	glutMouseFunc( mouse );
	glutMotionFunc(motion);
	glutReshapeFunc( myReshape );
	glutKeyboardFunc( keys );
	
	glEnable( GL_DEPTH_TEST );
	
	loadTextureMaps(E);
}

// A general OpenGL initialization function.
void InitGL(GLfloat Width, GLfloat Height)	
{
  	Transform( Width, Height );                 
}



// window transformation routine 
void Transform(GLfloat Width, GLfloat Height)
{
	// Set the viewport 
	glViewport(0, 0, Width, Height);              
	
	// Select the projection matrix
	glMatrixMode(GL_PROJECTION);             
	
	// Reset The Projection Matrix
	glLoadIdentity();			
	
	// Calculate The Aspect Ratio Of The Window 
	gluPerspective(45.0,Width/Height,0.1,100.0);  
	
	// Switch back to the modelview matrix
	glMatrixMode(GL_MODELVIEW);                   
}



void myReshape( int w, int h )
{	
	glViewport( 0, 0, w, h );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	
	if( w<=h ) {
		glFrustum( -2.0, 2.0, -2.0 * (GLfloat) h / (GLfloat) w,
				2.0 * (GLfloat) h / (GLfloat) w, 2.0, 20.0 );
	}
	else {
		glFrustum( -2.0, 2.0, -2.0 * (GLfloat) w / (GLfloat) h,
				2.0 * (GLfloat) w / (GLfloat) h, 2.0, 20.0 );
	}
	
	// or use glu perspective 
	// gluPerspective( 45.0, w/h, 1.0, 10.0 ); 
	
	glMatrixMode( GL_MODELVIEW );	
	
	// Sanity checks
	if (h==0)	h=1;                   
	if (w==0)	w=1;
	
	// do the transformation
	Transform( w, h );
	E->xWinSize = w;
	E->yWinSize = h;
}



void keys( unsigned char key, int x, int y ) 
{
	
	if( key == '+' ) {
		E->viewer[2] -= 0.1;
	}
	if( key == '-' ) {
		E->viewer[2] += 0.1;
	}
	if( key == '=' ) {
		E->viewer[2] -= 0.1;
	}
	if( key == '_' ) {
		E->viewer[2] += 0.1;
	}
	if( key == 'x' ) {
		E->theta[0] += 2.0;
		if( E->theta[0] > 360.0 ) E->theta[0] -= 360.0;
	}
	if( key == 'X' ) {
		E->theta[0] -= 2.0;
		if( E->theta[0] > 360.0 ) E->theta[0] -= 360.0;
	}
	
	if( key == 'y' ) {
		E->theta[1] += 2.0;
		if( E->theta[1] > 360.0 ) E->theta[1] -= 360.0;
	}
	if( key == 'Y' ) {
		E->theta[1] -= 2.0;
		if( E->theta[1] > 360.0 ) E->theta[1] -= 360.0;
	}
	
	if( key == 'z' ) {
		E->theta[2] += 2.0;
		if( E->theta[2] > 360.0 ) E->theta[2] -= 360.0;
	}
	if( key == 'Z' ) {
		E->theta[2] -= 2.0;
		if( E->theta[2] > 360.0 ) E->theta[2] -= 360.0;
	}
	if( key == 'r'){   // reset to the original view coords
	   E->theta[0]=125.0;
	   E->theta[1]=0.0;
	   E->theta[2]=225.0;
	   
	   E->viewer[2] = 3.0;
	   
	   //E->angle = 0;   	// in degrees
       //E->angle2 = 0;	// in degrees 
      
	}
	if( key == 's' || key == 'S') {
		//WindowDump_PPM();
		WindowDump_PNG();
	}
	if( key == 'w' || key == 'W') {
		//WindowDump_PPM();
		write_xml(E);
	}
	
	if( key == '1' ) {
		E->what_to_draw = 1;
	}
	if( key == '7' ) {
		if(E->numLayers>1){
			E->what_to_draw = 78;
			if(E->what_to_draw_core != 1){
				E->what_to_draw_core = 78;
			}
		}
	}
	if( key == '2' ) {
		if(E->numLayers>1){
			E->what_to_draw = 12;
			if(E->what_to_draw_core != 1){
				E->what_to_draw_core = 12;
			}
		}
	}
	if( key == '3' ) {
		if(E->numLayers>1){
			E->what_to_draw = 34;
			if(E->what_to_draw_core != 1){
				E->what_to_draw_core = 34;
			}
		}
	}
	if( key == '4' ) {
		if(E->numLayers>1){
			E->what_to_draw = 14;
			if(E->what_to_draw_core != 1){
				E->what_to_draw_core = 14;
			}
		}
	}
	if( key == 'i' || key == 'I' || key == 'h' || key == 'H' ){
	   fprintf(stdout,"keyboard navigation:\n");
	   fprintf(stdout,"s or S : save an image\n");
	   fprintf(stdout,"w or W : save xml\n");
	   fprintf(stdout,"x or X : rotate x viewer axis\n");
	   fprintf(stdout,"y or Y : rotate y viewer axis\n");
	   fprintf(stdout,"z or Z : rotate z viewer axis\n");
	   fprintf(stdout,"+ : zoom in viewer\n");
	   fprintf(stdout,"- : zoom out viewer\n");
	   fprintf(stdout,"r : reset to default view\n");
	   fprintf(stdout,"1 : draw whole sphere\n");
	   fprintf(stdout,"2 : draw half sphere\n");
	   fprintf(stdout,"3 : draw 3/4 sphere\n");
	   fprintf(stdout,"4 : draw quarter sphere\n");
	   fprintf(stdout,"7 : draw 7/8ths sphere\n");
	   fprintf(stdout,"q or esc : quit\n");
	}
	
	if( key == 'q' ) exit(0);
	
	if(key == 27){ // ESC key quits
	     glutDestroyWindow(glutGetWindow()); 
	     exit(0);
	}
	
	display();
	
}

void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			E->action = 1;
			E->startx = x;
			E->starty = y;
   	 	}
  	}
  	else if(button == GLUT_MIDDLE_BUTTON){
		if (state == GLUT_DOWN) {
			E->action = 2;
			E->startx = x;
			E->starty = y;
  		}
	}
}

void motion(int x, int y)
{
	switch(E->action){
		case 1:
			E->theta[1] = E->theta[1] - (x - E->startx);
    		E->theta[0] = E->theta[0] + (y - E->starty);
    		break;
    	case 2:
    		E->viewer[2] = E->viewer[2] + (x - E->startx);
    		break;
	}
    E->startx = x;
    E->starty = y;
    glutPostRedisplay();
}

