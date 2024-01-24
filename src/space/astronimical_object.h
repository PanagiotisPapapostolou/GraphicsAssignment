/* Filename: astronomical_object.h */

#ifndef SPACE_OBJECT_HEADER
#define SPACE_OBJECT_HEADER
	
#include <model.h>

/* Structure that represents a point in the 3D world */
typedef struct Point3D {
	double x, y, z;
} Point3D;

/* Class that represents a 3D Astronomical Object Model */
class AstronomicalObject {
protected:
	Model model3D;		// The 3D model the Astronomical Object is based on
	double scaleFactor; // The Astronomical Object's scale

	Point3D coords, orientation; // Astronomical Object Coordinates and orientation
	AstronomicalObject* orbitObject;   // The Astronomical Object on which that Astronomical Object orbits 

	double distanceFromOrbit;			  // The radious distance of its orbit Astronomical Object
	double velocity, spinningVelocity;    // The plant's velocity around its orbit Astronomical Object, and its spinning velocity
	double stepsCounter, spinningCounter; // Vaiables to keep track of the position and rotation of the Astronomical Object

	bool fullSpin; // Flag to determine whether the object has to do a full spin (all axis)

	glm::mat4 positionTranformation{}; // Supporting matrix to perform transformations to the Astronomical Object

	void fixOrientation(glm::mat4& transformation);

public:
	AstronomicalObject(const Model& model3D, const double distanceFromParent, const double velocity, const double spinningVelocity, const double scaleFactor, AstronomicalObject* orbitObject);
	AstronomicalObject(void) {}

	void inline setStartPositionOffset(const double value);
	void inline setOrientation(const double xOrient, const double yOrient, const double zOrient);
	void inline setFullSpin(const bool value) { this->fullSpin = value; }

	void inline setLocationX(const double x) { this->coords.x = x; }
	void inline setLocationY(const double y) { this->coords.y = y; }
	void inline setLocationZ(const double z) { this->coords.z = z; }
	
	void inline move(const double xFactor, const double yFactor, const double zFactor);

	void updatePosition(void);
	void draw(Shader& shader);
	
	static bool simulationPaused; // Supporting variable that determines whether the user has paused the simulation

	friend class BackgroundStar;
};

/* Astronomical Object's Constructor */
AstronomicalObject::AstronomicalObject(const Model& model3D, const double distanceFromParent, const double velocity, const double spinningVelocity, const double scaleFactor, AstronomicalObject* orbitObject)
{
	// Setting the coordinates of the Astronomical Object and its orientation x,y,z factors
	this->coords = this->orientation = { 0, 0, 0 };

	// Setting every characteristic of the Astronomical Object
	this->orbitObject = orbitObject;
	this->distanceFromOrbit = distanceFromParent;

	this->velocity = velocity; this->spinningVelocity = spinningVelocity;
	this->scaleFactor = scaleFactor;

	this->model3D = model3D;
	this->fullSpin = false;

	// Setting the usefull variables for the Astronomical Object's location and rotation
	this->stepsCounter = this->spinningCounter = 0;
}

/* Updates the position of the Astronomical Object in the 3D world */
void AstronomicalObject::updatePosition(void)
{
	glm::mat4 transformation = glm::mat4(1.0f);

	Point3D currCoords = { this->coords.x, this->coords.y, this->coords.z };
	double theta;

	if (velocity != 0) theta = this->stepsCounter * velocity;
	else theta = this->stepsCounter;

	transformation = glm::translate(transformation, glm::vec3(0, 0, 0));

	// if the application is not paused then calculate the new coordinates for the Astronomical Object according to its rotation around its orbit Astronomical Object
	if (!this->simulationPaused) {
		this->spinningCounter += this->spinningVelocity;

		if (this->orbitObject != NULL) {
			this->coords.x = this->distanceFromOrbit * cos(theta);
			this->coords.z = this->distanceFromOrbit * sin(theta);
			this->stepsCounter += this->velocity;
	}}

	// Update the current 3D point by assigning to it the correct position values of the Astronomical Object taking care of every progenitor of that Astronomical Object
	AstronomicalObject* progenitorObject = this->orbitObject;
	while (progenitorObject != NULL) {
		currCoords.x += progenitorObject->coords.x;
		currCoords.y += progenitorObject->coords.y;
		currCoords.z += progenitorObject->coords.z;

		progenitorObject = progenitorObject->orbitObject;
	}

	// Perform a transformation to the Astronomical Object, placing him at the right spot
	transformation = glm::translate(transformation, glm::vec3(currCoords.x, currCoords.y, currCoords.z));
	transformation = glm::scale(transformation, glm::vec3(this->scaleFactor, -this->scaleFactor, this->scaleFactor));
	transformation = glm::rotate(transformation, (float)this->spinningCounter, glm::vec3(0.0f, 1.0f, 0.0f));

	if (this->fullSpin) {
		transformation = glm::rotate(transformation, (float)this->spinningCounter, glm::vec3(1.0f, 0.0f, 0.0f));
		transformation = glm::rotate(transformation, (float)this->spinningCounter, glm::vec3(0.0f, 0.0f, 1.0f));
	}

	// Fix the object's orientation
	this->fixOrientation(transformation);

	this->positionTranformation = transformation; // Assign the new transformation to the Astronomical Object's matrix transformation variable
}

/* Sets an offset at the starting spaw position of the Astronomical Object */
void AstronomicalObject::setStartPositionOffset(const double value) { 
	this->stepsCounter = value; 
}

/* Sets the orientation of the Astronomical Object */
void AstronomicalObject::setOrientation(const double xOrient, const double yOrient, const double zOrient) {
	this->orientation = { xOrient, yOrient, zOrient };
}

/* Moves the Astronomical Object by x,y,z factors */
void AstronomicalObject::move(const double xFactor, const double yFactor, const double zFactor)
{
	this->coords.x += xFactor;
	this->coords.y += yFactor;
	this->coords.z += zFactor;
}

/* Spawns the Astronomical Object at the right point in the 3D scene */
void AstronomicalObject::draw(Shader& shader) {
	shader.setMat4("model", this->positionTranformation);
	this->model3D.Draw(shader);
}

/* Fixes the Astronomical Object's orientation */
void AstronomicalObject::fixOrientation(glm::mat4& transformation)
{
	transformation = glm::rotate(transformation, (float)this->orientation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	transformation = glm::rotate(transformation, (float)this->orientation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	transformation = glm::rotate(transformation, (float)this->orientation.z, glm::vec3(0.0f, 0.0f, 1.0f));
}

bool AstronomicalObject::simulationPaused = false;

#endif /* SPACE_OBJECT_HEADER */
