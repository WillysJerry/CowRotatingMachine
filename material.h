#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "algebra.h"

typedef struct {
	Vector ambient;
	Vector diffuse;
	Vector specular;

	float shininess;
} Material;

#endif