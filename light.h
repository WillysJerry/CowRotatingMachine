#pragma once
#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "algebra.h"

typedef struct {
	Vector pos;
	float intensity;
	float attenuation;
	Vector color;
} PointLight;

#endif