#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <shader.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

static void processInput(GLFWwindow* window);
static void frameBuffer_size_callback(GLFWwindow* window, const int width, const int height);

const unsigned int WINDOW_WIDTH = 1200; // Window width
const unsigned int WINDOW_HEIGHT = 900; // Window height

int main(int argc, char* argv[])
{
	/* GLFW: Initialize and Condigure */
	// ------------------------------------------------------------------------- //
	glfwInit(); // Initialize GLFW

	// Use OpenGL Version 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Initializing the appliacation window
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Graphics Assignment: Planets Simulation", NULL, NULL);
	if (window == NULL) {
		std::cout << "Could not create window." << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window); // Set the current window
	
	/* GLAD: Load all OpenGL function pointer */
	// ------------------------------------------------------------------------- //
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		return -1;
	}

	// Setting up the viewport
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glfwSetFramebufferSizeCallback(window, frameBuffer_size_callback);

	// Build and Compile our Shader Program
	// ------------------------------------------------------------------------- //
	Shader ourShader("src/vertex_core.glsl", "src/fragment_core.glsl");

	/* Set up vertex data (and buffers) and configure vertex attributes */
	// ------------------------------------------------------------------------- //
	float vertices[] = {
		// positions          // colors           // texture coords
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
	};
	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	unsigned int VBO, VAO, EBO; 
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// Bind VAO
	glBindVertexArray(VAO);

	// Bind VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Bind EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// Texture Initialization
	// ------------------------------------------------------------------------- //
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load("Assets/container.jpg", &width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
	ourShader.setMat4("projection", projection);

	/* Main Application Render Loop */
	// ------------------------------------------------------------------------- //
	while (!glfwWindowShouldClose(window)) {
		processInput(window); // Process the input

		// Render some color
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Bind the texture
		glActiveTexture(GL_TEXTURE0); // Activate the texture unit first before binding texture
		glBindTexture(GL_TEXTURE_2D, texture);

		ourShader.use();

		// Camera View Transformation
		glm::mat4 view = glm::mat4(1.0f);
		float radious = 10.0f;
		float camX = static_cast<float>(sin(glfwGetTime()) * radious);
		float camZ = static_cast<float>(cos(glfwGetTime()) * radious);
		view = glm::lookAt(glm::vec3(camX, 0.0f, camZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		// Render container
		glBindVertexArray(VAO);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.5, -0.5, 0.0f));
		float angle = 20.0f;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
		ourShader.setMat4("model", model);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// Send new frame to the window, by swapping buffers and poll IO events
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	// Deallocate all recources and terminate the GLFW
	// ------------------------------------------------------------------------- //
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteProgram(ourShader.shaderProgramID);

	glfwTerminate(); // Terminate GLFW
	return 0;
}

/* Processes the input */
static void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

/* Resets the viewport */
static void frameBuffer_size_callback(GLFWwindow* window, const int width, const int height) {
	glViewport(0, 0, width, height);
}
