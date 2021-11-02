#pragma once
#pragma pack(16)

#include <stdexcept>
#include <iostream>
#include <vector>



class Ray
{
public:
	Ray(double xpos, double ypos, double zpos, double xdir, double ydir, double zdir, double s0, double s1, double s2, double s3, double en, double w, double pathLength = 13, double order = 1, double lastElement = 5, double extraParameter = 2);
	Ray(double* location);
	Ray();
	~Ray();
	std::vector<double> getRayInformation();
	double getxDir();
	double getyDir();
	double getzDir();
	double getxPos();
	double getyPos();
	double getzPos();
	double getEnergy();
	double getWeight();
	double getS0();
	double getS1();
	double getS2();
	double getS3();

private:
	struct vec3
	{
		double x, y, z;
	};
	struct vec4
	{
		double s0, s1, s2, s3;
	};
	vec3 m_position;
	double m_weight;
	vec3 m_direction;
	double m_energy;
	vec4 m_stokes;
	double m_pathLength;
	double m_order;
	double m_lastElement;
	double m_extraParam;
};
