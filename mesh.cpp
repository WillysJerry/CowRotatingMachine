#include <stdlib.h>
#include <errno.h>
#include <string>
#include "mesh.h"

using namespace std;

float rnd() {
	return 2.0f * float(rand()) / float(RAND_MAX) - 1.0f;
}

void insertModel(Mesh **list, int nv, float * vArr, int nt, int * tArr, float scale) {
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
	
	for (int i = 0; i < nt; i++) {
		

		mesh->vnorms[mesh->triangles[i].vInds[0]] =
			Normalize(
				Add(mesh->vnorms[mesh->triangles[i].vInds[0]],
					Normalize(
						CrossProduct(
							Subtract(mesh->vertices[mesh->triangles[i].vInds[1]], mesh->vertices[mesh->triangles[i].vInds[0]]), //Vektor b - a 
							Subtract(mesh->vertices[mesh->triangles[i].vInds[2]], mesh->vertices[mesh->triangles[i].vInds[0]]))))); //Vektor c - a 
		mesh->vnorms[mesh->triangles[i].vInds[1]] =
			Normalize(
				Add(mesh->vnorms[mesh->triangles[i].vInds[1]],
					Normalize(
						CrossProduct(
							Subtract(mesh->vertices[mesh->triangles[i].vInds[2]], mesh->vertices[mesh->triangles[i].vInds[1]]), //Vektor a - b 
							Subtract(mesh->vertices[mesh->triangles[i].vInds[0]], mesh->vertices[mesh->triangles[i].vInds[1]]))))); //Vektor c - b 
		mesh->vnorms[mesh->triangles[i].vInds[2]] =
			Normalize(
				Add(mesh->vnorms[mesh->triangles[i].vInds[2]],
					Normalize(
						CrossProduct(
							Subtract(mesh->vertices[mesh->triangles[i].vInds[0]], mesh->vertices[mesh->triangles[i].vInds[2]]), //Vektor a - c 
							Subtract(mesh->vertices[mesh->triangles[i].vInds[1]], mesh->vertices[mesh->triangles[i].vInds[2]]))))); //Vektor b - c 

	}




	// Set transform parameters
	mesh->translation = { 0, 0, 0 };
	mesh->rotation = { 0, 0, 0 };
	mesh->scale = { scale, scale, scale };

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

//Kom ihåg att skicka tråden till GPU:n if mesh > cow
void RotateMesh(Mesh *mesh, float rot)
{
	Vector kossa;
	HomVector ret;
	Matrix m = RotateX(rot);

	for (int i = 0; i < mesh->nv; i++)
	{
		kossa.x = mesh->vertices[i].x;
		kossa.y = mesh->vertices[i].y;
		kossa.z = mesh->vertices[i].z;
		ret = MatVecMul(m, kossa);
		kossa = Homogenize(ret);
		mesh->vertices[i].x = kossa.x;
		mesh->vertices[i].y = kossa.y;
		mesh->vertices[i].z = kossa.z;

	}
}

void LoadObj(Mesh **list, const char* filename, float scale) {
	//Open the file
	FILE * file;
	errno_t err;
	err = fopen_s(&file, filename, "r");
	if (err != 0) {
		printf("Impossible to open the file !\n");
		return;
	}

	Mesh * mesh = (Mesh *)malloc(sizeof(Mesh));

	//Determines the size of arrays
	mesh->nv = 0; mesh->nt = 0;
	while (1) {
		char lineHeader[128];
		int res = fscanf_s(file, "%s", lineHeader);
		if (res == EOF) {
			break;
		}
		else {
			if (strcmp(lineHeader, "v") == 0) {
				mesh->nv++;
			}
			else if (strcmp(lineHeader, "f") == 0) {
				mesh->nt++;
			}
		}
	}

	//Allocate memory for the arrays
	mesh->vertices = (Vector *)malloc(mesh->nv * sizeof(Vector));
	mesh->vnorms = (Vector *)calloc(mesh->nv, sizeof(Vector));
	mesh->triangles = (Triangle *)malloc(mesh->nt * sizeof(Triangle));

	//Resets file pointer
	fseek(file, 0L, SEEK_SET);

	int i = 0, j = 0;
	while (1) {
		char lineHeader[128];
		int res = fscanf_s(file, "%s", lineHeader);// Read the first word of the line

		if (res == EOF) {
			break;
		}
		else { //Load vertices and vertex indecies from OBJ
			if (strcmp(lineHeader, "v") == 0) {
				fscanf_s(file, "%f %f %f\n", &mesh->vertices[i].x, &mesh->vertices[i].y, &mesh->vertices[i].z);
				i++;
			}
			else if (strcmp(lineHeader, "f") == 0) {
				unsigned int vInds[3], temp;
				int matches = fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
					&vInds[0], &temp, &temp,
					&vInds[1], &temp, &temp,
					&vInds[2], &temp, &temp);

				if (matches != 9) {
					printf("Nope! I can't read that\n");
					return;
				}

				mesh->triangles[j].vInds[0] = vInds[0] - 1;
				mesh->triangles[j].vInds[1] = vInds[1] - 1;
				mesh->triangles[j].vInds[2] = vInds[2] - 1;
				j++;
			}
		}
	}

	//Calculate normals (same as in insertModel)
	for (i = 0; i < mesh->nt; i++) {
		mesh->vnorms[mesh->triangles[i].vInds[0]] =
			Normalize(
				Add(mesh->vnorms[mesh->triangles[i].vInds[0]],
					Normalize(
						CrossProduct(
							Subtract(mesh->vertices[mesh->triangles[i].vInds[1]], mesh->vertices[mesh->triangles[i].vInds[0]]), //Vektor b - a 
							Subtract(mesh->vertices[mesh->triangles[i].vInds[2]], mesh->vertices[mesh->triangles[i].vInds[0]]))))); //Vektor c - a 
		mesh->vnorms[mesh->triangles[i].vInds[1]] =
			Normalize(
				Add(mesh->vnorms[mesh->triangles[i].vInds[1]],
					Normalize(
						CrossProduct(
							Subtract(mesh->vertices[mesh->triangles[i].vInds[2]], mesh->vertices[mesh->triangles[i].vInds[1]]), //Vektor a - b 
							Subtract(mesh->vertices[mesh->triangles[i].vInds[0]], mesh->vertices[mesh->triangles[i].vInds[1]]))))); //Vektor c - b 
		mesh->vnorms[mesh->triangles[i].vInds[2]] =
			Normalize(
				Add(mesh->vnorms[mesh->triangles[i].vInds[2]],
					Normalize(
						CrossProduct(
							Subtract(mesh->vertices[mesh->triangles[i].vInds[0]], mesh->vertices[mesh->triangles[i].vInds[2]]), //Vektor a - c 
							Subtract(mesh->vertices[mesh->triangles[i].vInds[1]], mesh->vertices[mesh->triangles[i].vInds[2]]))))); //Vektor b - c 
	}

	mesh->translation = { 0, 0, 0 };
	mesh->rotation = { 0, 0, 0 };
	mesh->scale = { scale, scale, scale };

	mesh->next = *list;
	//*list = mesh;
	return 0;
}

int LoadObj2(Mesh **list, const char* filename)
{
	Mesh* mesh;
	mesh = (Mesh*) malloc(sizeof(Mesh));

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
			j++;
		}
	}

	//fixa till resten av datat för mesh och lägg in i listan korrekt
	mesh->next = NULL;
	*list = mesh;
	return 0;
}
	*list = mesh;
}