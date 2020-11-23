#include <stdexcept>
#include <iostream>
#include <vector>

class Ray {
private:
	struct vec3 {
		double x, y, z;
	};
	struct disk {
		vec3 origin, normal;
		double radius;
	};
	vec3 position, direction;
	disk target;
	bool hitplane;
	void normalizeVector();
	double dotProduct(vec3 a, vec3 b);

public:
	void initRay(double xpos, double ypos, double zpos, double xdir, double ydir, double zdir);
	void initDisk(double xpos, double ypos, double zpos, double xdir, double ydir, double zdir, double radius);
	int traceRay();
	bool hitPlane();
	std::vector<double> getRayInformation();
	double getxDir();
	double getyDir();
	double getzDir();
	double getxPos();
	double getyPos();
	double getzPos();


};
