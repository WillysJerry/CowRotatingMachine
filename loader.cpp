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