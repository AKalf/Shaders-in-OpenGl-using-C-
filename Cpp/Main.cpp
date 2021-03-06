
#include "stdafx.h"
#include "Main.h"

int windowWidth;
int windowHeight;

GLFWwindow* window;



int main(void)
{
	std::cout << "Write the path for an .OBJ file or type 1 to load spaceship. Be sure to include .obj at the end." << '\n';
	string reply;
	string path;
	std::cin >> reply;
	if (reply != "1") {
		path = reply;
	}
	else
	{
		path = "ship/ship.obj";
	}
	
	if (MyOGLInitialize() != true) {
		return -1;
	}

	// Shaders 
	MyShader shipShader("MyShaders/ShipVertexShader.vertexshader", "MyShaders/ShipFragmentShader.fragmentshader", "ShipShader");
	
// Read our .obj file
	
	
	Model shipModel(path, false, shipShader.programID);
	shipModel.position = glm::vec3(-0.5f, -0.3f, -20.0f);
	// Create our camera
	MyCamera myCamera(glm::vec3(0, 0, 0));	

	// THE Render loop
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0) 
	{	
		
		UpdateDeltaTime();
		
		// clear the previous frame
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// Calculate model position
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, shipModel.position);
		model = glm::rotate(model, (float)glfwGetTime() * glm::radians(55.0f), glm::vec3(0., 0.1f, 0.1f));

		// Calculate view
		glm::mat4 view = glm::mat4(1.0f);
		view = glm::translate(view, myCamera.GetCameraPosition());

		// Calculate camera perspective
		glm::mat4 perspective = glm::mat4(1.0f);
		perspective = glm::perspective(glm::radians(50.0f), (float)(800 / 600), 0.1f, 100.0f);
		glm::mat4 MVP = perspective * view * model;

		// use our ship shader program
		glUseProgram(shipShader.programID);

		/*
		* Update uniform values in our shader
		*/
		int uniformPosition = glGetUniformLocation(shipShader.programID, "Model");
		glUniformMatrix4fv(uniformPosition, 1, GL_FALSE, glm::value_ptr(model));

		uniformPosition = glGetUniformLocation(shipShader.programID, "View");
		glUniformMatrix4fv(uniformPosition, 1, GL_FALSE, glm::value_ptr(view));

		uniformPosition = glGetUniformLocation(shipShader.programID, "Projection");
		glUniformMatrix4fv(uniformPosition, 1, GL_FALSE, glm::value_ptr(perspective));

		uniformPosition = glGetUniformLocation(shipShader.programID, "LightPos");
		glUniform3f(uniformPosition, 0.5f, 0.2f, -2.5f);

		uniformPosition = glGetUniformLocation(shipShader.programID, "Time");
		glUniform1f(uniformPosition, (float)glfwGetTime());

		shipModel.Draw();

		glfwSwapBuffers(window); 
		glfwPollEvents();
		
	}
	glDeleteProgram(shipShader.programID);
	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}





bool MyOGLInitialize() {

	std::cout << "Starting program..." << '\n';

	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return false;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(1920, 1080, "Tutorial 0 - Keyboard and Mouse", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return false;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	// Hide the mouse and enable unlimited mouvement
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set the mouse at the center of the screen
	glfwPollEvents();
	glfwSetCursorPos(window, 1024 / 2, 768 / 2);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	GLfloat ambientLight[] = { 0.2F, 0.2F, 0.2F, 1.0F };
	GLfloat diffuseLight[] = { 0.2F, 0.2F, 0.2F, 1.0F };
	GLfloat speculaLight[] = { 0.5F, 0.5F, 0.5F, 1.0F };
	GLfloat position[] = { -1.2F, 1.F, -4.F, 1.0F };

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, speculaLight);
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);
	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);
	return true;
}
