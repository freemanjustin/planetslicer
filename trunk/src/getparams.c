#include "slicer.h"
// Left justify and right pack a string
char *lr_pack( char *c )
{
	int j ;

	// Left pack
	while(strlen(c)>0 && *c==' ')
	    for(j=0;j<(int)strlen(c);j++) *(c+j) = *(c+j+1);
	    
	// get rid of tabs
	while( strlen(c)>0 && *c=='\t' )
	    for( j=0 ; j<(int)strlen(c) ; j++ ) *(c+j) = *(c+j+1) ;

	// Get rid of blanks on the right
	while(strlen(c)>0 && *(c+strlen(c)-1)==' ') *(c+strlen(c)-1) = '\x0'; 

	return(c);
}

void setDefaults(e *E)
{
	// set defaults for the E struct


}
void getparams_xml(char* parameter_filename, e *E)
{
    xmlDocPtr doc;
	xmlNodePtr cur;
	xmlChar *txt;
	
	E->numLayers = 0;
	
	setDefaults(E);
	
	doc = xmlParseFile(parameter_filename);
	
	if (doc == NULL ) {
		fprintf(stderr,"Could not open input file %s\n", parameter_filename);
		exit(1);
	}
	
	cur = xmlDocGetRootElement(doc);
	
	if (cur == NULL) {
		fprintf(stderr,"empty input file %s\n", parameter_filename);
		xmlFreeDoc(doc);
		exit(1);
	}
	
	if (xmlStrcmp(cur->name, (const xmlChar *) "SlicerData")) {
		fprintf(stderr,"%s is the wrong type, root node != SlicerData\n", parameter_filename);
		xmlFreeDoc(doc);
		exit(1);
	}
	
	// minimal parameters in an input file are:
	//
	//	1 sphere geometry
	// 	at least 1 layer
	
	cur = cur->xmlChildrenNode;
	while (cur != NULL) {
		// read in runtime control parameter
		if ((!xmlStrcmp(cur->name, (const xmlChar *) "params"))){
            //printf( "params :\n" );
			parseInputFile_params (E, doc, cur);
		}
		// read in the layer parameters
		else if ((!xmlStrcmp(cur->name, (const xmlChar *) "layer"))){
			txt = xmlGetProp( cur, (const xmlChar *) "name" );
            //printf( "layer %s:\n", txt );
            lr_pack( (char *)txt );
			
			// strdup is a GNU extension - don't use it!
			//E->L[E->numLayers].name = strdup((char*)txt);
			
			// malloc memory for the name
			E->L[E->numLayers].name = (char*)malloc(sizeof(char)*(strlen((char*)txt)+1));
			strcpy(E->L[E->numLayers].name, (char*)txt);
			
			parseInputFile_layers (E, doc, cur, txt);
			E->numLayers++;
			xmlFree( txt );
		}
		 
	cur = cur->next;
	}
	
	xmlFreeDoc(doc);
	
	//printf("There are %d layers present in %s\n", E->numLayers, parameter_filename);
	
	
	return;
 	
}

void parseInputFile_params (e *E, xmlDocPtr doc, xmlNodePtr cur) {

	xmlChar *key, *txt;
	static int	haveSphere = 0;
	static int 	haveCore = 0;
	static int	haveCamera = 0;
	static int	haveWindow = 0;
	static int	haveColor = 0;
	char 	*str;
	const char delimiters[] = " ";
	
	cur = cur->xmlChildrenNode;
	while (cur != NULL) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *) "geometry"))) {
			txt = xmlGetProp( cur, (const xmlChar *) "name" );
			
			if(!xmlStrcmp(txt,(const xmlChar *) "sphere")){	// sphere geometry
				key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
				lr_pack( (char *)key );
				E->what_to_draw = strtol((char *)key, (char **)NULL, 0);
				haveSphere++;
				//printf("got a sphere geometry: %s\n", key);
				//printf("E->what_to_draw = %d\n", E->what_to_draw);
				xmlFree(key);
				
			}
			else if(!xmlStrcmp(txt,(const xmlChar *) "core")){// core geometry
				key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
				lr_pack( (char *)key );
				E->what_to_draw_core = strtol((char *)key, (char **)NULL, 0);
				haveCore++;
				//printf("got a core geometry: %s\n", key);
				//printf("E->what_to_draw_core = %d\n", E->what_to_draw_core);
				xmlFree(key);
			}
		}
		else if((!xmlStrcmp(cur->name, (const xmlChar *) "backgroundColor"))){// backgorund color
				key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
				
				lr_pack( (char *)key );
				
				//str = strdup((char*)key);
				str = (char*)malloc(sizeof(char)*(strlen((char*)key)+1));
				strcpy(str, (char*)key);
				
				
				E->bg.red = strtof((const char *)strsep(&str, delimiters), (char **)NULL);
				E->bg.green = strtof((const char *)strsep(&str, delimiters), (char **)NULL);
				E->bg.blue = strtof((const char *)strsep(&str, delimiters), (char **)NULL);
				
				
				//printf("E->bgColor.red  = %f, E->bgColor.green = %f, E->bgColor.blue  = %f\n", 
				//					E->bgColor.red ,
				//					E->bgColor.green, 
				//					E->bgColor.blue );
				// set the background color
				//glClearColor(r,g,b, 0.0);
				//exit(1);
				haveColor++;
				xmlFree(key);
		}
		else if((!xmlStrcmp(cur->name, (const xmlChar *) "window"))){// backgorund color
				key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
				
				lr_pack( (char *)key );
				//str = strdup((char*)key);
				str = (char*)malloc(sizeof(char)*(strlen((char*)key)+1));
				strcpy(str, (char*)key);
				
				E->xWinSize = (GLint)strtol((const char *)strsep(&str, delimiters), (char **)NULL,0);
				E->yWinSize = (GLint)strtol((const char *)strsep(&str, delimiters), (char **)NULL,0);
				
				
				haveWindow++;
				xmlFree(key);
		}
		else if((!xmlStrcmp(cur->name, (const xmlChar *) "camera"))){// backgorund color
				key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
				
				lr_pack( (char *)key );
				//str = strdup((char*)key);
				str = (char*)malloc(sizeof(char)*(strlen((char*)key)+1));
				strcpy(str, (char*)key);
				
				E->theta[0] = strtof((const char *)strsep(&str, delimiters), (char **)NULL);
				E->theta[1] = strtof((const char *)strsep(&str, delimiters), (char **)NULL);
				E->theta[2] = strtof((const char *)strsep(&str, delimiters), (char **)NULL);
				haveCamera++;
				xmlFree(key);
		}
		else if((!xmlStrcmp(cur->name, (const xmlChar *) "zoom"))){// backgorund color
				key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
				
				lr_pack( (char *)key );
				E->viewer[2] = strtod((char *)key, (char **)NULL);
				
				
				xmlFree(key);
		}
		
	cur = cur->next;
	}
	
	// error check on geometry
	if(haveSphere > 1){
		fprintf(stderr,"Warning: Multiple sphere geometries in input file: using the last definition\n");
	}
	if(haveSphere == 0){
		fprintf(stderr,"Error: Missing sphere geometry in input file\n");
		exit(1);
	}
	if(haveCore > 1){
		fprintf(stderr,"Warning: Multiple core geometries in input file: using the last definition\n");
	}
	if( (haveCore == 0) && (haveSphere == 1)){
		fprintf(stderr,"Warning: Missing core geometry in input file\nSetting core geometry = sphere geometry\n");
		E->what_to_draw_core = E->what_to_draw;
	}
	if(haveCamera == 0){
		E->theta[0] = 125.0; 
		E->theta[1] = 0.0; 
		E->theta[2] = 225.0;
	
		E->viewer[0] = 0.0; 
		E->viewer[1] = 0.0; 
		E->viewer[2] = 3.0;
	}
	if(haveWindow == 0){
		E->xWinSize = E->yWinSize = 800;
	}
	if(haveColor == 0){
		E->bg.red = E->bg.green = E->bg.blue = 0.0;
	}
	//printf("\n");
    return;
}

void parseInputFile_layers (e *E, xmlDocPtr doc, xmlNodePtr cur, xmlChar *txt) {

	xmlChar *key;
	char 	*str;
	const char delimiters[] = " ";
		
	int		haveStartRadius = 0;
	int		haveEndRadius = 0;
	int		haveTexture = 0;
	int		haveColor = 0;
		
	cur = cur->xmlChildrenNode;
	while (cur != NULL) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *) "start_radius"))) {
		    key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
		    lr_pack( (char *)key );
		    //printf("start_radius: %s\n", key);
		    E->L[E->numLayers].start = strtod((char *)key, (char **)NULL);
		    //printf("E->L[%d].start = %f\n", E->numLayers, E->L[E->numLayers].start);
		    xmlFree(key);
		    haveStartRadius++;
 	    }
 	    else if ((!xmlStrcmp(cur->name, (const xmlChar *) "end_radius"))) {
		    key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
		    lr_pack( (char *)key );
		    //printf("end_radius: %s\n", key);
		    E->L[E->numLayers].end = strtod((char *)key, (char **)NULL);
		    //printf("E->L[%d].end = %f\n",E->numLayers, E->L[E->numLayers].end);
		    xmlFree(key);
		    haveEndRadius++;
 	    }
	    else if ((!xmlStrcmp(cur->name, (const xmlChar *) "texture"))) {
		    key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
		    lr_pack( (char *)key );
		    //printf("texture: %s\n", key);
		    sprintf(E->L[E->numLayers].textureMap,"%s",key);
		    //printf("E->L[%d].textureMap =%s\n",E->numLayers,E->L[E->numLayers].textureMap);
		    xmlFree(key);
		    haveTexture++;
 	    }
 	    else if ((!xmlStrcmp(cur->name, (const xmlChar *) "color"))) {
		    key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			
			lr_pack( (char *)key );
			//str = strdup((char*)key);
			str = (char*)malloc(sizeof(char)*(strlen((char*)key)+1));
			strcpy(str, (char*)key);
			
			E->L[E->numLayers].colors.red = strtof((const char *)strsep(&str, delimiters), (char **)NULL);
			E->L[E->numLayers].colors.green = strtof((const char *)strsep(&str, delimiters), (char **)NULL);
			E->L[E->numLayers].colors.blue = strtof((const char *)strsep(&str, delimiters), (char **)NULL);
			
		    //printf("color: %s.\n", key);
		    //printf("E->L[%d].rgb.r = %f, E->L[%d].rgb.g = %f, E->L[%d].rgb.b = %f\n", E->numLayers,E->L[E->numLayers].colors.red, 
		    //						E->numLayers,E->L[E->numLayers].colors.green, 
		    //						E->numLayers,E->L[E->numLayers].colors.blue);
		    xmlFree(key);
		    haveColor++;
 	    } 	    
		cur = cur->next;
	}
	
	//error check
	if(haveStartRadius > 1){
		fprintf(stderr,"Multiple start_radius in layer: %s\n", txt);
		exit(1);
	}
	if(haveStartRadius == 0){
		fprintf(stderr,"Missing start_radius in layer: %s\n", txt);
		exit(1);
	}
	
	if(haveEndRadius > 1){
		fprintf(stderr,"Multiple end_radius in layer: %s\n", txt);
		exit(1);
	}
	if(haveEndRadius == 0){
		fprintf(stderr,"Missing end_radius in layer: %s\n", txt);
		exit(1);
	}
	
	if(haveTexture == 0){
		sprintf(E->L[E->numLayers].textureMap,"NULL");
	}
	if(haveTexture > 1){
		fprintf(stderr,"Warning multiple texture definitions in layer: %s\n\tUsing the last definition\n", txt);
	}
	
	if(haveColor > 1){
		fprintf(stderr,"Warning multiple color definitions in layer: %s\n\tUsing the last definition\n", txt);
	}
	
	if(haveColor == 0){
		fprintf(stderr,"Warning color undefined for layer: %s\n\tUsing default color (1.0 1.0 1.0)\n", txt);
		E->L[E->numLayers].colors.red = 1.0;
		E->L[E->numLayers].colors.green = 1.0;
		E->L[E->numLayers].colors.blue = 1.0;
	}
	
	if( (haveColor == 0) && (haveTexture == 0)){
		fprintf(stderr,"Warning: No texture or color defined for layer: %s\n",txt);
	}
	
	//printf("\n");
    return;
}


void loadTextureMaps(e	*E)
{
	int	i;
	pngInfo infoLayer;
	pngSetStandardOrientation(1);
	pngSetViewingGamma(1.45);
	
	// load the texture maps
	// start from the center and go to the surface
	for(i=0;i<E->numLayers;i++){
		if(strncmp("NULL", E->L[i].textureMap,4) == 0){
			// don't assign a texture map
		}
		else{
			E->texture[i] = pngBind(E->L[i].textureMap, 
									PNG_BUILDMIPMAPS, 
									PNG_SOLID, 
									&infoLayer, 
									GL_CLAMP, 
									GL_NEAREST, 
									GL_NEAREST);
			if (E->texture[i] == 0) {
				fprintf(stderr,"Can't load file: %s\n",E->L[i].textureMap);
				exit(1);
			}
			/*
			else{
				printf("%s: Size=%i,%i Depth=%i Alpha=%i\n", 
									E->L[i].textureMap,
									infoLayer.Width, 
									infoLayer.Height, 
									infoLayer.Depth, 
									infoLayer.Alpha);
			
			}
			*/
		}
	}
}

