#include <stdlib.h>
#include "light.h"

void addPointLight(PointLight **list, Vector pos, Vector ambient, Vector diffuse, Vector specular, float attenuation)
{
	if (*list == 0) //Scene is initialized with calloc
		*list = NULL;
	PointLight *newLight = (PointLight*)malloc(sizeof(PointLight));
	newLight->pos = pos;
	newLight->ambient = ambient;
	newLight->diffuse = diffuse;
	newLight->specular = specular;
	newLight->attenuation = attenuation;
	newLight->next = *list;
	*list = newLight;
}
