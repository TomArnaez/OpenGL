#include "bounds.h"

#include <algorithm>

BoundingRegion::BoundingRegion(BoundTypes type)
	: type(type) {}

BoundingRegion::BoundingRegion(glm::vec3 centre, float radius)
	:type(BoundTypes::SPHERE), centre(centre), radius(radius) {}

BoundingRegion::BoundingRegion(glm::vec3 min, glm::vec3 max)
	: type(BoundTypes::AABB), min(min), max(max) {}

glm::vec3 BoundingRegion::calculateCentre()
{
	return (type == BoundTypes::AABB) ? (min + max) / 2.0f : centre;
}

glm::vec3 BoundingRegion::calculateDimensions()
{
	return (type == BoundTypes::AABB) ? (max - min) : glm::vec3(2.0f * radius);
}

bool BoundingRegion::containsPoint(glm::vec3 pt)
{
	if (type == BoundTypes::AABB) {
		// box - point must be larger than man and smaller than max
		return (pt.x >= min.x) && (pt.x <= max.x) &&
			(pt.y >= min.y) && (pt.y <= max.y) &&
			(pt.z >= min.z) && (pt.z <= max.z);
	}
	else {
		// sphere - distance must be less than radius
		// x^2 + y^2 + z^2 >= r^2
		float distSqrd = 0.0f;
		for (int i = 0; i < 3; ++i) {
			distSqrd += pow((centre[i] - pt[i]), 2);
		}
		return distSqrd <= pow(radius, 2);
	}
}

bool BoundingRegion::containsRegion(BoundingRegion br)
{
	if (br.type == BoundTypes::AABB) {
		// if br is a box, just has to contain min and max
		return containsPoint(br.min) && containsPoint(br.max);
	}
	else if (type == BoundTypes::SPHERE && br.type == BoundTypes::SPHERE) {
		// if both spheres, combination of distance from centres and br.radius is less than radius
		return glm::length(centre - br.centre) + br.radius < radius;
	}
	else {
		// if this is a box and br is a sphere
		if (!containsPoint(br.centre)) {
			// centre is outside of the box
			return false;
		}

		// center inside the box
		/*
			for each axis(x, y, z)
			- if distance from centre of sphere to each side is smaller than the radius, return false
		*/
		for (int i = 0; i < 3; ++i) {
			if (abs(max[i] - br.centre[i]) < br.radius ||
				abs(br.centre[i] - min[i]) < br.radius) {
				return false;
			}
		}

		return true;
	}
}

bool BoundingRegion::intersectsWith(BoundingRegion br)
{
	if (type == BoundTypes::AABB && br.type == BoundTypes::AABB) {
		// both boxes

		glm::vec3 rad = calculateDimensions() / 2.0f;		// "radius" of this box
		glm::vec3 radBr = br.calculateDimensions() / 2.0f;		// "radius" of br

		glm::vec3 centre = calculateCentre();
		glm::vec3 centreBr = br.calculateCentre();

		glm::vec3 dist = abs(centre - centreBr);

		for (int i = 0; i < 3; i++) {
			if (dist[i] > rad[i] + radBr[i]) {
				// no overlap on this axis
				return false;
			}
		}

		// failed to find overlap
		return true;
	}

	else if (type == BoundTypes::SPHERE && br.type == BoundTypes::SPHERE) {
		// both spheres - distance between centres msut be less than combined radius
		return glm::length(centre - br.centre) < (radius - br.radius);
	}

	else if (type == BoundTypes::SPHERE) {
		// this is a sphere, br is a box
		float distSqrd = 0.0f;
		for (int i = 0; i < 3; i++) {
			// determine closest side
			float closestPt = std::max(br.min[i], std::min(centre[i], br.max[i]));
			// add distance
			distSqrd += (closestPt - centre[i]) * (closestPt - centre[i]);
		}

		return distSqrd < (radius* radius);
	}

	else { 
		// this is a box, br is a sphere
		// call algo for br 
		return br.intersectsWith(*this);
	}
}
  