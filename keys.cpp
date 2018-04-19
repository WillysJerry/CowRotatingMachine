#include "keys.h"

void keypress(unsigned char key, int x, int y) {
	Camera *cam = player->cam;

	switch (key) {
		// Camera controls
	case 'Q': // Rotate camera counter-clockwise
	case 'q':
		cam->rotation.y -= 1.0f;
		break;
	case 'E': // Rotate camera clockwise
	case 'e':
		cam->rotation.y += 1.0f;
		break;
	case 'W': // Move camera forward
		cam->rotation.x += 1.0f;
		break;
	case 'w':
		cam->position.z += 0.2f;
		break;
	case 'A': // Move camera to the left
		cam->rotation.z -= 1.0f;
		break;
	case 'a':
		cam->position.x -= 0.2f;
		break;
	case 'S': // Move camera backwards
		cam->rotation.x -= 1.0f;
		break;
	case 's':
		cam->position.z -= 0.2f;
		break;
	case 'D': // Move camera to the right
		cam->rotation.z += 1.0;
		break;
	case 'd':
		cam->position.x += 0.2f;
		break;
	case 'Z': // Move camera upwards
	case 'z':
		cam->position.y -= 0.2f;
		break;
	case 'X': // Move camera downwards
	case 'x':
		cam->position.y += 0.2f;
		break;

		// Mesh controlls
	case 'U': // Rotate Mesh counter-clockwise
	case 'u':
		activeMesh->rotation.y -= 1.0f;
		break;
	case 'O': // Rotate Mesh clockwise
	case 'o':
		activeMesh->rotation.y += 1.0f;
		break;
	case 'I': // Move Mesh forward
	case 'i':
		activeMesh->translation.z -= 0.2f;
		break;
	case 'J': // Move Mesh to the left
	case 'j':
		activeMesh->translation.x -= 0.2f;
		break;
	case 'K': // Move Mesh backwards
	case 'k':
		activeMesh->translation.z += 0.2f;
		break;
	case 'L': // Move Mesh to the right
	case 'l':
		activeMesh->translation.x += 0.2f;
		break;
	case 'T': // Move Mesh backwards
	case 't':
		activeMesh->scale.x += 0.2f;
		activeMesh->scale.y += 0.2f;
		activeMesh->scale.z += 0.2f;
		break;
	case 'Y': // Move Mesh backwards
	case 'y':
		activeMesh->scale.x -= 0.2f;
		activeMesh->scale.y -= 0.2f;
		activeMesh->scale.z -= 0.2f;
		break;
	case '\t': // Toggle between meshes in meshList
		if (activeMesh->next == NULL) {
			activeMesh = scene->meshes;
		}
		else {
			activeMesh = activeMesh->next;
		}
		break;

	// Misc options
	case 'V': // Toggle camera modes (free-look and not free-look)
	case 'v':
		player->viewMode = (player->viewMode + 1) % 2;
		break;
	case '0': // Toggle between orthographic- and perpective projection and frustum projection
		player->projMode = (player->projMode + 1) % 3;
		break;
	//case '8': // Toggle between bounce and static
	//	bounceMode = (bounceMode + 1) % 2;
	//	break;
	case '9': // Disable faces
		player->shaderMode = (player->shaderMode + 1) % 2;
		break;
	case '�': // Quit
		glutLeaveMainLoop();
		break;
	case '+':
		cam->fov += 1;
		break;
	case '-':
		cam->fov -= 1;
		break;
	}
	glutPostRedisplay(); //S�ger att vi m�ste rita om f�nstret
}