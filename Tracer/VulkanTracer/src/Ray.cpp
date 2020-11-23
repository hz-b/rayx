#include "Ray.h"

void Ray::initRay(double xpos, double ypos, double zpos, double xdir, double ydir, double zdir) {
	position.x = xpos;
	position.y = ypos;
	position.z = zpos;
	direction.x = xdir;
	direction.y = ydir;
	direction.z = zdir;
}

void Ray::initDisk(double xpos, double ypos, double zpos, double xdir, double ydir, double zdir, double radius) {
	target.origin.x = xpos;
	target.origin.y = ypos;
	target.origin.z = zpos;
	target.normal.x = xdir;
	target.normal.y = ydir;
	target.normal.z = zdir;
	target.radius = radius;
}

int Ray::traceRay() {
	try {
		vec3 a;
		a.x = target.origin.x - position.x;
		a.y = target.origin.y - position.y;
		a.z = target.origin.z - position.z;
		double t = dotProduct(a, target.normal) / dotProduct(direction, target.normal);
		hitplane = (t < target.radius);
	}
	catch (const std::exception& e) {
		std::cerr << "Error in traceRay occured" << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
bool Ray::hitPlane() {
	return hitplane;
}

std::vector<double> Ray::getRayInformation()
{
	std::vector<double> rayInfo;
	rayInfo.push_back(position.x);
	rayInfo.push_back(position.y);
	rayInfo.push_back(position.z);
	rayInfo.push_back(direction.x);
	rayInfo.push_back(direction.y);
	rayInfo.push_back(direction.z);
	return rayInfo;
}

double Ray::getxDir()
{
	return direction.x;

}double Ray::getyDir()
{
	return direction.y;
}

double Ray::getzDir()
{
	return direction.z;
}

double Ray::getxPos()
{
	return position.x;
}

double Ray::getyPos()
{
	return position.y;
}

double Ray::getzPos()
{
	return position.z;
}



void Ray::normalizeVector() {

}

double Ray::dotProduct(vec3 a, vec3 b) {
	double result = a.x * b.x + a.y * b.y + a.z * b.z;
	return result;
}
