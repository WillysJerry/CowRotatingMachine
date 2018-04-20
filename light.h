#pragma once
#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "algebra.h"

typedef struct _PointLight{
	Vector pos;
	Vector ambient;
	Vector diffuse;
	Vector specular;
	float attenuation;
	_PointLight *next;
} PointLight;

#endif