#ifndef _LOADER_H_
#define _LOADER_H_

/*	############### LOADER ###############
	Used to load resources (textures, etc)
	###################################### */

#include <glew.h>
#include <freeglut.h>
#include <stdio.h>
#include "mesh.h"

GLuint loadBMP(const char* filepath);
GLuint loadTGA(const char* filepath);
Mesh* loadObj(const char* filepath, unsigned int vsize = 1000, unsigned int tsize = 1000, unsigned int stepsize = 1000);

#endif