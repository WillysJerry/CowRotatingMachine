#include <stdlib.h>
#include <errno.h>
#include <string>
#include "mesh.h"

#include <fstream>
#include <sstream>

using namespace std;

Scene *scene = (Scene*)calloc(1, sizeof(Scene));
Mesh *activeMesh = (Mesh*)calloc(1, sizeof(Mesh));

float rnd() {
	return 2.0f * float(rand()) / float(RAND_MAX) - 1.0f;
}

void calculateMeshNormals(Mesh* mesh) {
	for (int i = 0; i < mesh->nt; i++) {
		mesh->vnorms[mesh->triangles[i].vInds[0]] =
			Normalize(
				Add(mesh->vnorms[mesh->triangles[i].vInds[0]],
					Normalize(
						CrossProduct(
							Subtract(mesh->vertices[mesh->triangles[i].vInds[1]], mesh->vertices[mesh->triangles[i].vInds[0]]), // a -> b
							Subtract(mesh->vertices[mesh->triangles[i].vInds[2]], mesh->vertices[mesh->triangles[i].vInds[0]]))))); // a -> c
		mesh->vnorms[mesh->triangles[i].vInds[1]] =
			Normalize(
				Add(mesh->vnorms[mesh->triangles[i].vInds[1]],
					Normalize(
						CrossProduct(
							Subtract(mesh->vertices[mesh->triangles[i].vInds[2]], mesh->vertices[mesh->triangles[i].vInds[1]]), //Vektor b -> a
							Subtract(mesh->vertices[mesh->triangles[i].vInds[0]], mesh->vertices[mesh->triangles[i].vInds[1]]))))); //Vektor b -> c
		mesh->vnorms[mesh->triangles[i].vInds[2]] =
			Normalize(
				Add(mesh->vnorms[mesh->triangles[i].vInds[2]],
					Normalize(
						CrossProduct(
							Subtract(mesh->vertices[mesh->triangles[i].vInds[0]], mesh->vertices[mesh->triangles[i].vInds[2]]), //Vektor c -> a
							Subtract(mesh->vertices[mesh->triangles[i].vInds[1]], mesh->vertices[mesh->triangles[i].vInds[2]]))))); //Vektor c -> b
	}
}

void insertModel(Mesh **list, int nv, float * vArr, int nt, int * tArr, Material material, float scale) {
	Mesh * mesh = (Mesh *) malloc(sizeof(Mesh));
	mesh->nv = nv;
	mesh->nt = nt;	
	mesh->vertices = (Vector *) malloc(nv * sizeof(Vector));
	mesh->vnorms = (Vector *)calloc(nv, sizeof(Vector));
	mesh->triangles = (Triangle *) malloc(nt * sizeof(Triangle));
	
	// set mesh vertices
	for (int i = 0; i < nv; i++) {
		mesh->vertices[i].x = vArr[i*3];
		mesh->vertices[i].y = vArr[i*3+1];
		mesh->vertices[i].z = vArr[i*3+2];
	}

	// set mesh triangles
	for (int i = 0; i < nt; i++) {
		mesh->triangles[i].vInds[0] = tArr[i*3];
		mesh->triangles[i].vInds[1] = tArr[i*3+1];
		mesh->triangles[i].vInds[2] = tArr[i*3+2];
	}

	// Assignment 1: 
	// Calculate and store suitable vertex normals for the mesh here.
	// Replace the code below that simply sets some arbitrary normal values
	calculateMeshNormals(mesh);

	// Set transform parameters
	mesh->translation = { 0, 0, 0 };
	mesh->rotation = { 0, 0, 0 };
	mesh->scale = { scale, scale, scale };

	mesh->material = material;

	mesh->next = *list;
	*list = mesh;	
}

void insertMesh(Mesh** list, Mesh* mesh, Material material, float scale) {

	//calculateMeshNormals(mesh);

	mesh->translation = { 0, 0, 0 };
	mesh->rotation = { 0, 0, 0 };
	mesh->scale = { scale, scale, scale };
	mesh->material = material;

	mesh->next = *list;
	*list = mesh;
}

Matrix LocalToWorld(Vector t, Vector r, Vector s) {
	Matrix T = Translate(t.x, t.y, t.z);
	Matrix Rx = RotateX(r.x);
	Matrix Ry = RotateY(r.y);
	Matrix Rz = RotateZ(r.z);
	Matrix S = Scale(s.x, s.y, s.z);

	Matrix W = MatMatMul(T, MatMatMul(Rx, MatMatMul(Ry, MatMatMul(Rz, S))));
	return W;
}

int insertModelFromFile(Mesh **list, const char* filename, Material material) {
	Mesh* mesh;
	mesh = (Mesh*)malloc(sizeof(Mesh));

	mesh->nv = 0;
	mesh->nt = 0;
	string line;

	//Open the file
	ifstream infile(filename);
	if (!infile) {
		printf("Cannot open %s\n", filename);
		return -1;
	}

	//Determines the size of arrays
	while (getline(infile, line)) {
		if (line.substr(0, 2) == "v ") {
			mesh->nv += 1;
		}
		else if (line.substr(0, 2) == "f ") {
			mesh->nt += 1;
		}
	}

	//Allocates memory for arrays
	mesh->vertices = (Vector *)malloc(mesh->nv * sizeof(Vector));
	mesh->vnorms = (Vector *)calloc(mesh->nv, sizeof(Vector));
	mesh->triangles = (Triangle *)malloc(mesh->nt * sizeof(Triangle));

	//Resets stream to begining of file
	infile.clear();
	infile.seekg(0, ios::beg);

	//Fills arrays with values
	int i = 0, j = 0;
	std::size_t posA, posB;
	while (getline(infile, line)) {
		if (line.substr(0, 2) == "v ") {
			istringstream s(line.substr(2));
			s >> mesh->vertices[i].x;
			s >> mesh->vertices[i].y;
			s >> mesh->vertices[i].z;
			i++;
		}
		else if (line.substr(0, 2) == "f ") {
			//fulhack
			//f 4059/1/1 4050/2/1 4054/3/1
			//Vi vill gå från position 2 till pos för första '/' och efter det vill vi gå till första whitespace
			//Efter whitespace vill vi läsa in till första '/' igen och efter det vill vi gå till första whitespace
			//Efter whitespace vill vi läsa in till första '/' och sedan är det slut.
			posA = line.find("/");
			istringstream a(line.substr(2, posA));
			a >> mesh->triangles[j].vInds[0];
			posB = line.find(" ", posA + 1);
			posA = line.find("/", posB + 1);
			istringstream b(line.substr(posB + 1, posA));
			b >> mesh->triangles[j].vInds[1];
			posB = line.find(" ", posA + 1);
			posA = line.find("/", posB + 1);
			istringstream c(line.substr(posB + 1, posA));
			c >> mesh->triangles[j].vInds[2];

			mesh->triangles[j].vInds[0]--;
			mesh->triangles[j].vInds[1]--;
			mesh->triangles[j].vInds[2]--;
			j++;
		}
	}

	calculateMeshNormals(mesh);
	

	mesh->translation = { 0, 0, 0 };
	mesh->rotation = { 0, 0, 0 };
	mesh->scale = { 1, 1, 1 };

	mesh->material = material;

	mesh->next = *list;
	*list = mesh;
	return 0;
}


// Computes mesh tangents and bitangents based on:
// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-13-normal-mapping/#computing-the-tangents-and-bitangents
void computeTangentBasis(Mesh* mesh) {
	free(mesh->tangents);
	free(mesh->bitangents);

	mesh->tangents = (Vector*)malloc(sizeof(Vector) * mesh->nv);
	mesh->bitangents = (Vector*)malloc(sizeof(Vector) * mesh->nv);

	for (int i = 0; i < mesh->nv; i += 3) {
		Vector v0 = mesh->vertices[i + 0];
		Vector v1 = mesh->vertices[i + 1];
		Vector v2 = mesh->vertices[i + 2];

		Vector uv0 = Vector{ mesh->uvs[i + 0].x, mesh->uvs[i + 0].y, 0 };
		Vector uv1 = Vector{ mesh->uvs[i + 1].x, mesh->uvs[i + 1].y, 0 };
		Vector uv2 = Vector{ mesh->uvs[i + 2].x, mesh->uvs[i + 2].y, 0 };

		Vector deltaPos1 = Subtract(v1, v0);
		Vector deltaPos2 = Subtract(v2, v0);

		Vector deltaUV1 = Subtract(uv1, uv0);
		Vector deltaUV2 = Subtract(uv2, uv0);

		float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
		Vector tangent = ScalarVecMul(r, Subtract(ScalarVecMul(deltaUV2.y, deltaPos1), ScalarVecMul(deltaUV1.y, deltaPos2)));
		Vector bitangent = ScalarVecMul(r, Subtract(ScalarVecMul(deltaUV1.x, deltaPos2), ScalarVecMul(deltaUV2.x, deltaPos1)));
		

		mesh->tangents[i + 0] = Vector{ tangent.x, tangent.y, tangent.z };
		mesh->tangents[i + 1] = Vector{ tangent.x, tangent.y, tangent.z };
		mesh->tangents[i + 2] = Vector{ tangent.x, tangent.y, tangent.z };

		mesh->bitangents[i + 0] = Vector{ bitangent.x, bitangent.y, bitangent.z };
		mesh->bitangents[i + 1] = Vector{ bitangent.x, bitangent.y, bitangent.z };
		mesh->bitangents[i + 2] = Vector{ bitangent.x, bitangent.y, bitangent.z };
	}

	mesh->normalMapped = true;
}