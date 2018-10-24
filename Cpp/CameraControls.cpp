#include "stdafx.h"
#include "CameraControls.h"


float cameraRotateAmount = 0.0f;

MyCamera::MyCamera(glm::vec3 cameraPos) {
	cameraPosition = cameraPos;
	horizontalAngle = 3.14f;
	verticalAngle = 0.f;
}

void MyCamera::CameraUpdate(GLFWwindow* window, std::vector<glm::mat4>* matrices) {

		// Compute new orientation - Move camera depending on mouse position
		glfwGetCursorPos(window, &xpos, &ypos);
		glfwGetWindowSize(window, &currentWidth, &currentHeight);
		if (!(xpos == (currentWidth / 2) && ypos == (currentHeight / 2))) {
			horizontalAngle += mouseSpeed * GetMyDeltaTime() * float(currentWidth / 2 - xpos);
			verticalAngle += mouseSpeed * GetMyDeltaTime() * float(currentHeight / 2 - ypos);
		}
		
}

void MyCamera::MoveCamera(GLFWwindow* window, glm::vec3 cameraDirection, glm::vec3 right) {
	
	// Move forward
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		cameraPosition += cameraDirection * GetMyDeltaTime() * speed;
	}
	// Move backward
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		cameraPosition -= cameraDirection * GetMyDeltaTime() * speed;

	}
	// Strafe right
	
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		cameraPosition += right * GetMyDeltaTime() * speed;

		// Strafe left
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			cameraPosition -= right * GetMyDeltaTime() * speed;
		}

	}
	
}


glm::vec3 MyCamera::GetCameraPosition() {
	return cameraPosition;
}



