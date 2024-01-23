/* Filename: planet.h */

#ifndef PLANET_HEADER
#define PLANET_HEADER

#include <iostream>
#include <model.h>

/* Structure that represents a point in the 3D world */
typedef struct Point3D {
	double x, y, z;
} Point3D;

/* Class that represents a 3D Planet Model */
class Planet : public Model {
private:
	Point3D coords, orientation; // Planet Coordinates and orientation
	Planet* orbitPlanet;		 // Ôhe planet on which that planet orbits 

	double distanceFromOrbit;			  // The radious distance of its orbit planet
	double velocity, spinningVelocity;    // The plant's velocity around its orbit planet, and its spinning velocity
	double stepsCounter, spinningCounter; // Vaiables to keep track of the position and rotation of the planet

	double scaleFactor; // The planet's scale

	glm::mat4 positionTranformation{}; // Supporting matrix to perform transformations to the planet

	void fixOrientation(glm::mat4& transformation);

public:
	static bool simulationPaused; // Supporting variable that determines whether the user has paused the simulation

	Planet(const std::string& path, const double distanceFromParent, const double velocity, const double spinningVelocity, const double scaleFactor, Planet* parentPlanet);
	Planet(void): Model() {}

	void inline setStartPositionOffset(const double value);
	void inline setOrientation(const double xOrient, const double yOrient, const double zOrient);
	void inline move(const double xFactor, const double yFactor, const double zFactor);

	void updatePosition(void);
	void draw(Shader& shader);
};

/* Planet's Constructor */
Planet::Planet(const std::string& path, const double distanceFromParent, const double velocity, const double spinningVelocity, const double scaleFactor, Planet* parentPlanet):
	Model(path)
{
	// Setting the coordinates of the planet and its orientation x,y,z factors
	this->coords = this->orientation = { 0, 0, 0 };

	// Setting every characteristic of the planet
	this->orbitPlanet = parentPlanet;
	this->distanceFromOrbit = distanceFromParent;

	this->velocity = velocity; this->spinningVelocity = spinningVelocity;
	this->scaleFactor = scaleFactor;

	// Setting the usefull variables for the planet's location and rotation
	this->stepsCounter = this->spinningCounter = 0;
}

/* Updates the position of the planet in the 3D world */
void Planet::updatePosition(void)
{
	Point3D currCoords = { this->coords.x, this->coords.y, this->coords.z };
	glm::mat4 transformation = glm::mat4(1.0f);
	double theta;

	if (velocity != 0)
		theta = this->stepsCounter * velocity;
	else
		theta = this->stepsCounter;

	transformation = glm::translate(transformation, glm::vec3(0, 0, 0));

	// if the application is not paused then calculate the new coordinates for the planet according to its rotation around its orbit planet
	if (!this->simulationPaused) {
		this->spinningCounter += this->spinningVelocity;

		if (this->orbitPlanet != NULL) {
			this->coords.x = this->distanceFromOrbit * cos(theta);
			this->coords.z = this->distanceFromOrbit * sin(theta);
			this->stepsCounter += this->velocity;
	}}

	// Update the current 3D point by assigning to it the correct position values of the planet taking care of every progenitor of that planet
	Planet* progenitorPlanet = this->orbitPlanet;
	while (progenitorPlanet != NULL) {
		currCoords.x += progenitorPlanet->coords.x;
		currCoords.y += progenitorPlanet->coords.y;
		currCoords.z += progenitorPlanet->coords.z;

		progenitorPlanet = progenitorPlanet->orbitPlanet;
	}

	// Perform a transformation to the planet, placing him at the right spot
	transformation = glm::translate(transformation, glm::vec3(currCoords.x, currCoords.y, currCoords.z));
	transformation = glm::scale(transformation, glm::vec3(this->scaleFactor, -this->scaleFactor, this->scaleFactor));
	transformation = glm::rotate(transformation, (float)this->spinningCounter, glm::vec3(0.0f, 1.0f, 0.0f));

	// Fix the planets orientation
	this->fixOrientation(transformation);

	this->positionTranformation = transformation; // Assign the new transformation to the planet's matrix transformation variable
}

/* Sets an offset at the starting spaw position of the planet */
void Planet::setStartPositionOffset(const double value) { 
	this->stepsCounter = value; 
}

/* Sets the orientation of the planet */
void Planet::setOrientation(const double xOrient, const double yOrient, const double zOrient) {
	this->orientation = { xOrient, yOrient, zOrient };
}

/* Moves the planet by x,y,z factors */
void Planet::move(const double xFactor, const double yFactor, const double zFactor)
{
	this->coords.x += xFactor;
	this->coords.y += yFactor;
	this->coords.z += zFactor;
}

/* Spawns the planet at the right point in the 3D scene */
void Planet::draw(Shader& shader) {
	shader.setMat4("model", this->positionTranformation);
	this->Draw(shader);
}

/* Fixes the planet's orientation */
void Planet::fixOrientation(glm::mat4& transformation)
{
	transformation = glm::rotate(transformation, (float)this->orientation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	transformation = glm::rotate(transformation, (float)this->orientation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	transformation = glm::rotate(transformation, (float)this->orientation.z, glm::vec3(0.0f, 0.0f, 1.0f));
}

bool Planet::simulationPaused = false;

#endif /* PLANET_HEADER */
