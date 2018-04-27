#include <stdlib.h>
#include "loader.h"

GLuint loadBMP(const char* filepath) {

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

struct TGA {
	GLubyte header[6];
	GLuint imageSize;
	GLuint imageWidth;
	GLuint imageHeight;
	GLuint bytesPerPixel;
	GLuint type;
};

GLubyte *loadUncompressedTGA(FILE* fp, struct TGA *tga)
{
	// Allocate Memory
	GLubyte *data = (GLubyte *)malloc(tga->imageSize);
	if (data == NULL)
	{
		printf("Error: Memory allocation failed in loadUncompressedTGA.");
		return NULL;
	}

	if (fread(data, 1, tga->imageSize, fp) != tga->imageSize)
	{
		printf("Error: Failed to read data in loadUncompressedTGA.");
		return NULL;
	}

	// Start The Loop
	for (GLuint i = 0; i < (int)tga->imageSize; i += tga->bytesPerPixel)
	{
		// 1st Byte XOR 3rd Byte XOR 1st Byte XOR 3rd Byte
		data[i] ^= data[i + 2] ^= data[i] ^= data[i + 2];
	}

	return data;
}


//http://nehe.gamedev.net/tutorial/loading_compressed_and_uncompressed_tgas/22001/
GLubyte *loadCompressedTGA(FILE* fp, struct TGA *tga) {

	// Allocate Memory
	GLubyte *data = (GLubyte *)malloc(tga->imageSize);
	if (data == NULL)
	{
		printf("Error: Memory allocation failed in loadCompressedTGA.");
		return NULL;
	}

	GLuint pixelcount = tga->imageHeight * tga->imageWidth; // Number Of Pixels In The Image
	GLuint currentpixel = 0;            // Current Pixel We Are Reading From Data
	GLuint currentbyte = 0;            // Current Byte We Are Writing Into Imagedata
									   // Storage For 1 Pixel
	GLubyte * colorbuffer = (GLubyte *)malloc(tga->bytesPerPixel);

	do
	{
		GLubyte chunkheader = 0;            // Variable To Store The Value Of The Id Chunk
		if (fread(&chunkheader, sizeof(GLubyte), 1, fp) == 0)  // Attempt To Read The Chunk's Header
		{
			printf("Error: Failed to read header in loadCompressedTGA.");
			return NULL;
		}
		if (chunkheader < 128)                // If The Chunk Is A 'RAW' Chunk
		{
			chunkheader++;

			// Start Pixel Reading Loop
			for (short counter = 0; counter < chunkheader; counter++)
			{
				// Try To Read 1 Pixel
				if (fread(colorbuffer, 1, tga->bytesPerPixel, fp) != tga->bytesPerPixel)
				{
					printf("Error: Failed to read RAW pixel in loadCompressedTGA.");
					return NULL;
				}
				data[currentbyte] = colorbuffer[2];        // Write The 'R' Byte
				data[currentbyte + 1] = colorbuffer[1]; // Write The 'G' Byte
				data[currentbyte + 2] = colorbuffer[0]; // Write The 'B' Byte
				if (tga->bytesPerPixel == 4)                  // If It's A 32bpp Image...
				{
					data[currentbyte + 3] = colorbuffer[3];    // Write The 'A' Byte
				}
				// Increment The Byte Counter By The Number Of Bytes In A Pixel
				currentbyte += tga->bytesPerPixel;
				currentpixel++;                 // Increment The Number Of Pixels By 1
			}
		}
		else                        // If It's An RLE Header
		{
			chunkheader -= 127;         // Subtract 127 To Get Rid Of The ID Bit
										// Read The Next Pixel
			if (fread(colorbuffer, 1, tga->bytesPerPixel, fp) != tga->bytesPerPixel)
			{
				printf("Error: Failed to read RLE pixel in loadCompressedTGA.");
				return false;
			}
			// Start The Loop
			for (short counter = 0; counter < chunkheader; counter++)
			{
				// Copy The 'R' Byte
				data[currentbyte] = colorbuffer[2];
				// Copy The 'G' Byte
				data[currentbyte + 1] = colorbuffer[1];
				// Copy The 'B' Byte
				data[currentbyte + 2] = colorbuffer[0];
				if (tga->bytesPerPixel == 4)      // If It's A 32bpp Image
				{
					// Copy The 'A' Byte
					data[currentbyte + 3] = colorbuffer[3];
				}
				currentbyte += tga->bytesPerPixel;   // Increment The Byte Counter
				currentpixel++;             // Increment The Pixel Counter
			}
		}
	} while (currentpixel < pixelcount);    // More Pixels To Read? ... Start Loop Over
	fclose(fp);               // Close File

	return data;
}

GLuint loadTGA(const char* filepath)
{
	FILE* fp = fopen(filepath, "rb");

	if (!fp)
		return NULL;

	// Read the header of the TGA, compare it with the known headers for compressed and uncompressed TGAs
	unsigned char header[18];
	fread(header, sizeof(unsigned char) * 18, 1, fp);

	struct TGA tga;

	tga.imageWidth = header[13] * 256 + header[12];
	tga.imageHeight = header[15] * 256 + header[14];
	tga.bytesPerPixel = header[16] / 8;

	if (tga.bytesPerPixel == 3)    // If 24bit (3*8)
		tga.type = GL_RGB;
	else							// else it's 32 bit
		tga.type = GL_RGBA;

	tga.imageSize = (tga.bytesPerPixel * tga.imageWidth * tga.imageHeight);

	// check whether width, height an BitsPerPixel are valid
	if ((tga.imageWidth <= 0) || (tga.imageHeight <= 0) || ((tga.bytesPerPixel != 1) && (tga.bytesPerPixel != 3) && (tga.bytesPerPixel != 4)))
	{
		fclose(fp);
		return NULL;
	}

	GLubyte *data = NULL;
	// call the appropriate loader-routine
	if (header[2] == 2)
	{
		data = loadUncompressedTGA(fp, &tga);
	}
	else if (header[2] == 10)
	{
		data = loadCompressedTGA(fp, &tga);
	}
	else
	{
		fclose(fp);
		return NULL;
	}

	fclose(fp);

	GLuint textureID;
	glGenTextures(1, &textureID);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tga.imageWidth, tga.imageHeight, 0, tga.type, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	printf("Loaded image %s\n", filepath);
	return textureID;
}

Mesh* loadObj(const char* filepath, unsigned int vsize, unsigned int tsize, unsigned int stepsize) {
	FILE* fp = fopen(filepath, "rb");
	if(!fp) {
		printf("Obj file could not be opened.\n");
		return NULL;
	}

	Mesh* m = (Mesh*)calloc(1, sizeof(Mesh));

	char buff[200];
	bool reallocated = false;
	unsigned int v = 0, vt = 0, vn = 0, f = 0;
	float a, b, c;
	unsigned int f1v, f1t, f1n, f2v, f2t, f2n, f3v, f3t, f3n;
	
	unsigned int vAlloc = vsize, nAlloc = vsize, uAlloc = vsize, tAlloc = tsize;

	m->vertices = (Vector*)malloc(sizeof(Vector) * vAlloc);
	m->vnorms = (Vector*)malloc(sizeof(Vector) * nAlloc);
	m->uvs = (Vector2D*)malloc(sizeof(Vector2D) * uAlloc);
	m->triangles = (Triangle*)malloc(sizeof(Triangle) * tAlloc);

	Vector* verts = (Vector*)malloc(sizeof(Vector) * vAlloc);
	Vector* norms = (Vector*)malloc(sizeof(Vector) * nAlloc);
	Vector2D* uvs = (Vector2D*)malloc(sizeof(Vector2D) * uAlloc);

	int vi = 0, vni = 0, vti = 0;

	while (!feof(fp)) {
		fgets(buff, 200, fp);

		if (buff[0] == 'v' && buff[1] == ' ') {
			// Vertex
			v++;
			if (v > vAlloc) {
				vAlloc += stepsize;
				m->vertices = (Vector*)realloc(m->vertices, sizeof(Vector) * vAlloc);
				verts = (Vector*)realloc(verts, sizeof(Vector) * vAlloc);
				reallocated = true;
			}

			sscanf(&buff[2], "%f %f %f", &a, &b, &c);

			verts[v - 1] = { a, b, c };
		}
		else if (buff[0] == 'v' && buff[1] == 't') {
			// UV coord
			vt++;
			if (vt > uAlloc) {
				uAlloc += stepsize;
				m->uvs = (Vector2D*)realloc(m->uvs, sizeof(Vector2D) * uAlloc);
				uvs = (Vector2D*)realloc(uvs, sizeof(Vector2D) * uAlloc);
				reallocated = true;
			}

			sscanf(&buff[3], "%f %f", &a, &b);

			uvs[vt - 1] = { a, b };
		}
		else if (buff[0] == 'v' && buff[1] == 'n') {
			// Vertex normal
			vn++;
			if (vn > nAlloc) {
				nAlloc += stepsize;

				norms = (Vector*)realloc(norms, sizeof(Vector) * nAlloc);
				m->vnorms = (Vector*)realloc(m->vnorms, sizeof(Vector) * nAlloc);
				reallocated = true;
			}

			sscanf(&buff[3], "%f %f %f", &a, &b, &c);

			norms[vn - 1] = { a, b, c };
		}
		else if (buff[0] == 'f' && buff[1] == ' ') {
			// Face
			f++;

			// FORMAT: vert index/uv index/normal index x3
			sscanf(&buff[2], "%d/%d/%d %d/%d/%d %d/%d/%d", &f1v, &f1t, &f1n, &f2v, &f2t, &f2n, &f3v, &f3t, &f3n);

			if (f > tAlloc) {
				tAlloc += stepsize;
				m->triangles = (Triangle*)realloc(m->triangles, sizeof(Triangle) * tAlloc);
				reallocated = true;
			}

			if (vi > vAlloc - 3) {
				vAlloc += stepsize;
				m->vertices = (Vector*)realloc(m->vertices, sizeof(Vector) * vAlloc);
				reallocated = true;
			}

			if (vni > nAlloc - 3) {
				nAlloc += stepsize;
				m->vnorms = (Vector*)realloc(m->vnorms, sizeof(Vector) * nAlloc);
				reallocated = true;
			}

			if (vti > uAlloc - 3) {
				uAlloc += stepsize;
				m->uvs = (Vector2D*)realloc(m->uvs, sizeof(Vector2D) * uAlloc);
				reallocated = true;
			}

			//m->triangles[f - 1] = { {f1v - 1, f2v - 1, f3v - 1} };
			m->triangles[f - 1] = { { vi, vi + 1, vi + 2 } };

			memcpy(&(m->vertices[vi++]), &(verts[f1v - 1]), sizeof(Vector));
			memcpy(&(m->vertices[vi++]), &(verts[f2v - 1]), sizeof(Vector));
			memcpy(&(m->vertices[vi++]), &(verts[f3v - 1]), sizeof(Vector));

			memcpy(&(m->vnorms[vni++]), &(norms[f1n - 1]), sizeof(Vector));
			memcpy(&(m->vnorms[vni++]), &(norms[f2n - 1]), sizeof(Vector));
			memcpy(&(m->vnorms[vni++]), &(norms[f3n - 1]), sizeof(Vector));

			memcpy(&(m->uvs[vti++]), &(uvs[f1t - 1]), sizeof(Vector2D));
			memcpy(&(m->uvs[vti++]), &(uvs[f2t - 1]), sizeof(Vector2D));
			memcpy(&(m->uvs[vti++]), &(uvs[f3t - 1]), sizeof(Vector2D));

			/*m->vnorms[f1v - 1].x = norms[f1n - 1].x;
			m->vnorms[f1v - 1].y = norms[f1n - 1].y;
			m->vnorms[f1v - 1].z = norms[f1n - 1].z;

			m->vnorms[f2v - 1].x = norms[f2n - 1].x;
			m->vnorms[f2v - 1].y = norms[f2n - 1].y;
			m->vnorms[f2v - 1].z = norms[f2n - 1].z;

			m->vnorms[f3v - 1].x = norms[f3n - 1].x;
			m->vnorms[f3v - 1].y = norms[f3n - 1].y;
			m->vnorms[f3v - 1].z = norms[f3n - 1].z;*/


			/*m->uvs[f1v - 1].x = uvs[f1t - 1].x;
			m->uvs[f1v - 1].y = uvs[f1t - 1].y;

			m->uvs[f2v - 1].x = uvs[f2t - 1].x;
			m->uvs[f2v - 1].y = uvs[f2t - 1].y;

			m->uvs[f3v - 1].x = uvs[f3t - 1].x;
			m->uvs[f3v - 1].y = uvs[f3t - 1].y;*/

		}
	}

	// Trim excess memory
	m->vertices = (Vector*)realloc(m->vertices, sizeof(Vector) * vi);
	m->vnorms = (Vector*)realloc(m->vnorms, sizeof(Vector) * vni);
	m->uvs = (Vector2D*)realloc(m->uvs, sizeof(Vector2D) * vti);
	m->triangles = (Triangle*)realloc(m->triangles, sizeof(Triangle) * f);

	free(norms);
	free(uvs);
	free(verts);

	m->nt = f;
	m->nv = vi;

	if (reallocated)
		printf("Reallocated mesh \"%s\".\n\tVertices: %d\n\tNormals: %d\n\tUVs: %d\n\tTriangles: %d\n", filepath, vi, vni, vti, f);
	else
		printf("Loaded mesh %s. Vertices: %d, faces: %d.\n", filepath, vi, f);

	fclose(fp);
	return m;
}