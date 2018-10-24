
#ifndef __CAMERACONTROLS_H_INCLUDED__
#define __CAMERACONTROLS_H_INCLUDED__

// Include standard headers
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <string>
#include <sstream>
#include <vector>


// Include GLEW
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GLFW/glfw3.h>

// It is included in "MyTime.h"
//#include <GLFW/glfw3.h> 

// Include GLM
#include <glm/glm.hpp>
#include <glm\common.hpp>
#include <glm\simd\common.h>
#include <glm/gtx/transform.hpp> // after <glm/glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include "MyTime.h"
#include "MyDebug.h"

using namespace glm;
#define __LOAD_SHADERS_H__

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

// Initial Field of View
const float initialFoV = 75.0f;

const float speed = 3; // 3 units / second
const float mouseSpeed = 0.0005f;




class MyCamera {
	private:
		// Get mouse position
		double xpos, ypos;
		// Variables to calculate center of window
		int currentWidth, currentHeight;
		// horizontal angle : toward -Z
		float horizontalAngle;
		// vertical angle : 0, look at the horizon
		float verticalAngle;

		// position
		glm::vec3 cameraPosition;

		void MoveCamera(GLFWwindow* window, glm::vec3 cameraDirection, glm::vec3 right);

	public:
		MyCamera(glm::vec3 cameraPos);

		void CameraUpdate(GLFWwindow* window, std::vector<glm::mat4>* matrices);

		glm::vec3 GetCameraPosition();

		
	
};
#endif