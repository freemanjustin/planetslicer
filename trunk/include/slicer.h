#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stddef.h>

// glut include
#ifdef _OS_X_
//#include </System/Library/Frameworks/GLUT.framework/Headers/glut.h>
#include <GLUT/glut.h>
#endif

#ifdef _LINUX_
#include <GL/gl.h>
#include <GL/glut.h>
#endif

// libxml2 includes
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

// libpng include
#include <png.h>

#include "loadTexture.h"

// defines
#define PI_ 3.14159265358979323846
#define TRUE 1
#define FALSE 0

#define RES 200

#define MAX_LAYERS	12

typedef struct {
	float	red;
	float	green;
	float	blue;
}Colors;

typedef struct {
	char		*name;
	float		start;
	float		end;
	char		textureMap[256];
	Colors		colors;
}layers;

// struct definition
typedef struct {
	int	DRAW_ALL;
	GLfloat theta[3];
	GLdouble viewer[3];

	GLint xWinSize;
	GLint yWinSize;

	int startx, starty;
	
	GLuint  texture[MAX_LAYERS]; // current limit is 6 texture maps

	GLint action;
	
	// params red in from file
	int		numLayers;
	int		what_to_draw;
	int		what_to_draw_core;
	layers	L[MAX_LAYERS];
	Colors	bg;
	
}e;

// E is a global
e *E;

// prototypes
void setupOpenGLEnvironment();

void InitGL(GLfloat Width, GLfloat Height);

void Transform(GLfloat Width, GLfloat Height);

void myReshape( int w, int h );

void keys( unsigned char key, int x, int y );

void motion(int x, int y);

void mouse(int button, int state, int x, int y);

void display();

void WindowDump_PPM(void);
void WindowDump_PNG(void);

void draw();

char *lr_pack( char *c );

void getparams_xml(char* parameter_filename, e *E);
void parseInputFile_params (e *E, xmlDocPtr doc, xmlNodePtr cur);
void parseInputFile_layers (e *E, xmlDocPtr doc, xmlNodePtr cur, xmlChar *txt);

void loadTextureMaps(e	*E);

void setDefaults(e *E);

void write_xml(e *E);
