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

#include "space/astronimical_object.h"
#include "space/background_star.h"

#define getRandFloat(min,max) min+((float)rand()/RAND_MAX)*(max-min);

struct EnvironmentColors { 
    float red, green, blue, alpha; 
};

struct SphericalCoordinates {
    double r;
    double theta;
    double phi;
};

/* Define the default Window Screen Dimensions */
unsigned int SCR_WIDTH = 1200;
unsigned int SCR_HEIGHT = 1000;

/* Creating the Camera Settings */
Camera camera(glm::vec3(0.0f, 0.0f, 10.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

/* Define some Timimg Settings */
float deltaTime = 0.0f;
float lastFrame = 0.0f;

/* Define the size of the Sun */
const double sunSize = 1.0f;

/* Define the stars' settings */
const unsigned int starsAmount    = 1000;
const double starsSize            = (float)(sunSize / 40);
const double starsDistanceFromSun = (float)(sunSize * 85);

/* Define the asteroids' settings */
const unsigned int asteroidsAmount = 100;
const double asteroidsSize_MIN             =  (float)(sunSize / 600);
const double asteroidsSize_MAX             =  (float)(sunSize / 100);
const double asteroidsDistanceFromSun_MIN  =  (float)(sunSize * 2);
const double asteroidsDistanceFromSun_MAX  =  (float)(sunSize * 40);
const double asteroidsOrientation_MIN      =  0;
const double asteroidsOrientation_MAX      =  360;
const double asteroidsVelocity_MIN         =  (float)(sunSize / 85);
const double asteroidsVelocity_MAX         =  (float)(sunSize / 40);
const double asteroidsSpinningVelocity_MIN =  (float)(sunSize / 100);
const double asteroidsSpinningVelocity_MAX =  (float)(sunSize / 10);
const double asteroidsElevation_MIN        = -(float)(sunSize / 5);
const double asteroidsElevation_MAX        =  (float)(sunSize / 5);

/* Define the venus' settings */
const double venusSize             = (float)(sunSize / 115);
const double venusRadius           = (float)(sunSize * 3);
const double venusVelocity         = (float)(sunSize / 15);
const double venusSpinningVelocity = (float)(sunSize / 3500);

/* Define the mars' settings */
const double marsSize             = (float)(sunSize / 115);
const double marsRadius           = (float)(sunSize * 8);
const double marsVelocity         = (float)(sunSize / 25);
const double marsSpinningVelocity = (float)(sunSize / 3500);

/* Define the mercury's settings */
const double mercurySize             = (float)(sunSize / 55);
const double mercuryRadius           = (float)(sunSize * 2);
const double mercuryVelocity         = (float)(sunSize / 22);
const double mercurySpinningVelocity = (float)(sunSize / 3500);

/* Define the pluto's settings */
const double plutoSize             = (float)(sunSize / 1580);
const double plutoRadius           = (float)(sunSize * 18);
const double plutoVelocity         = (float)(sunSize / 30);
const double plutoSpinningVelocity = (float)(sunSize / 3500);

/* Define the jupiter's settings */
const double jupiterSize             = (float)(sunSize / 25);
const double jupiterRadius           = (float)(sunSize * 9);
const double jupiterVelocity         = (float)(sunSize / 30);
const double jupiterSpinningVelocity = (float)(sunSize / 3500);

/* Define the saturn's settings */
const double saturnSize             = (float)(sunSize / 25);
const double saturnRadius           = (float)(sunSize * 10);
const double saturnVelocity         = (float)(sunSize / 35);
const double saturnSpinningVelocity = (float)(sunSize / 3500);

/* Define the nepture's settings */
const double neptureSize             = (float)(sunSize / 125);
const double neptureRadius           = (float)(sunSize * 15);
const double neptureVelocity         = (float)(sunSize / 55);
const double neptureSpinningVelocity = (float)(sunSize / 3500);

/* Define the uranus' settings */
const double uranusSize             = (float)(sunSize / 125);
const double uranusRadius           = (float)(sunSize * 12);
const double uranusVelocity         = (float)(sunSize / 45);
const double uranusSpinningVelocity = (float)(sunSize / 3500);

/* Define the earth's settings */
const double earthSize             = (float)(sunSize / 109.12144);
const double earthRadius           = (float)(sunSize * 6);
const double earthVelocity         = (float)(sunSize / 20);
const double earthSpinningVelocity = (float)(sunSize / 35);

/* Define the earth's moon settings */
const double moonSize             = (float)(earthSize / 4);
const double moonRadius           = (float)(earthSize * 32);
const double moonVelocity         = (float)(earthSize * 20);
const double moonSpinningVelocity = 0.0f;

/* Creating the environoment color and lighting */
EnvironmentColors envColor = { 0.0f, 0.0f, 0.0f, 1.0f };
glm::vec3 lightPos(0.0f, 0.0f, 0.0f);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

/* Main Function */
int main(int argc, char* argv[])
{
    srand(static_cast<unsigned int>(glfwGetTime()));

	/* GLFW: Initialization and Configuration */
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWmonitor* myMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(myMonitor);
    SCR_WIDTH = mode->width;
    SCR_HEIGHT = mode->height;

	/* GLFW Window Creation */
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "GraphicsAssignment: Planet Simluation", myMonitor, NULL);
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
    Shader defaultShader("src/shaders/shader.vs", "src/shaders/shader.fs");
    Shader lightShader("src/shaders/lightShader.vs", "src/shaders/lightShader.fs");

    // Loading all the 3D planet models
    Model sun_model("Assets/sun/scene.gltf");
    Model mars_model("Assets/Planets/Mars/Mars_2K.obj");
    Model mercury_model("Assets/Planets/Mercury/Mercury_1K.obj");
    Model pluto_model("Assets/Planets/Pluto/Pluto_1K.obj");
    Model venus_model("Assets/Planets/Venus/Venus_1K.obj");
    Model jupiter_model("Assets/Planets/Jupiter/jupiter.obj");
    Model saturn_model("Assets/Planets/Saturn/saturn.obj");
    Model saturn_ring_model("Assets/Planets/Saturn/ring.obj");
    Model nepture_model("Assets/Planets/Nepture/Nepture.obj");
    Model uranus_model("Assets/Planets/Uranus/Uranus.obj");
    Model earth_model("Assets/Planets/earth/Earth_2K.obj");
    Model moon_model("Assets/Planets/moon/Moon.obj");
    Model star_model("Assets/star/star.obj");
    Model rock_model("Assets/Rock/rock.obj");

    /* Creating all the planets, stars, rocks etc. */
    AstronomicalObject sun(sun_model, 0, 0, 0, sunSize, NULL); sun.setOrientation(90, 0, 0);
    AstronomicalObject earth(earth_model, earthRadius, earthVelocity, earthSpinningVelocity, earthSize, &sun); 
    AstronomicalObject venus(venus_model, venusRadius, venusVelocity, venusSpinningVelocity, venusSize, &sun); 
    AstronomicalObject mars(mars_model, marsRadius, marsVelocity, marsSpinningVelocity, marsSize, &sun); 
    AstronomicalObject mercury(mercury_model, mercuryRadius, mercuryVelocity, mercurySpinningVelocity, mercurySize, &sun);
    AstronomicalObject pluto(pluto_model, plutoRadius, plutoVelocity, plutoSpinningVelocity, plutoSize, &sun); 
    AstronomicalObject jupiter(jupiter_model, jupiterRadius, jupiterVelocity, jupiterSpinningVelocity, jupiterSize, &sun); 
    AstronomicalObject saturn(saturn_model, saturnRadius, saturnVelocity, saturnSpinningVelocity, saturnSize, &sun); 
    AstronomicalObject saturn_ring(saturn_ring_model, saturnRadius, saturnVelocity, saturnSpinningVelocity, saturnSize, &sun); 
    AstronomicalObject nepture(nepture_model, neptureRadius, neptureVelocity, neptureSpinningVelocity, neptureSize, &sun); 
    AstronomicalObject uranus(uranus_model, uranusRadius, uranusVelocity, uranusSpinningVelocity, uranusSize, &sun);
    AstronomicalObject moon(moon_model, moonRadius, moonVelocity, moonSpinningVelocity, moonSize, &earth);

    /* Setting the orientation of every planet */
    saturn.setOrientation(90.0f, 90.0f, 0.0f);
    saturn_ring.setOrientation(45.0f, 180.0f, 45.0f);
    nepture.setOrientation(0.0f, 0.0f, 0.0f);
    uranus.setOrientation(0.0f, 0.0f, 0.0f);
       
    /* Setting a random starting position for every planet */
    earth.setStartPositionOffset(rand() % 360);
    venus.setStartPositionOffset(rand() % 360);
    mars.setStartPositionOffset(rand() % 360);
    mercury.setStartPositionOffset(rand() % 360);
    pluto.setStartPositionOffset(rand() % 360);
    jupiter.setStartPositionOffset(rand() % 360);
    nepture.setStartPositionOffset(rand() % 360);
    uranus.setStartPositionOffset(rand() % 360);

    /* Creating the asteroids */
    AstronomicalObject* asteroids = new AstronomicalObject[asteroidsAmount];
    for (unsigned int i = 0; i < asteroidsAmount; i++) {
        double distance = getRandFloat(asteroidsDistanceFromSun_MIN, asteroidsDistanceFromSun_MAX);

        double elevationLevel = getRandFloat(asteroidsElevation_MIN, asteroidsElevation_MAX);

        double orientX = getRandFloat(asteroidsOrientation_MIN, asteroidsOrientation_MAX);
        double orientY = getRandFloat(asteroidsOrientation_MIN, asteroidsOrientation_MAX);
        double orientZ = getRandFloat(asteroidsOrientation_MIN, asteroidsOrientation_MAX);

        double spinningVelocity = getRandFloat(asteroidsSpinningVelocity_MIN, asteroidsSpinningVelocity_MIN);
        double velocity = getRandFloat(asteroidsVelocity_MIN, asteroidsVelocity_MAX);

        double size = getRandFloat(asteroidsSize_MIN, asteroidsSize_MAX);

        AstronomicalObject asteroid = AstronomicalObject(rock_model, distance, velocity, spinningVelocity, size, &sun);
        asteroid.setLocationY(elevationLevel);
        asteroid.setStartPositionOffset(rand() % 360);
        asteroid.setOrientation(orientX, orientY, orientZ);
        asteroid.setFullSpin(true);

        asteroids[i] = asteroid;
    }

    /* Creating the stars background */
    BackgroundStar* stars = new BackgroundStar[starsAmount];
    for (unsigned int i = 0; i < starsAmount; i++) {
        BackgroundStar star = BackgroundStar(star_model, starsDistanceFromSun, starsSize, &sun);

        double r = starsDistanceFromSun;
        double theta = (double)(rand() % 180);
        double phi = (double)(rand() % 360);

        star.setLocationX(r * sin(theta) * cos(phi));
        star.setLocationY(r * sin(theta) * sin(phi));
        star.setLocationZ(r * cos(theta));

        stars[i] = star;
    }

    /* Application Render Loop */
    while (!glfwWindowShouldClose(window)) {
        // Per-frame time logic
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Processing the input
        processInput(window);

        // Rendering the environment background
        glClearColor(envColor.red, envColor.green, envColor.blue, envColor.alpha);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Enable the default shader before setting uniforms
        defaultShader.use();
        defaultShader.setVec3("objectColor", 1.0f, 1.0f, 1.0f);
        defaultShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        defaultShader.setVec3("lightPos", lightPos);
        defaultShader.setVec3("viewPos", camera.Position);

        // View/Projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        defaultShader.setMat4("projection", projection);
        defaultShader.setMat4("view", view);

        /* Rendering every planet */
        venus.updatePosition();   venus.draw(defaultShader);   // Rendering Venus
        mars.updatePosition();    mars.draw(defaultShader);    // Rendering Mars
        mercury.updatePosition(); mercury.draw(defaultShader); // Rendering Mercury
        pluto.updatePosition();   pluto.draw(defaultShader);   // Rendering Pluto
        jupiter.updatePosition(); jupiter.draw(defaultShader); // Rendering Jupiter
        saturn.updatePosition();  saturn.draw(defaultShader);  // Rendering Saturn
        nepture.updatePosition(); nepture.draw(defaultShader); // Rendering Nepture
        uranus.updatePosition();  uranus.draw(defaultShader);  // Rendering Uranus
        earth.updatePosition();   earth.draw(defaultShader);   // Rendering the Earth
        moon.updatePosition();    moon.draw(defaultShader);    // Rendering the Moon

        // Rendering the asteroids around the sun
        for (unsigned int i = 0; i < asteroidsAmount; i++) {
            asteroids[i].updatePosition();
            asteroids[i].draw(defaultShader);
        }
        
        // Enable the light shader
        lightShader.use();
        lightShader.setMat4("projection", projection);
        lightShader.setMat4("view", view);

        /* Rendering the Sun and the Saturn Ring */
        sun.updatePosition();         sun.draw(lightShader);         // Rendering the sun
        saturn_ring.updatePosition(); saturn_ring.draw(lightShader); // Rendering Saturn Ring

        // Rendering the stars backgound
        for (unsigned int i = 0; i < starsAmount; i++) {
            stars[i].updatePosition();
            stars[i].draw(lightShader);
        }
        // GLFW: Swap Buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // Deallocate memory used
    delete[] stars;
    delete[] asteroids;

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

    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && !AstronomicalObject::simulationPaused) { 
        AstronomicalObject::simulationPaused = true; 
        std::this_thread::sleep_for(std::chrono::milliseconds(200)); 
    }
    else if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && AstronomicalObject::simulationPaused) { 
        AstronomicalObject::simulationPaused = false; 
        std::this_thread::sleep_for(std::chrono::milliseconds(200)); 
    }
    
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS && camera.MovementSpeed == SPEED) { 
        camera.MovementSpeed = SLOWER_SPEED; 
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); 
    }
    else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS && camera.MovementSpeed == SLOWER_SPEED) { 
        camera.MovementSpeed = SPEED; 
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); 
    }
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
