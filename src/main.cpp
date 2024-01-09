#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

void processInput(GLFWwindow* window); // Processes the input
void frameBuffer_size_callback(GLFWwindow* window, int width, int height); // Resets the viewport

int main(int argc, char* argv[])
{
	glfwInit(); // Initialize GLFW

	// Use OpenGL Version 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	// Initializing the appliacation window
	GLFWwindow* window = glfwCreateWindow(800, 600, "Graphics Assignment: Planet Simulation", NULL, NULL);
	if (window == NULL) {
		std::cout << "Could not create window." << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window); // Set the current window
	
	// Initializing GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		return -1;
	}
	
	// Setting up the viewport
	glViewport(0, 0, 800, 600);
	glfwSetFramebufferSizeCallback(window, frameBuffer_size_callback);

	// Main Application Loop
	while (!glfwWindowShouldClose(window)) {
		processInput(window); // Process the input
		
		// Render some color
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(window); // Send new frame to the window
		glfwPollEvents();
	}

	glfwTerminate(); // Terminate GLFW
	return 0;
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void frameBuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}
