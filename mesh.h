#ifndef _MESH_H_
#define _MESH_H_

#include "algebra.h"

typedef struct _Triangle {
	int vInds[3]; //vertex indices
} Triangle;

typedef struct _Mesh { 
	int nv;				
	Vector *vertices;
	Vector *vnorms; //Förklaring från Afshin: Det är bättre att utgå från verts isf för center of mass för trianglarna pga. shaders
	int nt;				
	Triangle *triangles;
	struct _Mesh *next; 
		
	Vector translation;
	Vector rotation;
	Vector scale;
	unsigned int vbo, ibo, vao; // OpenGL handles for rendering (Vi använder dem för att hantera objekt som ligger på grafikkortets minne eftersom vi inte kan peka dit)
} Mesh;

typedef struct _Camera {
	Vector position;
	Vector rotation;
	double fov; 
	double nearPlane; 
	double farPlane;
	Vector eye;
	Vector up;
	Vector center;
} Camera;

void insertModel(Mesh ** objlist, int nv, float * vArr, int nt, int * tArr, float scale = 1.0);

void RotateMesh(Mesh *mesh, float rot);

Matrix LocalToWorld(Vector t, Vector r, Vector s);
void LoadObj(Mesh **list, const char* filename, float scale);
#endif
