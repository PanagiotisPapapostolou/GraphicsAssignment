#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader.h>
#include <camera.h>
#include <model.h>
#include <math.h>
#include <iostream>
#include <thread>
#include <chrono>

typedef struct EnvironmentColors {
    float red;
    float green;
    float blue;
    float alpha;

} EnvironmentColors;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

/* Settings */
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 1000;

/* Camera */
Camera camera(glm::vec3(0.0f, 0.0f, 10.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

/* Timimg */
float deltaTime = 0.0f;
float lastFrame = 0.0f;

/* Environment Options */
const double earthRadius = 20;
const double moonRadius = 2;
const double sunVelocity = 0.0f;
const double earthVelocity = 0.05f;
const double earthSpinningVelocity = 0.02;
const double moonVelocity = 0.2f;

EnvironmentColors envColor = { 0.0f, 0.0f, 0.0f, 1.0f };

struct Point { double x, y, z; };

bool paused = false;

// Lighting
glm::vec3 lightPos(0.0f, 0.0f, 0.0f);

int main(int argc, char* argv[])
{
	/* GLFW: Initialization and Configuration */
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	/* GLFW Window Creation */
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Tell GLFW to capture the user's mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    /* GLAD: Loading all OpenGL function pointers */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    // Configure global OpenGL State
    glEnable(GL_DEPTH_TEST);

    // Build and Compile the application shaders
    Shader lightShader("src/vertex_core.glsl", "src/fragment_core.glsl");
    Shader lightSourceShader("src/vertex_core_light_source.glsl", "src/fragment_core_light_source.glsl");

    // Load the models
    Model sun("Assets/sun/scene.gltf");
    Model earth("Assets/earth/Earth.obj");
    Model moon("Assets/moon/Moon.obj");
    
    Point earthCoords = { 0.0f, 0.0f, 0.0f };
    Point sunCoords = { 0.0f, 0.0f, 0.0f };
    Point moonCoords = { 0.0f, 0.0f, 0.0f };
    double earthStepsCounter = 0, earthSpinningCounter = 0;
    double moonStepsCounter = 0;
    double theta;

    /* Application Render Loop */
    while (!glfwWindowShouldClose(window)) {
        // Per-frame time logic
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Processing the input
        processInput(window);

        // Rendering
        glClearColor(envColor.red, envColor.green, envColor.blue, envColor.alpha);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Don't forget to enable shader before setting uniforms
        lightShader.use();
        lightShader.setVec3("objectColor", 1.0f, 1.0f, 1.0f);
        lightShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        lightShader.setVec3("lightPos", lightPos);

        // View/Projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        lightShader.setMat4("projection", projection);
        lightShader.setMat4("view", view);

        glm::mat4 moonModel = glm::mat4(1.0f);
        glm::mat4 earthModel = glm::mat4(1.0f);
        glm::mat4 sunModel = glm::mat4(1.0f);
        
        // Rendering the Earth
        earthModel = glm::translate(earthModel, glm::vec3(0.0f, 0.0f, 0.0f));
        theta = earthStepsCounter * earthVelocity;
        if (!paused) {
            earthCoords.x = earthRadius * cos(theta);
            earthCoords.z = earthRadius * sin(theta);
            earthStepsCounter += earthVelocity;
            earthSpinningCounter += earthSpinningVelocity;
        }
        earthModel = glm::translate(earthModel, glm::vec3(sunCoords.x + earthCoords.x, sunCoords.y + earthCoords.y, sunCoords.z + earthCoords.z));
        earthModel = glm::scale(earthModel, glm::vec3(0.1f, -0.1f, 0.1f));
        earthModel = glm::rotate(earthModel, (float)earthSpinningCounter, glm::vec3(0.0f, 1.0f, 0.0f));
        lightShader.setMat4("model", earthModel);
        earth.Draw(lightShader);

        // Rendering the Moon
        moonModel = glm::translate(moonModel, glm::vec3(moonCoords.x, moonCoords.y, moonCoords.z));
        theta = moonStepsCounter * moonVelocity;
        if (!paused) {
            moonCoords.x = moonRadius * cos(theta);
            moonCoords.z = moonRadius * sin(theta);
            moonStepsCounter += moonVelocity;
        }
        moonModel = glm::translate(moonModel, glm::vec3(sunCoords.x + moonCoords.x + earthCoords.x, sunCoords.y + moonCoords.y + earthCoords.y, sunCoords.z + moonCoords.z + earthCoords.z));
        moonModel = glm::scale(moonModel, glm::vec3(0.025f, -0.025f, 0.025f));
        lightShader.setMat4("model", moonModel);
        moon.Draw(lightShader);

        // Render Light Source
        lightSourceShader.use();
        lightSourceShader.setMat4("projection", projection);
        lightSourceShader.setMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(1.0f));
        lightSourceShader.setMat4("model", model);

        // Rendering the Sun
        if (!paused) sunCoords.x += sunVelocity;
        sunModel = glm::translate(sunModel, glm::vec3(sunCoords.x, sunCoords.y, sunCoords.z)); // Translate it down so it's at the center of the scene
        sunModel = glm::scale(sunModel, glm::vec3(10.9f, -10.9f, 10.9f)); // It's a bit too big for our scene, so scale it down
        lightSourceShader.setMat4("model", sunModel);
        sun.Draw(lightSourceShader);

        // GLFW: Swap Buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // GLFW: Terminate, clearing all previously allocated GLFW recourses
    glfwTerminate();
    return 0;
}

/* GLFW: Whenever the window size changed (by OS or user resize) this callback function executes */
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.ProcessKeyBoard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.ProcessKeyBoard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.ProcessKeyBoard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.ProcessKeyBoard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) camera.ProcessKeyBoard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) camera.ProcessKeyBoard(DOWN, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && !paused) { paused = true; std::this_thread::sleep_for(std::chrono::milliseconds(200)); }
    else if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && paused) { paused = false; std::this_thread::sleep_for(std::chrono::milliseconds(200)); }
}

/* GLFW: Whenever the window size changed (by OS or user resize) this callback function executes */
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

/* GLFW: Whenever the mouse moves, this callback is called */
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos; lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos; lastY = ypos;
    camera.ProcessMouseMovement(xoffset, yoffset);
}

/* GLFW: Whenever the mouse scroll wheel scrolls, this callback is called */
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
