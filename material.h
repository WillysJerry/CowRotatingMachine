#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include <glew.h>
#include <freeglut.h>
#include "algebra.h"

typedef struct {
	Vector ambient;
	Vector diffuse;
	Vector specular;
	GLint texture;

	float shininess;
} Material;

#endif