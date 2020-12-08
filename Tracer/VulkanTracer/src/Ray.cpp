#include "Ray.h"

void Ray::initRay(double xpos, double ypos, double zpos, double xdir, double ydir, double zdir)
{
	position.x = xpos;
	position.y = ypos;
	position.z = zpos;
	direction.x = xdir;
	direction.y = ydir;
	direction.z = zdir;
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
}
double Ray::getyDir()
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
