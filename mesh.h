#ifndef _MESH_H_
#define _MESH_H_

#include "algebra.h"
#include "light.h"
#include "material.h"

typedef struct _Triangle {
	int vInds[3];
} Triangle;

typedef struct _Mesh { 
	int nv;				
	Vector *vertices;
	Vector *vnorms;
	int nt;				
	Triangle *triangles;
	struct _Mesh *next; 
		
	Vector translation;
	Vector rotation;
	Vector scale;
	unsigned int vbo, ibo, vao;

	Material material;
	Vector *uvs;
} Mesh;

typedef struct _Scene {
	Mesh *meshes;
	PointLight *pointLights;
} Scene;

extern Scene *scene;
extern Mesh *activeMesh;

void insertModel(Mesh ** objlist, int nv, float * vArr, int nt, int * tArr, Material mat, float scale = 1.0);
void insertMesh(Mesh** list, Mesh* mesh, Material material, float scale);
Matrix LocalToWorld(Vector t, Vector r, Vector s);
int insertModelFromFile(Mesh **list, const char* filename, Material mat);
#endif
