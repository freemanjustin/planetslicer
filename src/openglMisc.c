#include "slicer.h"
#include <time.h>

void WindowDump_PPM(void)
{
	int i,j;
	FILE *fptr;
	static int counter = 0; /* This supports animation sequences */
	char fname[64];
	//char fname2[64];
	//char sysCommand[256];
	unsigned char *image;
	GLint width;
	GLint height;
	GLint viewport[4];

	glGetIntegerv (GL_VIEWPORT, viewport);
	width = viewport[2];
	height = viewport[3];
	
	//width = screenWidth;
	//height = screenHeight;
	
	//printf("width = %d  height = %d \n",width,height);
	
	
	// Allocate our buffer for the image
	if ((image = malloc(3*width*height*sizeof(char))) == NULL) {
		fprintf(stderr,"Failed to allocate memory for image \n");
		return;
	}
	
	glPixelStorei(GL_PACK_ALIGNMENT,1);
	
	// Open the file
	sprintf(fname,"window%04d.ppm",counter);
	//sprintf(fname2,"window%04d.jpg",counter);
	
	if ((fptr = fopen(fname,"w")) == NULL) {
		fprintf(stderr,"Failed to open file for window dump \n");
		return;
	}
	
	// Copy the image into our buffer
	glReadBuffer(GL_BACK_LEFT);
	glReadPixels(0,0,width,height,GL_RGB,GL_UNSIGNED_BYTE,image);
	
	// Write the raw file
	fprintf(fptr,"P6\n%d %d\n255\n",(int)width,(int)height); //for ppm 
	for (j=height-1;j>=0;j--) {
		for (i=0;i<width;i++) {
			fputc(image[3*j*width+3*i+0],fptr);
			fputc(image[3*j*width+3*i+1],fptr);
			fputc(image[3*j*width+3*i+2],fptr);
		}
	}
	
	fclose(fptr);
	
	// Clean up
	counter++;
	free(image);
	//sprintf(sysCommand,"convert -type TrueColorMatte %s %s",fname,fname2);
	//sprintf(sysCommand,"convert %s %s",fname,fname2);
	//system(sysCommand);
	//sprintf(sysCommand,"rm %s",fname);
	//system(sysCommand);
	
	printf("Saved PPM image\n");	
}

void WindowDump_PNG(void)
{
	int i;
	FILE *fptr;
	static int counter = 0; /* This supports animation sequences */
	char fname[64];
	unsigned char *image;
	GLint width;
	GLint height;
	GLint viewport[4];
	
	// libpng stuff
	int rowStride;
	png_structp png_ptr;
    png_infop info_ptr;
    png_bytep *row_pointers;

	glGetIntegerv (GL_VIEWPORT, viewport);
	width = viewport[2];
	height = viewport[3];
	
	rowStride = (width * 3 + 3) & ~0x3;
	
	// Allocate our buffer for the image
	if ((image = malloc(3*width*height*sizeof(char))) == NULL) {
		fprintf(stderr,"Failed to allocate memory for image \n");
		return;
	}
	
	glPixelStorei(GL_PACK_ALIGNMENT,1);
	
	// Open the file
	sprintf(fname,"window%04d.png",counter);
	
	if ((fptr = fopen(fname,"w")) == NULL) {
		fprintf(stderr,"Failed to open file for window dump \n");
		return;
	}
	
	// Copy the image into our buffer
	glReadBuffer(GL_BACK_LEFT);
	glReadPixels(0,0,width,height,GL_RGB,GL_UNSIGNED_BYTE,image);
	
	// now we have the image data in `image'
	// write it to disk as a png image
	
	// first set up the png structs
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,NULL, NULL, NULL); 
	info_ptr = png_create_info_struct(png_ptr);
	setjmp(png_jmpbuf(png_ptr));
	
	png_init_io(png_ptr, fptr);
	
	png_set_IHDR(png_ptr, info_ptr, 
        width,                          /* Width */
        height,                         /* Height */
        8,                              /* Bit depth */ 
        PNG_COLOR_TYPE_RGB,             /* Color type */
        PNG_INTERLACE_NONE,             /* Interlacing */
        PNG_COMPRESSION_TYPE_DEFAULT,   /* Compression */
        PNG_FILTER_TYPE_DEFAULT);       /* Filter method */
    
	row_pointers = png_malloc(png_ptr, height * sizeof(png_bytep));
	for (i=0 ; i<height ; i++ )
		row_pointers[i] = (png_bytep) &image[rowStride * (height - i - 1)];
	
	png_set_rows(png_ptr, info_ptr, row_pointers);
	
	png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

    // Clean up
    png_destroy_write_struct(&png_ptr, &info_ptr);
	counter++;
	free(image);
	fclose(fptr);
	printf("Saved png image\n");	
}

void write_xml(e *E)
{
	FILE *fptr;
	static int counter = 0; /* This supports animation sequences */
	char fname[64];
	time_t curtime;
    struct tm *loctime;
    int	i;
 
    // Get the current time
    curtime = time (NULL);

    // Convert it to local time representation
    loctime = localtime (&curtime);

	// Open the file
	sprintf(fname,"window%04d.xml",counter);
	
	if ((fptr = fopen(fname,"w")) == NULL) {
		fprintf(stderr,"Failed to open file for window dump \n");
		return;
	}
	
	fprintf(fptr,"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    //fprintf(fptr,"\n\t<!-- xml saved at %s -->",lr_pack(asctime (loctime)) );
    //fprintf(fptr,"\n");
    
    fprintf(fptr,"<SlicerData>\n");
    fprintf(fptr,"\t<!-- slicer parameters -->\n");
    
    fprintf(fptr,"\t<params>\n");
    fprintf(fptr,"\t\t<geometry name=\"sphere\">%d</geometry>\n", E->what_to_draw);
    fprintf(fptr,"\t\t<geometry name=\"core\">%d</geometry>\n", E->what_to_draw_core);
    fprintf(fptr,"\t\t<backgroundColor> %f %f %f </backgroundColor>\n", E->bg.red, E->bg.green, E->bg.blue);
    fprintf(fptr,"\t\t<window> %d %d </window>\n", (int)E->xWinSize, (int)E->yWinSize);
    fprintf(fptr,"\t\t<camera> %f %f %f </camera>\n", E->theta[0], E->theta[1], E->theta[2]);
    fprintf(fptr,"\t\t<zoom> %f </zoom>\n", E->viewer[2]);
	fprintf(fptr,"\t</params>\n\n");
	
	for(i=0;i<E->numLayers;i++){
		fprintf(fptr,"\t<layer name=\"%s\">\n", E->L[i].name);
		fprintf(fptr,"\t\t<start_radius> %f </start_radius>\n",E->L[i].start);
		fprintf(fptr,"\t\t<end_radius> %f </end_radius>\n",E->L[i].end);
		fprintf(fptr,"\t\t<texture> %s </texture>\n",E->L[i].textureMap);
		fprintf(fptr,"\t\t<color> %f %f %f </color>\n",E->L[i].colors.red, E->L[i].colors.green, E->L[i].colors.blue);
		fprintf(fptr,"\t</layer>\n");
	}
	fprintf(fptr,"</SlicerData>\n\n");
	
	fclose(fptr);
	
}
