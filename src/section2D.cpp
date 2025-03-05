#include "section2D.h"

//own
#include "settings.h"

std::vector<Ray2D> generateMissSectionRays(const RadialSection& radialSection) {
	Ray2D startRay = std::get<0>(radialSection.startRay);    
	Ray2D endRay = std::get<0>(radialSection.endRay);  
	
	std::vector<Ray2D> rays;
	rays.push_back(startRay);
	real_t startEndAngle = startRay.direction.angle_to(endRay.direction);
	if(startEndAngle < 0 || startEndAngle > Settings::arcSegmentLimit) {
		real_t fullArc = startEndAngle < 0 ? (Math_PI * 2) - abs(startEndAngle) : startEndAngle;
		std::size_t numberOfArcs = fullArc / Settings::arcSegmentLimit; 
		real_t segmentArc = fullArc / numberOfArcs;
		for(std::size_t i = 0; i < numberOfArcs; i++) {
			Vector2 rayDir = startRay.direction.rotated(segmentArc * (i + 1));
			rays.push_back(Ray2D{startRay.origin, rayDir});   
		}
	}

	rays.push_back(endRay);
	return rays;
}