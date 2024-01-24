/* Filename: main.cpp */

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader.h>
#include <camera.h>
#include <model.h>
#include <cmath>
#include <iostream>
#include <thread>
#include <chrono>

#include "planet.h"

#define getRandFloat(min,max) min+((float)rand()/RAND_MAX)*(max-min);

struct EnvironmentColors { 
    float red, green, blue, alpha; 
};

struct SphericalCoordinates {
    double r;
    double theta;
    double phi;
};

struct Rock {
    glm::mat4 transformation;
    double distanceFromSun;
    double size;
    double orientationX, orientationY, orientationZ;
    double spinningVelocityX, spinningVelocityY, spinningVelocityZ;
};

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
const double sunSize = 1.0f;

const unsigned int starsAmount = 1000;
const double starsSize = (float)(sunSize / 40);
const double starsDistanceFromSun = (float)(sunSize * 85);

const unsigned int rocksAmount = 3000;
const double rocksSize_MIN = (float)(sunSize / 600);
const double rocksSize_MAX = (float)(sunSize / 100);
const double rocksDistanceFromSun_MIN = (float)(sunSize * 2);
const double rocksDistanceFromSun_MAX = (float)(sunSize * 40);
const double rocksOrientation_MIN = 0;
const double rocksOrientation_MAX = 360;
const double rocksVelocity_MIN = (float)(sunSize / 40);
const double rocksVelocity_MAX = (float)(sunSize / 10);
const double rocksSpinningVelocity_MIN = (float)(sunSize / 10000);
const double rocksSpinningVelocity_MAX = (float)(sunSize / 1000);

const double earthSize = (float)(sunSize / 109.12144);
const double earthRadius = (float)(sunSize * 6);
const double earthVelocity = (float)(sunSize / 20);
const double earthSpinningVelocity = (float)(sunSize / 35);

const double moonSize = (float)(earthSize / 4);
const double moonRadius = (float)(earthSize * 32);
const double moonVelocity = (float)(earthSize * 20);
const double moonSpinningVelocity = 0.0f;

EnvironmentColors envColor = { 0.0f, 0.0f, 0.0f, 1.0f };

struct Point { double x, y, z; };

bool paused = false;

// Lighting
glm::vec3 lightPos(0.0f, 0.0f, 0.0f);

int main(int argc, char* argv[])
{
    srand(static_cast<unsigned int>(glfwGetTime()));

	/* GLFW: Initialization and Configuration */
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	/* GLFW Window Creation */
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "GraphicsAssignment: Planet Simluation", NULL, NULL);
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

    // Loading all the 3D planet models
    Planet sun_model("Assets/sun/scene.gltf", 0, 0, 0, sunSize, NULL); sun_model.setOrientation(90, 0, 0);
    Planet earth_model("Assets/earth/Earth.obj", earthRadius, earthVelocity, earthSpinningVelocity, earthSize, &sun_model);
    Planet moon_model("Assets/moon/Moon.obj", moonRadius, moonVelocity, moonSpinningVelocity, moonSize, &earth_model);
    Planet star_model("Assets/star/star.obj", 0, 0, 0, starsSize, &sun_model);
    Planet rock_model("Assets/Rock/rock.obj", 0, 0.1, 0, 0, &sun_model);

    /* Loading the stars backgound */
    glm::mat4* starsTransformations = new glm::mat4[starsAmount];
    for (unsigned int i = 0; i < starsAmount; i++) {
        glm::mat4 currTransformation = glm::mat4(1.0f);

        double r = starsDistanceFromSun;
        double theta = (double)(rand() % 180);
        double phi = (double)(rand() % 360);
        
        double x = r * sin(theta) * cos(phi);
        double y = r * sin(theta) * sin(phi);
        double z = r * cos(theta);

        currTransformation = glm::translate(currTransformation, glm::vec3(x, y, z));
        currTransformation = glm::scale(currTransformation, glm::vec3(starsSize, starsSize, starsSize));

        starsTransformations[i] = currTransformation;
    }
    
    /* Loading the rocks around the sun */
    Rock* rocks = new Rock[rocksAmount];
    for (unsigned int i = 0; i < rocksAmount; i++) {
        glm::mat4 currTransformation = glm::mat4(1.0);
        double theta = rand() % 360;
        double rocksElevation_MIN = -0.2, rocksElevation_MAX = 0.2;

        rocks[i].distanceFromSun = getRandFloat(rocksDistanceFromSun_MIN, rocksDistanceFromSun_MAX);
        rocks[i].size = getRandFloat(rocksSize_MIN, rocksSize_MAX);

        rocks[i].orientationX = getRandFloat(rocksOrientation_MIN, rocksOrientation_MAX);
        rocks[i].orientationY = getRandFloat(rocksOrientation_MIN, rocksOrientation_MAX);
        rocks[i].orientationZ = getRandFloat(rocksOrientation_MIN, rocksOrientation_MAX);

        rocks[i].spinningVelocityX = getRandFloat(rocksSpinningVelocity_MIN, rocksSpinningVelocity_MAX);
        rocks[i].spinningVelocityY = getRandFloat(rocksSpinningVelocity_MIN, rocksSpinningVelocity_MAX);
        rocks[i].spinningVelocityZ = getRandFloat(rocksSpinningVelocity_MIN, rocksSpinningVelocity_MAX);

        double x = rocks[i].distanceFromSun * cos(theta);
        double z = rocks[i].distanceFromSun * sin(theta);
        double y = rocksElevation_MIN + ((float)rand() / RAND_MAX) * (rocksElevation_MAX - rocksElevation_MIN);

        currTransformation = glm::translate(currTransformation, glm::vec3(x, y, z));
        currTransformation = glm::scale(currTransformation, glm::vec3(rocks[i].size, rocks[i].size, rocks[i].size));
        currTransformation = glm::rotate(currTransformation, (float)rocks[i].orientationX, glm::vec3(1.0f, 0.0f, 0.0f));
        currTransformation = glm::rotate(currTransformation, (float)rocks[i].orientationY, glm::vec3(0.0f, 1.0f, 0.0f));
        currTransformation = glm::rotate(currTransformation, (float)rocks[i].orientationZ, glm::vec3(0.0f, 0.0f, 1.0f));

        rocks[i].transformation = currTransformation;
    }

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
        lightShader.setVec3("viewPos", camera.Position);

        // View/Projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        lightShader.setMat4("projection", projection);
        lightShader.setMat4("view", view);

        // Rendering the Earth
        earth_model.updatePosition();
        earth_model.draw(lightShader);

        // Rendering the Moon
        moon_model.updatePosition();
        moon_model.draw(lightShader);

        // Rendering the rocks around the sun
        for (unsigned int i = 0; i < rocksAmount; i++) {
            rocks[i].transformation = glm::rotate(rocks[i].transformation, (float)(glfwGetTime() * rocks[i].spinningVelocityX), glm::vec3(1.0f, 0.0f, 0.0f));
            rocks[i].transformation = glm::rotate(rocks[i].transformation, (float)(glfwGetTime() * rocks[i].spinningVelocityY), glm::vec3(0.0f, 1.0f, 0.0f));
            rocks[i].transformation = glm::rotate(rocks[i].transformation, (float)(glfwGetTime() * rocks[i].spinningVelocityZ), glm::vec3(0.0f, 0.0f, 1.0f));

            rock_model.positionTranformation = rocks[i].transformation;
            rock_model.draw(lightShader);
        }
        
        // Render Light Source
        lightSourceShader.use();
        lightSourceShader.setMat4("projection", projection);
        lightSourceShader.setMat4("view", view);

        // Rendering the sun
        sun_model.updatePosition();
        sun_model.draw(lightSourceShader);

        // Rendering the stars
        for (unsigned int i = 0; i < starsAmount; i++) {
            star_model.positionTranformation = starsTransformations[i];
            star_model.draw(lightSourceShader);
        }

        // GLFW: Swap Buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    delete[] starsTransformations;
    delete[] rocks;

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

    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && !Planet::simulationPaused) { Planet::simulationPaused = true; std::this_thread::sleep_for(std::chrono::milliseconds(200)); }
    else if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && Planet::simulationPaused) { Planet::simulationPaused = false; std::this_thread::sleep_for(std::chrono::milliseconds(200)); }
    
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS && camera.MovementSpeed == SPEED) { camera.MovementSpeed = SLOWER_SPEED; std::this_thread::sleep_for(std::chrono::milliseconds(100)); }
    else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS && camera.MovementSpeed == SLOWER_SPEED) { camera.MovementSpeed = SPEED; std::this_thread::sleep_for(std::chrono::milliseconds(100)); }
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
