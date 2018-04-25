#ifndef _LOADER_H_
#define _LOADER_H_

/*	############### LOADER ###############
	Used to load resources (textures, etc)
	###################################### */

#include <glew.h>
#include <freeglut.h>
#include <stdio.h>
#include "mesh.h"

GLint loadBMP(const char* filepath);
Mesh* loadObj(const char* filepath, unsigned int vsize, unsigned int nsize);

#endif