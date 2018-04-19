#include "shader.h"

GLuint prepareShaderProgram(const char ** vs_src, const char ** fs_src) {
	GLint success = GL_FALSE;
	GLuint shprg = glCreateProgram();

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, vs_src, NULL);
	glCompileShader(vs);
	glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
	if (!success) printf("Error in vertex shader!\n");
	else printf("Vertex shader compiled successfully!\n");

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, fs_src, NULL);
	glCompileShader(fs);
	glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
	if (!success) printf("Error in fragment shader!\n");
	else printf("Fragment shader compiled successfully!\n");

	glAttachShader(shprg, vs);
	glAttachShader(shprg, fs);
	glLinkProgram(shprg);
	GLint isLinked = GL_FALSE;
	glGetProgramiv(shprg, GL_LINK_STATUS, &isLinked);
	if (!isLinked) printf("Link error in shader program!\n");
	else printf("Shader program linked successfully!\n");

	return shprg;
}

void prepareMesh(Mesh *mesh, GLuint shader) {
	int sizeVerts = mesh->nv * 3 * sizeof(float);
	int sizeCols = mesh->nv * 3 * sizeof(float);
	int sizeTris = mesh->nt * 3 * sizeof(int);

	// For storage of state and other buffer objects needed for vertex specification
	glGenVertexArrays(1, &mesh->vao);
	glBindVertexArray(mesh->vao);

	// Allocate VBO and load mesh data (vertices and normals)
	glGenBuffers(1, &mesh->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeVerts + sizeCols, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeVerts, (void *)mesh->vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeVerts, sizeCols, (void *)mesh->vnorms);

	// Allocate index buffer and load mesh indices
	glGenBuffers(1, &mesh->ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeTris, (void *)mesh->triangles, GL_STATIC_DRAW);

	// Define the format of the vertex data
	GLint vPos = glGetAttribLocation(shader, "vPos");
	glEnableVertexAttribArray(vPos);
	glVertexAttribPointer(vPos, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	// Define the format of the vertex data 
	GLint vNorm = glGetAttribLocation(shader, "vNorm");
	glEnableVertexAttribArray(vNorm);
	glVertexAttribPointer(vNorm, 3, GL_FLOAT, GL_FALSE, 0, (void *)(mesh->nv * 3 * sizeof(float)));

	glBindVertexArray(0);

}

void readShaderFile(const char file[], const char *shader[]) {
	int i = 0, bufblock = 128, buflen = bufblock;
	char *buffer = (char*)malloc(buflen);
	int ch;
	FILE *fp;
	fp = fopen(file, "r");
	while ((ch = fgetc(fp)) != EOF) {
		buffer[i++] = ch;
		if (i >= buflen) {
			buflen += bufblock;							// Get another block
			buffer = (char*)realloc(buffer, buflen);	// to save stuff
		}
	}
	fclose(fp);
	//buffer[i] = '\0';
	buffer = (char*)realloc(buffer, i + 1);				//Remove unused space, save that memory
	shader[0] = buffer;
}

void renderMesh(Mesh *mesh, Matrix V, Matrix P, Matrix PV) {

	static Matrix W, M;

	//GLuint shprg = player->shprg;
	//PointLight *light = scene->pointLights;
	//Camera *cam = player->cam;

	// Assignment 1: Apply the transforms from local mesh coordinates to world coordinates here
	// Combine it with the viewing transform that is passed to the shader below
	Vector radRotation = { Deg2Rad(mesh->rotation.x), Deg2Rad(mesh->rotation.y), Deg2Rad(mesh->rotation.z) };

	W = LocalToWorld(mesh->translation, radRotation, mesh->scale);
	M = MatMatMul(PV, W);

	// Pass the viewing transform to the shader
	GLint loc_PV = glGetUniformLocation(player->shprg, "PV");
	glUniformMatrix4fv(loc_PV, 1, GL_FALSE, M.e);
	//GLint loc_Mod = glGetUniformLocation(shprg, "model");
	//glUniformMatrix4fv(loc_Mod, 1, GL_FALSE, W.e);
	//GLint loc_Vie = glGetUniformLocation(shprg, "view");
	//glUniformMatrix4fv(loc_Vie, 1, GL_FALSE, V.e);
	//GLint loc_Pro = glGetUniformLocation(shprg, "projection");
	//glUniformMatrix4fv(loc_Pro, 1, GL_FALSE, P.e);

	//GLint loc_OC = glGetUniformLocation(shprg, "objectColor");
	//glUniform3f(loc_OC, 1.0f, 1.0f, 1.0f);
	//GLint loc_LC = glGetUniformLocation(shprg, "lightColor");
	//glUniform3f(loc_LC, light->color.x, light->color.y, light->color.z);
	//GLint loc_LP = glGetUniformLocation(shprg, "lightPos");
	//glUniform3f(loc_LP, light->pos.x, light->pos.y, light->pos.z);
	//GLint loc_VP = glGetUniformLocation(shprg, "viewPos");
	//glUniform3f(loc_VP, cam->position.x, cam->position.y, cam->position.z);
	//GLint loc_LI = glGetUniformLocation(shprg, "lightIntensity");
	//glUniform1f(loc_LI, light->intensity);
	//GLint loc_LA = glGetUniformLocation(shprg, "lightAttenuation");
	//glUniform1f(loc_LA, light->attenuation);
	//GLint loc_LAM = glGetUniformLocation(shprg, "lightAmbient");
	//glUniform1f(loc_LAM, light->ambient);


	// Select current resources 
	glBindVertexArray(mesh->vao);

	// To accomplish wireframe rendering (can be removed to get filled triangles)
	if (player->shaderMode == 1) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Draw all triangles
	glDrawElements(GL_TRIANGLES, mesh->nt * 3, GL_UNSIGNED_INT, NULL);
	glBindVertexArray(0);
}