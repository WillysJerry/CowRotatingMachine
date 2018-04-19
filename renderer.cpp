#include "renderer.h"
#include "algebra.h"
#include "material.h"

Renderer* activeRenderer;

Renderer initRenderer(Camera* cam, int screen_width, int screen_height, Mesh* meshList, PointLight* light, GLuint shprg) {
	Renderer r = {
		cam,
		screen_width,
		screen_height,
		meshList,
		light,
		shprg
	};

	return r;
}

void setActiveRenderer(Renderer* renderer) {
	activeRenderer = renderer;
}

void renderMesh(Mesh *mesh, Matrix V, Matrix P, Matrix PV) {

	static Matrix W, M;

	GLuint shprg = activeRenderer->shprg;
	PointLight light = *(activeRenderer->light);
	Camera cam = *(activeRenderer->cam);

	// Assignment 1: Apply the transforms from local mesh coordinates to world coordinates here
	// Combine it with the viewing transform that is passed to the shader below
	Vector radRotation = { Deg2Rad(mesh->rotation.x), Deg2Rad(mesh->rotation.y), Deg2Rad(mesh->rotation.z) };
	
	W = LocalToWorld(mesh->translation, radRotation, mesh->scale);
	M = MatMatMul(PV, W);

	// TODO: REPLACE WITH DIFFERENT MATERIALS PER MESH ?
	Material mat = {
		{ 0.1f, 0.1f, 0.1f },	// Ambient
		{ 0.7f, 0.1f, 0.1f },	// Diffuse
		{ 1.0f, 1.0f, 1.0f },	// Specular
		0.8f					// Shininess
	};


	// Pass the viewing transform to the shader
	//GLint loc_PV = glGetUniformLocation(shprg, "PV");
	//glUniformMatrix4fv(loc_PV, 1, GL_FALSE, PV.e);
	GLint loc_Mod = glGetUniformLocation(shprg, "model");
	glUniformMatrix4fv(loc_Mod, 1, GL_FALSE, W.e);
	GLint loc_Vie = glGetUniformLocation(shprg, "view");
	glUniformMatrix4fv(loc_Vie, 1, GL_FALSE, V.e);
	GLint loc_Pro = glGetUniformLocation(shprg, "projection");
	glUniformMatrix4fv(loc_Pro, 1, GL_FALSE, P.e);


	GLint loc_MA = glGetUniformLocation(shprg, "material.ambient");
	glUniform3f(loc_MA, mat.ambient.x, mat.ambient.y, mat.ambient.z);
	GLint loc_MD = glGetUniformLocation(shprg, "material.diffuse");
	glUniform3f(loc_MD, mat.diffuse.x, mat.diffuse.y, mat.diffuse.z);
	GLint loc_MS = glGetUniformLocation(shprg, "material.specular");
	glUniform3f(loc_MS, mat.specular.x, mat.specular.y, mat.specular.z);
	GLint loc_MSH = glGetUniformLocation(shprg, "material.shininess");
	glUniform1f(loc_MSH, mat.shininess);
	GLint loc_LC = glGetUniformLocation(shprg, "light.color");
	glUniform3f(loc_LC, light.color.x, light.color.y, light.color.z);
	GLint loc_LP = glGetUniformLocation(shprg, "light.pos");
	glUniform3f(loc_LP, light.pos.x, light.pos.y, light.pos.z);
	GLint loc_VP = glGetUniformLocation(shprg, "viewPos");
	glUniform3f(loc_VP, cam.position.x, cam.position.y, cam.position.z);
	GLint loc_LI = glGetUniformLocation(shprg, "light.intensity");
	glUniform1f(loc_LI, light.intensity);
	GLint loc_LA = glGetUniformLocation(shprg, "light.attenuation");
	glUniform1f(loc_LA, light.attenuation);
	GLint loc_LAM = glGetUniformLocation(shprg, "light.ambient");
	glUniform1f(loc_LAM, light.ambient);
	GLint loc_LST = glGetUniformLocation(shprg, "light.specularStrength");
	glUniform1f(loc_LST, 80.0f);


	// Select current resources 
	glBindVertexArray(mesh->vao);

	// To accomplish wireframe rendering (can be removed to get filled triangles)
	if (activeRenderer->shaderMode == 1) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Draw all triangles
	glDrawElements(GL_TRIANGLES, mesh->nt * 3, GL_UNSIGNED_INT, NULL);
	glBindVertexArray(0);
}


void display(void) {
	static Matrix V, P, PV;
	Mesh *mesh;
	Camera cam = *(activeRenderer->cam);

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);


	float a = Deg2Rad(cam.rotation.x), b = Deg2Rad(cam.rotation.y), c = Deg2Rad(cam.rotation.z);
	Vector pos = { -cam.position.x, -cam.position.y, -cam.position.z };

	Matrix rz = RotateZ(-c);
	Matrix ry = RotateY(-b);
	Matrix rx = RotateX(-a);
	Matrix t = Translate(pos.x, pos.y, pos.z);


	if (activeRenderer->viewMode == 0) {
		V = MatMatMul(t, MatMatMul(rx, MatMatMul(rz, ry)));
	}
	else {
		// Free look
		Matrix a = MatMatMul(ry, rx);	// Pls note we only rotate around the x- and y-axes when we determine the gaze direction

		Vector eye = cam.position;											// the cam position
		Vector gaze = Homogenize(MatVecMul(a, { 0, 0, -1 }));				// Gaze is any direction the viewer looks at
																			//Vector up = { 0, 1, 0 };											// Screen up is always up
		Vector up = Normalize(Homogenize(MatVecMul(rz, { 0, 1, 0 })));	// Direction pointing up from the viewer

		V = MatLookAt(eye, gaze, up);		// Then we do this <- over there on the left
	}

	// Assignment 1: Calculate the projection transform yourself 	
	// The matrix P should be calculated from camera parameters
	// Therefore, you need to replace this hard-coded transform. 	

	if (activeRenderer->projMode == 0) {
		P = MatOrtho(-10, 10, -10, 10, 0, 100000);
	}
	else if (activeRenderer->projMode == 1) {
		P = MatPerspective(Deg2Rad(cam.fov), activeRenderer->screen_width / activeRenderer->screen_height, 1, 100000);
	}
	else {
		P = MatFrustum(-1, 1, -1, 1, 1, 100000);

	}

	// This finds the combined view-projection matrix
	PV = MatMatMul(P, V);

	// Select the shader program to be used during rendering 
	glUseProgram(activeRenderer->shprg);

	// Render all meshes in the scene
	mesh = activeRenderer->meshList;

	while (mesh != NULL) {
		renderMesh(mesh, V, P, PV);
		mesh = mesh->next;
	}

	glFlush();
}
