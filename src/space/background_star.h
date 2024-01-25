/* Filename: background_star.h */

#ifndef BACKGROUND_STAR_HEADER
#define BACKGROUND_STAR_HEADER

#include "astronimical_object.h"

/* Subclass for the background stars. Represents only one star */
class BackgroundStar : public AstronomicalObject {
public:
	BackgroundStar(const Model& model3D, const double distanceFromParent, const double scaleFactor, AstronomicalObject* parentObject) : AstronomicalObject(model3D, distanceFromParent, 0.0f, 0.0f, scaleFactor, parentObject) {}
	BackgroundStar(void) {}

	void updatePosition(void);
};

/* Updates the position of the star in the 3D world */
void BackgroundStar::updatePosition(void)
{
	glm::mat4 transformation = glm::mat4(1.0f);

	transformation = glm::translate(transformation, glm::vec3(this->coords.x, this->coords.y, this->coords.z));
	transformation = glm::scale(transformation, glm::vec3(this->scaleFactor, -this->scaleFactor, this->scaleFactor));

	this->positionTranformation = transformation;
}

#endif /* BACKGROUND_STAR_HEADER */
