#include "stdafx.h"
#include <GLFW/glfw3.h>
#include "MyTime.h"

static double currentTime = 0;
static double lastTime = 0;
static float deltaTime = 0;

// Calculates deltaTime. Need to be called every frame
void UpdateDeltaTime() {
	lastTime = currentTime;
	currentTime = glfwGetTime();
	deltaTime = float(currentTime - lastTime);
}
float GetMyDeltaTime() {
	return deltaTime;
}
