#include "slicer.h"
#include "jitter.h"

void display()
{
	GLint viewport[4];
    int jitter;
    int ACSIZE = 2;

	glGetIntegerv (GL_VIEWPORT, viewport);
   
	glLoadIdentity();
	gluLookAt( E->viewer[0], E->viewer[1], E->viewer[2], 0.0, 0.0, 0.0, 
				0.0, 1.0, 0.0 );
	
	// Perform scene rotations based on user input
	glRotatef( E->theta[0], 1.0, 0.0, 0.0 );
	glRotatef( E->theta[1], 0.0, 1.0, 0.0 );
	glRotatef( E->theta[2], 0.0, 0.0, 1.0 );
	
   	// anti-alias
	glClear(GL_ACCUM_BUFFER_BIT);
	for (jitter = 0; jitter < ACSIZE; jitter++) {
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glPushMatrix ();
		glTranslatef (j8[jitter].x*1.5/viewport[2],
                    j8[jitter].y*1.5/viewport[3], 0.0);
      	draw();
      	glPopMatrix ();
      	glAccum(GL_ACCUM, 1.0/ACSIZE);
	}
	glAccum (GL_RETURN, 1.0);
	glFlush();
	glutSwapBuffers();
}


// the main drawing code
//
// this fucntion is a total mess!

void draw()
{
	int i ;	
    
	// the clipping plane equations
    GLdouble eqn1[4]  = {0.0, 0.0, -1.0, 0.0};
	GLdouble eqn2[4] = {0.0, 1.0, 0.0, 0.0};
	GLdouble eqn3[4] = {-1.0, 0.0, 0.0, 0.0};	
	
	GLdouble eqn4[4]  = {0.0, 0.0, 1.0, 0.0};
	GLdouble eqn5[4] = {0.0, -1.0, 0.0, 0.0};
	GLdouble eqn6[4] = {1.0, 0.0, 0.0, 0.0};
    
    GLUquadricObj*  s = gluNewQuadric( );
    GLUquadricObj*  t = gluNewQuadric( );
    GLUquadricObj*  u = gluNewQuadric( );
    GLUquadricObj*  v = gluNewQuadric( );
    GLUquadricObj*  w = gluNewQuadric( );
    GLUquadricObj*  a = gluNewQuadric( );
    GLUquadricObj*  b = gluNewQuadric( );
    GLUquadricObj*  c = gluNewQuadric( );
    
    gluQuadricDrawStyle ( s, GLU_FILL );
   	gluQuadricNormals ( s, GLU_SMOOTH );
   	gluQuadricTexture ( s, GL_TRUE    );
   	
   	gluQuadricDrawStyle ( t, GLU_FILL );
   	gluQuadricNormals ( t, GLU_SMOOTH );
   	gluQuadricTexture ( t, GL_TRUE    );
   	
   	gluQuadricDrawStyle ( u, GLU_FILL );
   	gluQuadricNormals ( u, GLU_SMOOTH );
   	gluQuadricTexture ( u, GL_TRUE    );
   	
   	gluQuadricDrawStyle ( v, GLU_FILL );
   	gluQuadricNormals ( v, GLU_SMOOTH );
   	gluQuadricTexture ( v, GL_TRUE    );
   	
   	
   	gluQuadricDrawStyle ( w, GLU_FILL );
   	gluQuadricNormals ( w, GLU_SMOOTH );
   	gluQuadricTexture ( w, GL_TRUE    );
   	
   	gluQuadricDrawStyle ( a, GLU_FILL );
   	gluQuadricNormals ( a, GLU_SMOOTH );
   	gluQuadricTexture ( a, GL_TRUE    );
   	
   	gluQuadricDrawStyle ( b, GLU_FILL );
   	gluQuadricNormals ( b, GLU_SMOOTH );
   	gluQuadricTexture ( b, GL_TRUE    );
   	
   	
   	gluQuadricDrawStyle ( c, GLU_FILL );
   	gluQuadricNormals ( c, GLU_SMOOTH );
   	gluQuadricTexture ( c, GL_TRUE    );
   	
	  
    glPushMatrix();
    
    // need this rotation so that the texture maps show up correctly. duh.
    glRotatef ( 180.0, 0.0, 90.0, 0.0 );
    
    
	// draw the surface of the planet
	if(E->what_to_draw == 1){
		// last texture map should always be the surface texture
		glBindTexture(GL_TEXTURE_2D, E->texture[E->numLayers-1]);
		gluSphere ( s, 1.0, RES, RES );
	}
	else{
		// draw the whole planet with the surface and cutaway bits
		
		// for 7/8ths of the planet
		if(E->what_to_draw == 78){
			glPushMatrix();
			glClipPlane (GL_CLIP_PLANE0, eqn1);
			glEnable (GL_CLIP_PLANE0);		
				glBindTexture(GL_TEXTURE_2D, E->texture[E->numLayers-1]);
				gluSphere ( s, 1.0, RES, RES );
			glDisable(GL_CLIP_PLANE0);
			glPopMatrix();
			 
			glPushMatrix();
			glClipPlane (GL_CLIP_PLANE1, eqn2);
			glEnable (GL_CLIP_PLANE1);
				glBindTexture(GL_TEXTURE_2D, E->texture[E->numLayers-1]);
				gluSphere ( s, 1.0, RES, RES );
			glDisable(GL_CLIP_PLANE1);
			glPopMatrix();
			 
			glPushMatrix();
			glClipPlane (GL_CLIP_PLANE2, eqn3);
			glEnable (GL_CLIP_PLANE2);
				glBindTexture(GL_TEXTURE_2D, E->texture[E->numLayers-1]);
				gluSphere ( s, 1.0, RES, RES );
			glDisable(GL_CLIP_PLANE2);
			glPopMatrix();
		 
			// fill in the sides of the 1/8 that has been removed from the sphere
			glDisable(GL_CULL_FACE);
			for(i= 1;i<E->numLayers-1;i++){
				//printf("i = %d\n",i);
				glPushMatrix();
				// set the texture map 
				glBindTexture(GL_TEXTURE_2D, E->texture[i]);
				// set the layer color
				glColor3f(E->L[i].colors.red,E->L[i].colors.green,E->L[i].colors.blue);
				glRotatef ( 0.0, 90.0, 0.0, 0.0 );
				// fill in a side
				gluDisk(t,E->L[i].start,E->L[i].end,RES,RES);
				//gluPartialDisk(t,E->L[i].start,E->L[i].end,RES,RES, -90, -90);
				glRotatef ( 90.0, 90.0, 0.0, 0.0 );
				gluDisk(u,E->L[i].start,E->L[i].end,RES,RES);
				//gluPartialDisk(u,E->L[i].start,E->L[i].end,RES,RES, -90, -90);
				glRotatef ( 90.0, 0.0, 90.0, 0.0 );
				gluDisk(v,E->L[i].start,E->L[i].end,RES,RES);
				//gluPartialDisk(v,E->L[i].start,E->L[i].end,RES,RES, -90, -90);
				glPopMatrix();
			}
			glEnable(GL_CULL_FACE);
		}
		else if(E->what_to_draw == 34){
			glPushMatrix();
			glClipPlane (GL_CLIP_PLANE0, eqn1);
			glEnable (GL_CLIP_PLANE0);		
				glBindTexture(GL_TEXTURE_2D, E->texture[E->numLayers-1]);
				gluSphere ( s, 1.0, RES, RES );
			glDisable(GL_CLIP_PLANE0);
			glPopMatrix();
			 
			 /*
			 glPushMatrix();
			 glClipPlane (GL_CLIP_PLANE1, eqn2);
			 glEnable (GL_CLIP_PLANE1);
				glBindTexture(GL_TEXTURE_2D, E->texture[E->numLayers-1]);
				gluSphere ( s, 1.0, RES, RES );
			 glDisable(GL_CLIP_PLANE1);
			 glPopMatrix();
			 */
			 
			 glPushMatrix();
			 glClipPlane (GL_CLIP_PLANE2, eqn3);
			 glEnable (GL_CLIP_PLANE2);
				glBindTexture(GL_TEXTURE_2D, E->texture[E->numLayers-1]);
				gluSphere ( s, 1.0, RES, RES );
			 glDisable(GL_CLIP_PLANE2);
			 glPopMatrix();
		 
		 
			 // fill in the sides of the 1/8 that has been removed from the sphere
			 glDisable(GL_CULL_FACE);
			 for(i= 1;i<E->numLayers-1;i++){
				//printf("i = %d\n",i);
				glPushMatrix();
				// set the texture map 
				glBindTexture(GL_TEXTURE_2D, E->texture[i]);
				// set the layer color
				glColor3f(E->L[i].colors.red,E->L[i].colors.green,E->L[i].colors.blue);
				
				// fill in a side
				glRotatef ( 0.0, 90.0, 0.0, 0.0 );
				gluDisk(t,E->L[i].start,E->L[i].end,RES,RES);
				//gluPartialDisk(t,E->L[i].start,E->L[i].end,RES,RES, 0, -180);
					// don't do these ones
					//glRotatef ( 90.0, 90.0, 0.0, 0.0 );
					//gluDisk(u,E->L[i].start,E->L[i].end,RES,RES);
					//gluPartialDisk(u,E->L[i].start,E->L[i].end,RES,RES, -90, -90);
				
				glRotatef ( 90.0, 0.0, 90.0, 0.0 );
				gluDisk(v,E->L[i].start,E->L[i].end,RES,RES);
				//gluPartialDisk(t,E->L[i].start,E->L[i].end,RES,RES, 0, 180);
				glPopMatrix();
			 }
			 glEnable(GL_CULL_FACE);
		 }
		// for 1/2 of the planet
		else if(E->what_to_draw == 12){
			 glPushMatrix();
			 glClipPlane (GL_CLIP_PLANE1, eqn2);
			 glEnable (GL_CLIP_PLANE1);
				glBindTexture(GL_TEXTURE_2D, E->texture[E->numLayers-1]);
				gluSphere ( s, 1.0, RES, RES );
			 glDisable(GL_CLIP_PLANE1);
			 glPopMatrix();

			 // fill in the sides of the 1/2 that has been removed from the sphere
			 glDisable(GL_CULL_FACE);
			 for(i= 1;i<E->numLayers-1;i++){
				//printf("i = %d\n",i);
				glPushMatrix();
				// set the texture map 
				glBindTexture(GL_TEXTURE_2D, E->texture[i]);
				// set the layer color
				glColor3f(E->L[i].colors.red,E->L[i].colors.green,E->L[i].colors.blue);
				glRotatef ( 90.0, 90.0, 0.0, 0.0 );
				gluDisk(u,E->L[i].start,E->L[i].end,RES,RES);
				glPopMatrix();
			 }
			 glEnable(GL_CULL_FACE);
		 } 
		 // for 1/4 of the planet
		else if(E->what_to_draw == 14){
			 glPushMatrix();
			 glClipPlane (GL_CLIP_PLANE0, eqn4);
			 glClipPlane (GL_CLIP_PLANE1, eqn5);
			 glClipPlane (GL_CLIP_PLANE2, eqn6);
			 glEnable (GL_CLIP_PLANE0);
			 glEnable (GL_CLIP_PLANE1);
			 glEnable (GL_CLIP_PLANE2);
			 
				glBindTexture(GL_TEXTURE_2D, E->texture[E->numLayers-1]);
				gluSphere ( s, 1.0, RES, RES ); 
				 
			 glDisable(GL_CLIP_PLANE0);
			 glDisable(GL_CLIP_PLANE1);
			 glDisable(GL_CLIP_PLANE2);
			 glPopMatrix();
		 
		 
			 // fill in the sides of the 1/8 that has been removed from the sphere
			 glDisable(GL_CULL_FACE);
			 for(i= 1;i<E->numLayers-1;i++){
				//printf("i = %d\n",i);
				glPushMatrix();
				// set the texture map 
				glBindTexture(GL_TEXTURE_2D, E->texture[i]);
				// set the layer color
				glColor3f(E->L[i].colors.red,E->L[i].colors.green,E->L[i].colors.blue);
				
				glRotatef ( 0.0, 90.0, 0.0, 0.0 );
				// fill in a side
					//gluDisk(t,E->L[i].start,E->L[i].end,RES,RES);
				gluPartialDisk(t,E->L[i].start,E->L[i].end,RES,RES, 90, 90);
				
				glRotatef ( 90.0, -90.0, 0.0, 0.0 );
					//gluDisk(u,E->L[i].start,E->L[i].end,RES,RES);
				gluPartialDisk(u,E->L[i].start,E->L[i].end,RES,RES, 90, 90);
				
				glRotatef ( -90.0, 0.0, -90.0, 0.0 );
					//gluDisk(v,E->L[i].start,E->L[i].end,RES,RES);
				gluPartialDisk(v,E->L[i].start,E->L[i].end,RES,RES, 90, 90);
				
				glPopMatrix();
			 }
			 glEnable(GL_CULL_FACE);
		 }
		 
		 
		 // draw the inner most layer - usually the core
		 if(E->what_to_draw_core == 1){
			 // draw the core as a smaller sphere
			 glBindTexture(GL_TEXTURE_2D, E->texture[0]);
			 glPushMatrix();
			 glColor3f(E->L[0].colors.red,E->L[0].colors.green,E->L[0].colors.blue);
			 gluSphere ( w, E->L[0].end, RES, RES );
			 glPopMatrix();
			 glColor3f(1.0,1.0,1.0);
		 }
		 else if(E->what_to_draw_core == 78){
		 	glDisable(GL_CULL_FACE);
		 	glPushMatrix();
			// set the texture map 
			glBindTexture(GL_TEXTURE_2D, E->texture[0]);
			// set the layer color
			glColor3f(E->L[0].colors.red,E->L[0].colors.green,E->L[0].colors.blue);
			glRotatef ( 0.0, 90.0, 0.0, 0.0 );
			// fill in a side
			gluDisk(t,E->L[0].start,E->L[0].end,RES,RES);
			//gluPartialDisk(t,E->L[0].start,E->L[0].end,RES,RES, -90, -90);
			glRotatef ( 90.0, 90.0, 0.0, 0.0 );
			gluDisk(u,E->L[0].start,E->L[0].end,RES,RES);
			//gluPartialDisk(t,E->L[0].start,E->L[0].end,RES,RES, -90, -90);
			glRotatef ( 90.0, 0.0, 90.0, 0.0 );
			gluDisk(v,E->L[0].start,E->L[0].end,RES,RES);
			//gluPartialDisk(t,E->L[0].start,E->L[0].end,RES,RES, -90, -90);
			glPopMatrix();	
		 	glEnable(GL_CULL_FACE);
		 }
		 else if(E->what_to_draw_core == 12){
		 	glDisable(GL_CULL_FACE);
		 	glPushMatrix();
			// set the texture map 
			glBindTexture(GL_TEXTURE_2D, E->texture[0]);
			// set the layer color
			glColor3f(E->L[0].colors.red,E->L[0].colors.green,E->L[0].colors.blue);
			glRotatef ( 90.0, 90.0, 0.0, 0.0 );
			gluDisk(u,E->L[0].start,E->L[0].end,RES,RES);
			glPopMatrix();	
		 	glEnable(GL_CULL_FACE);
		 }
		 else if(E->what_to_draw_core == 14){
		 	
		 	glDisable(GL_CULL_FACE);
		 	glPushMatrix();
			// set the texture map 
			glBindTexture(GL_TEXTURE_2D, E->texture[0]);
			// set the layer color
			glColor3f(E->L[0].colors.red,E->L[0].colors.green,E->L[0].colors.blue);
			glRotatef ( 0.0, 90.0, 0.0, 0.0 );
			// fill in a side
				//gluDisk(t,E->L[0].start,E->L[0].end,RES,RES);
			gluPartialDisk(t,E->L[0].start,E->L[0].end,RES,RES, 90, 90);
			glRotatef ( 90.0, -90.0, 0.0, 0.0 );
				//gluDisk(u,E->L[0].start,E->L[0].end,RES,RES);
			gluPartialDisk(t,E->L[0].start,E->L[0].end,RES,RES, 90, 90);
			glRotatef ( -90.0, 0.0, -90.0, 0.0 );
				//gluDisk(v,E->L[0].start,E->L[0].end,RES,RES);
			gluPartialDisk(t,E->L[0].start,E->L[0].end,RES,RES, 90, 90);
			glPopMatrix();	
		 	glEnable(GL_CULL_FACE);
		 	
		 }
		 else if(E->what_to_draw_core == 34){
		 	glDisable(GL_CULL_FACE);
		 	glPushMatrix();
		 	// set the texture map 
			glBindTexture(GL_TEXTURE_2D, E->texture[0]);
			// set the layer color
			glColor3f(E->L[0].colors.red,E->L[0].colors.green,E->L[0].colors.blue);
		 
				// fill in a side
				glRotatef ( 0.0, 90.0, 0.0, 0.0 );
				//gluDisk(t,E->L[i].start,E->L[i].end,RES,RES);
				gluPartialDisk(t,E->L[0].start,E->L[0].end,RES,RES, 0, 180);
					// don't do these ones
					//glRotatef ( 90.0, 90.0, 0.0, 0.0 );
					//gluDisk(u,E->L[i].start,E->L[i].end,RES,RES);
					//gluPartialDisk(u,E->L[i].start,E->L[i].end,RES,RES, -90, -90);
				
				glRotatef ( 90.0, 0.0, 90.0, 0.0 );
				//gluDisk(v,E->L[i].start,E->L[i].end,RES,RES);
				gluPartialDisk(t,E->L[0].start,E->L[0].end,RES,RES, 0, -180);
				glPopMatrix();
			 glEnable(GL_CULL_FACE);
		 }
	}
	
	gluDeleteQuadric ( s );
	gluDeleteQuadric ( t );
	gluDeleteQuadric ( u );
	gluDeleteQuadric ( v );
	gluDeleteQuadric ( w );
	gluDeleteQuadric ( a );
	gluDeleteQuadric ( b );
	gluDeleteQuadric ( c );
	
    
	glPopMatrix();
	
}
