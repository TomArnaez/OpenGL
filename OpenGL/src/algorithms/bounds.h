#ifndef BOUND_H
#define BOUND_H

#include <glm/glm.hpp>

enum class BoundTypes {
	AABB, // Axis-aligned bounding box
	SPHERE
};
class BoundingRegion {
public:
	BoundTypes type;

	// sphere values
	glm::vec3 centre;
	float radius;

	// bounding box values
	glm::vec3 min;
	glm::vec3 max;

	BoundingRegion(BoundTypes type);

	// initalize as pshere
	BoundingRegion(glm::vec3 centre, float radius);

	// initalize as AABB
	BoundingRegion(glm::vec3 min, glm::vec3 max);

	/*
		Calculating values for the region
	*/

	glm::vec3 calculateCentre();

	glm::vec3 calculateDimensions();

	/*
		testing methods
	*/

	bool containsPoint(glm::vec3 pt);

	bool containsRegion(BoundingRegion br);
	
	// determine if region interescts (partial containment)
	bool intersectsWith(BoundingRegion br);
};

#endif