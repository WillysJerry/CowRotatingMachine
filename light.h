#pragma once
#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "algebra.h"

typedef struct _PointLight{
	Vector pos;
	float intensity;
	float attenuation;
	float ambient;
	Vector color;
	_PointLight *next;
} PointLight;

#endif