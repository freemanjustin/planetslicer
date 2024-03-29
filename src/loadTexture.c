#include "slicer.h"

static int PalettedTextures = -1;
static GLint MaxTextureSize = 0;

// for os x only ?
#ifdef _OS_X_
static double screenGamma = 2.2 / 1.45;
#endif

#ifdef _LINUX_
static double screenGamma = 1.7;
#endif

static unsigned char StencilRed = 0, StencilGreen = 0, StencilBlue = 0;
static unsigned char (*AlphaCallback)(unsigned char red, unsigned char green, unsigned char blue) = DefaultAlphaCallback;
static int StandardOrientation = 0;

static char gammaExplicit = 0;	/*if  */

#define GET(o) ((int)*(data + (o)))



unsigned int pngBind(const char *filename, int mipmap, int trans, pngInfo *info, int wrapst, int minfilter, int magfilter) {
	
	unsigned int id = SetParams(wrapst, magfilter, minfilter);

	if ( (id != 0) && (pngLoad(filename, mipmap, trans, info)) ){
		return id;
	}
	else
		return 0;
}

int  pngLoad(const char *filename, int mipmap, int trans, pngInfo *pinfo) {
	int result;
	FILE *fp = fopen(filename, "rb");
	if (fp == NULL) return 0;

	result = pngLoadF(fp, mipmap, trans, pinfo);

	if (fclose(fp) != 0) return 0;

	return result;
}


int pngLoadF(FILE *fp, int mipmap, int trans, pngInfo *pinfo) {
	GLint pack, unpack;

	unsigned char header[8];
	png_structp png;
	png_infop   info;
	png_infop   endinfo;
	png_bytep   data, data2;
   png_bytep  *row_p;
   double	fileGamma;

	png_uint_32 width, height, rw, rh;
	int depth, color;

	png_uint_32 i;

	fread(header, 1, 8, fp);
	if (!png_check_sig(header, 8)) return 0;

	png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	info = png_create_info_struct(png);
	endinfo = png_create_info_struct(png);

	png_init_io(png, fp);
	png_set_sig_bytes(png, 8);
	png_read_info(png, info);
	png_get_IHDR(png, info, &width, &height, &depth, &color, NULL, NULL, NULL);

	if (pinfo != NULL) {
		pinfo->Width  = width;
		pinfo->Height = height;
		pinfo->Depth  = depth;
	}

	if (MaxTextureSize == 0)
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &MaxTextureSize);

	#ifdef SUPPORTS_PALETTE_EXT

	#ifdef _WIN32
		if (PalettedTextures == -1)

			PalettedTextures = ExtSupported("GL_EXT_paletted_texture") && (strstr((const char *) glGetString(GL_VERSION), "1.1.0 3Dfx Beta") == NULL);



		if (PalettedTextures) {
			if (glColorTableEXT == NULL) {
				glColorTableEXT = (PFNGLCOLORTABLEEXTPROC) wglGetProcAddress("glColorTableEXT");
				if (glColorTableEXT == NULL)
					PalettedTextures = 0;
			}
		}

	#endif
	#endif


	if (PalettedTextures == -1)

		PalettedTextures = 0;


	if (color == PNG_COLOR_TYPE_GRAY || color == PNG_COLOR_TYPE_GRAY_ALPHA)
		png_set_gray_to_rgb(png);

	if (color&PNG_COLOR_MASK_ALPHA && trans != PNG_ALPHA) {
		png_set_strip_alpha(png);
		color &= ~PNG_COLOR_MASK_ALPHA;
	}

	if (!(PalettedTextures && mipmap >= 0 && trans == PNG_SOLID))
		if (color == PNG_COLOR_TYPE_PALETTE)
			png_set_expand(png);

	/*--GAMMA--*/
	checkForGammaEnv();
	if (png_get_gAMA(png, info, &fileGamma))
		png_set_gamma(png, screenGamma, fileGamma);
	else
		png_set_gamma(png, screenGamma, 1.0/2.2);

	png_read_update_info(png, info);

	data = (png_bytep) malloc(png_get_rowbytes(png, info)*height);
	row_p = (png_bytep *) malloc(sizeof(png_bytep)*height);


	for (i = 0; i < height; i++) {

		if (StandardOrientation)
			row_p[height - 1 - i] = &data[png_get_rowbytes(png, info)*i];

		else

			row_p[i] = &data[png_get_rowbytes(png, info)*i];

	}


	png_read_image(png, row_p);
	free(row_p);

	rw = SafeSize(width), rh = SafeSize(height);

	if (rw != width || rh != height) {
		const int channels = png_get_rowbytes(png, info)/width;

		data2 = (png_bytep) malloc(rw*rh*channels);

 		/* Doesn't work on certain sizes */
/* 		if (gluScaleImage(glformat, width, height, GL_UNSIGNED_BYTE, data, rw, rh, GL_UNSIGNED_BYTE, data2) != 0)
 			return 0;
*/
		Resize(channels, data, width, height, data2, rw, rh);

		width = rw, height = rh;
		free(data);
		data = data2;
	}

	{ /* OpenGL stuff */

		glGetIntegerv(GL_PACK_ALIGNMENT, &pack);

		glGetIntegerv(GL_UNPACK_ALIGNMENT, &unpack);

		glPixelStorei(GL_PACK_ALIGNMENT, 1);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);



		#ifdef SUPPORTS_PALETTE_EXT
		if (PalettedTextures && mipmap >= 0 && trans == PNG_SOLID && color == PNG_COLOR_TYPE_PALETTE) {
			png_colorp pal;
			int cols;
			GLint intf;

			if (pinfo != NULL) pinfo->Alpha = 0;
			png_get_PLTE(png, info, &pal, &cols);

			switch (cols) {
				case 1<<1:  intf = GL_COLOR_INDEX1_EXT;  break;
				case 1<<2:  intf = GL_COLOR_INDEX2_EXT;  break;
				case 1<<4:  intf = GL_COLOR_INDEX4_EXT;  break;
				case 1<<8:  intf = GL_COLOR_INDEX8_EXT;  break;
				case 1<<12: intf = GL_COLOR_INDEX12_EXT; break;
				case 1<<16: intf = GL_COLOR_INDEX16_EXT; break;
				default:
					/*printf("Warning: Colour depth %i not recognised\n", cols);*/
					return 0;
			}
			glColorTableEXT(GL_TEXTURE_2D, GL_RGB8, cols, GL_RGB, GL_UNSIGNED_BYTE, pal);
			glTexImage2D(GL_TEXTURE_2D, mipmap, intf, width, height, 0, GL_COLOR_INDEX, GL_UNSIGNED_BYTE, data);
		}
		else

		#endif

		if (trans == PNG_SOLID || trans == PNG_ALPHA || color == PNG_COLOR_TYPE_RGB_ALPHA || color == PNG_COLOR_TYPE_GRAY_ALPHA) {
			GLenum glformat;
			GLint glcomponent;

			switch (color) {
				case PNG_COLOR_TYPE_GRAY:
				case PNG_COLOR_TYPE_RGB:
				case PNG_COLOR_TYPE_PALETTE:
					glformat = GL_RGB;
					glcomponent = 3;
					if (pinfo != NULL) pinfo->Alpha = 0;
					break;

				case PNG_COLOR_TYPE_GRAY_ALPHA:
				case PNG_COLOR_TYPE_RGB_ALPHA:
					glformat = GL_RGBA;
					glcomponent = 4;
					if (pinfo != NULL) pinfo->Alpha = 8;
					break;

				default:
					/*puts("glformat not set");*/
					return 0;
			}

			if (mipmap == PNG_BUILDMIPMAPS)
				Build2DMipmaps(glcomponent, width, height, glformat, data, 1);

			else if (mipmap == PNG_SIMPLEMIPMAPS)

				Build2DMipmaps(glcomponent, width, height, glformat, data, 0);
			else
				glTexImage2D(GL_TEXTURE_2D, mipmap, glcomponent, width, height, 0, glformat, GL_UNSIGNED_BYTE, data);
		}
		else {
			png_bytep p, endp, q;
			int r, g, b, a;

			p = data, endp = p+width*height*3;
			q = data2 = (png_bytep) malloc(sizeof(png_byte)*width*height*4);

			if (pinfo != NULL) pinfo->Alpha = 8;

			#define FORSTART \
				do { \
					r = *p++; /*red  */ \
					g = *p++; /*green*/ \
					b = *p++; /*blue */ \
					*q++ = r; \
					*q++ = g; \
					*q++ = b;

			#define FOREND \
					q++; \
				} while (p != endp);

			#define ALPHA *q

			switch (trans) {
				case PNG_CALLBACK:
					FORSTART
						ALPHA = AlphaCallback((unsigned char) r, (unsigned char) g, (unsigned char) b);
					FOREND
					break;

				case PNG_STENCIL:
					FORSTART
						if (r == StencilRed && g == StencilGreen && b == StencilBlue)
							ALPHA = 0;
						else
							ALPHA = 255;
					FOREND
					break;

				case PNG_BLEND1:
					FORSTART
						a = r+g+b;
						if (a > 255) ALPHA = 255; else ALPHA = a;
					FOREND
					break;

				case PNG_BLEND2:
					FORSTART
						a = r+g+b;
						if (a > 255*2) ALPHA = 255; else ALPHA = a/2;
					FOREND
					break;

				case PNG_BLEND3:
					FORSTART
						ALPHA = (r+g+b)/3;
					FOREND
					break;

				case PNG_BLEND4:
					FORSTART
						a = r*r+g*g+b*b;
						if (a > 255) ALPHA = 255; else ALPHA = a;
					FOREND
					break;

				case PNG_BLEND5:
					FORSTART
						a = r*r+g*g+b*b;
						if (a > 255*2) ALPHA = 255; else ALPHA = a/2;
					FOREND
					break;

				case PNG_BLEND6:
					FORSTART
						a = r*r+g*g+b*b;
						if (a > 255*3) ALPHA = 255; else ALPHA = a/3;
					FOREND
					break;

				case PNG_BLEND7:
					FORSTART
						a = r*r+g*g+b*b;
						if (a > 255*255) ALPHA = 255; else ALPHA = (int) sqrt(a);
					FOREND
					break;
			}

			#undef FORSTART
			#undef FOREND
			#undef ALPHA

			if (mipmap == PNG_BUILDMIPMAPS)

				Build2DMipmaps(4, width, height, GL_RGBA, data2, 1);
			else if (mipmap == PNG_SIMPLEMIPMAPS)

				Build2DMipmaps(4, width, height, GL_RGBA, data2, 0);

			else
				glTexImage2D(GL_TEXTURE_2D, mipmap, 4, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data2);

			free(data2);
		}


		glPixelStorei(GL_PACK_ALIGNMENT, pack);

		glPixelStorei(GL_UNPACK_ALIGNMENT, unpack);

	} /* OpenGL end */

   png_read_end(png, endinfo);
	png_destroy_read_struct(&png, &info, &endinfo);

	free(data);

	return 1;
}

unsigned int SetParams(int wrapst, int magfilter, int minfilter) {
	//unsigned int id;
	GLuint id;
	
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapst);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapst);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magfilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minfilter);

	return id;
}

void checkForGammaEnv()
{
	double viewingGamma;
	char *gammaEnv = getenv("VIEWING_GAMMA");

	if(gammaEnv && !gammaExplicit)
	{
		sscanf(gammaEnv, "%lf", &viewingGamma);
		screenGamma = 2.2/viewingGamma;
	}
}

/* Returns a safe texture size to use (ie a power of 2), based on the current texture size "i" */
int SafeSize(int i) {
	int p;

	if (i > MaxTextureSize) return MaxTextureSize;

	for (p = 0; p < 24; p++)
		if (i <= (1<<p))
			return 1<<p;

	return MaxTextureSize;
}

/* Resize the texture since gluScaleImage doesn't work on everything */
void Resize(int components, const png_bytep d1, int w1, int h1, png_bytep d2, int w2, int h2) {
	const float sx = (float) w1/w2, sy = (float) h1/h2;
	int x, y, xx, yy, c;
	png_bytep d;

	for (y = 0; y < h2; y++) {
		yy = (int) (y*sy)*w1;

		for (x = 0; x < w2; x++) {
			xx = (int) (x*sx);
			d = d1 + (yy+xx)*components;

			for (c = 0; c < components; c++)
				*d2++ = *d++;
		}
	}
}

/* Replacement for gluBuild2DMipmaps so GLU isn't needed */

void Build2DMipmaps(GLint components, GLint width, GLint height, GLenum format, const unsigned char *data, int filter) {

	int level = 0;

	unsigned char *d = (unsigned char *) malloc((width/2)*(height/2)*components+4);

	const unsigned char *last = data;



	glTexImage2D(GL_TEXTURE_2D, level, components, width, height, 0, format, GL_UNSIGNED_BYTE, data);

	level++;



	while (HalfSize(components, width, height, last, d, filter)) {

		if (width  > 1) width  /= 2;

		if (height > 1) height /= 2;



		glTexImage2D(GL_TEXTURE_2D, level, components, width, height, 0, format, GL_UNSIGNED_BYTE, d);

		level++;

		last = d;

	}



	free(d);

}

unsigned char DefaultAlphaCallback(unsigned char red, unsigned char green, unsigned char blue) {
	return 255;
}

int HalfSize(GLint components, GLint width, GLint height, const unsigned char *data, unsigned char *d, int filter) {

	int x, y, c;

	int line = width*components;



	if (width > 1 && height > 1) {

		if (filter)

			for (y = 0; y < height; y += 2) {

				for (x = 0; x < width; x += 2) {

					for (c = 0; c < components; c++) {

						*d++ = (GET(0)+GET(components)+GET(line)+GET(line+components)) / 4;

						data++;

					}

					data += components;

				}

				data += line;

			}

		else

			for (y = 0; y < height; y += 2) {

				for (x = 0; x < width; x += 2) {

					for (c = 0; c < components; c++) {

						*d++ = GET(0);

						data++;

					}

					data += components;

				}

				data += line;

			}

	}

	else if (width > 1 && height == 1) {

		if (filter)

			for (y = 0; y < height; y += 1) {

				for (x = 0; x < width; x += 2) {

					for (c = 0; c < components; c++) {

						*d++ = (GET(0)+GET(components)) / 2;

						data++;

					}

					data += components;

				}

			}

		else

			for (y = 0; y < height; y += 1) {

				for (x = 0; x < width; x += 2) {

					for (c = 0; c < components; c++) {

						*d++ = GET(0);

						data++;

					}

					data += components;

				}

			}

	}

	else if (width == 1 && height > 1) {

		if (filter)

			for (y = 0; y < height; y += 2) {

				for (x = 0; x < width; x += 1) {

					for (c = 0; c < components; c++) {

						*d++ = (GET(0)+GET(line)) / 2;

						data++;

					}

				}

				data += line;

			}

		else

			for (y = 0; y < height; y += 2) {

				for (x = 0; x < width; x += 1) {

					for (c = 0; c < components; c++) {

						*d++ = GET(0);

						data++;

					}

				}

				data += line;

			}

	}

	else {

		return 0;

	}



	return 1;

}

void pngSetStandardOrientation(int standardorientation) {

	StandardOrientation = standardorientation;

}

void pngSetViewingGamma(double viewingGamma) {
	if(viewingGamma > 0) {
		gammaExplicit = 1;
		screenGamma = 2.2/viewingGamma;
	}
	else {
		gammaExplicit = 0;
		screenGamma = 2.2;
	}
}

