#include "Ray.h"
#include <cstring>

Ray::Ray(double xpos, double ypos, double zpos, double xdir, double ydir, double zdir, double s0, double s1, double s2, double s3, double en, double w)
{
	m_position.x = xpos;
	m_position.y = ypos;
	m_position.z = zpos;
	m_direction.x = xdir;
	m_direction.y = ydir;
	m_direction.z = zdir;
	m_stokes.s0 = s0;
	m_stokes.s1 = s1;
	m_stokes.s2 = s2;
	m_stokes.s3 = s3;
	m_weight = w;
	m_energy = en;
}
//copies 64 byts of data in the following format: xloc, yloc, zloc, weight, xdir, ydir, zdir, placeholder
Ray::Ray(double* location) {
	memcpy(&m_position.x, location, 64);
}
Ray::Ray() {}
Ray::~Ray() {

}

std::vector<double> Ray::getRayInformation()
{
	std::vector<double> rayInfo;
	rayInfo.push_back(m_position.x);
	rayInfo.push_back(m_position.y);
	rayInfo.push_back(m_position.z);
	rayInfo.push_back(m_direction.x);
	rayInfo.push_back(m_direction.y);
	rayInfo.push_back(m_direction.z);
	return rayInfo;
}

double Ray::getxDir()
{
	return m_direction.x;
}
double Ray::getyDir()
{
	return m_direction.y;
}

double Ray::getzDir()
{
	return m_direction.z;
}

double Ray::getxPos()
{
	return m_position.x;
}

double Ray::getyPos()
{
	return m_position.y;
}

double Ray::getzPos()
{
	return m_position.z;
}
double Ray::getEnergy()
{
	return m_energy;
}
double Ray::getWeight()
{
	return m_weight;
}
double Ray::getS0() {
	return m_stokes.s0;
}
double Ray::getS1() {
	return m_stokes.s1;
}
double Ray::getS2() {
	return m_stokes.s2;
}
double Ray::getS3() {
	return m_stokes.s3;
}