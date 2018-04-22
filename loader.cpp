#include "loader.h"

GLint loadBMP(const char* filepath) {

	// Data read from the header of the BMP file
	unsigned char header[54]; // Each BMP file begins by a 54-bytes header
	unsigned int dataPos;     // Position in the file where the actual data begins
	unsigned int width, height;
	unsigned int imageSize;   // = width*height*3

	// Actual RGB data
	unsigned char * data;

	FILE* fp = fopen(filepath, "rb");
	if (!fp) {
		printf("Image file could not be opened.\n");
		return 0;
	}

	if (fread(header, 1, 54, fp) != 54) { // If not 54 bytes read : problem
		printf("Not a correct BMP file.\n");
		return 0;
	}

	if (header[0] != 'B' || header[1] != 'M') {	// The BMP header always begins with 'B' and 'M'
		printf("Not a correct BMP file.\n");
		return 0;
	}

	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);

	// Some BMP files are misformatted, guess missing information
	if (imageSize == 0)    
		imageSize = width * height * 3; // 3 : one byte for each Red, Green and Blue component
	if (dataPos == 0)      
		dataPos = 54; // The BMP header is done that way

	data = (unsigned char*)malloc(imageSize);

	// Read the actual data from the file into the buffer
	fread(data, 1, imageSize, fp);

	// Data is in memory, close the file
	fclose(fp);

	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	printf("Loaded image %s\n", filepath);
	return textureID;

}

Mesh* loadObj(const char* filepath) {
	FILE* fp = fopen(filepath, "rb");
	if(!fp) {
		printf("Obj file could not be opened.\n");
		return NULL;
	}

	Mesh* m = (Mesh*)malloc(sizeof(Mesh));
	m->vertices = NULL;
	m->vnorms = NULL;
	m->uvs = NULL;
	m->triangles = NULL;

	char buff[200];
	int v = 0, vt = 0, vn = 0, f = 0;
	float a, b, c;
	int f1v, f1t, f1n, f2v, f2t, f2n, f3v, f3t, f3n;

	Vector* norms = NULL;
	Vector* uvs = NULL;

	while (!feof(fp)) {
		fgets(buff, 200, fp);

		if (buff[0] == 'v' && buff[1] == ' ') {
			// Vertex
			v++;
		}
		else if (buff[0] == 'v' && buff[1] == 't') {
			// UV coord
			vt++;
		}
		else if (buff[0] == 'v' && buff[1] == 'n') {
			// Vertex normal
			vn++;
		}
		else if (buff[0] == 'f' && buff[1] == ' ') {
			// Face
			f++;
		}
	}

	fseek(fp, 0, SEEK_SET);

	m->vertices = (Vector*)malloc(sizeof(Vector) * v);
	m->vnorms = (Vector*)malloc(sizeof(Vector) * vn);
	m->uvs = (Vector*)malloc(sizeof(Vector) * vt);
	m->triangles = (Triangle*)malloc(sizeof(Triangle) * f);

	norms = (Vector*)malloc(sizeof(Vector) * vn);
	uvs = (Vector*)malloc(sizeof(Vector) * vt);

	while (!feof(fp)) {
		fgets(buff, 200, fp);

		if (buff[0] == 'v' && buff[1] == ' ') {
			// Vertex
			//v++;
			sscanf(&buff[2], "%f %f %f", &a, &b, &c);

			//m->vertices = (Vector*)realloc(m->vertices, sizeof(Vector) * v);
			m->vertices[v - 1] = { a, b, c };
		}
		else if (buff[0] == 'v' && buff[1] == 't') {
			// UV coord
			//vt++;
			sscanf(&buff[2], "%f %f", &a, &b);

			//uvs = (Vector*)realloc(uvs, sizeof(Vector) * vt);
			uvs[vt - 1] = { a, b, 0 };

			m->uvs = (Vector*)realloc(m->uvs, sizeof(Vector) * vt);
		}
		else if (buff[0] == 'v' && buff[1] == 'n') {
			// Vertex normal
			//vn++;

			sscanf(&buff[2], "%f %f %f", &a, &b, &c);

			//norms = (Vector*)realloc(norms, sizeof(Vector) * vn);
			//m->vnorms = (Vector*)realloc(m->vnorms, sizeof(Vector) * vn);

			norms[vn - 1] = { a, b, c };
		}
		else if (buff[0] == 'f' && buff[1] == ' ') {
			// Face
			//f++;

			// FORMAT: vert index/uv index/normal index x3
			sscanf(&buff[2], "%d/%d/%d %d/%d/%d %d/%d/%d", &f1v, &f1t, &f1n, &f2v, &f2t, &f2n, &f3v, &f3t, &f3n);

			//m->triangles = (Triangle*)realloc(m->triangles, sizeof(Triangle) * f);
			m->triangles[f - 1] = { {f1v - 1, f2v - 1, f3v - 1} };

			m->vnorms[f1v - 1] = norms[f1n - 1];
			m->vnorms[f2v - 1] = norms[f2n - 1];
			m->vnorms[f3v - 1] = norms[f3n - 1];

			m->uvs[f1v - 1] = uvs[f1t - 1];
			m->uvs[f2v - 1] = uvs[f2t - 1];
			m->uvs[f3v - 1] = uvs[f3t - 1];
		}
	}

	m->nt = f;
	m->nv = v;

	printf("Done loading mesh %s.\n", filepath);
	return m;
}