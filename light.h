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

void addPointLight(PointLight **list, Vector pos, Vector ambient = { 0.1f, 0.1f, 0.1f }, Vector diffuse = { 0.7f, 0.7f, 0.7f }, Vector specular = { 1, 1, 1 }, float attenuation = 0.002f);

#endif