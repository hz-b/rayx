#define COLLISION_EPSILON 1e-6

#include "Collision.h"
#include "Utils.h"
#include "InvocationState.h"

/**************************************************************
 *                    Quadric collision
 **************************************************************/
Collision getQuadricCollision(Ray r, QuadricSurface q) {
    Collision col;
    col.found = true;
    col.hitpoint = dvec3(0, 0, 0);
    col.normal = dvec3(0, 0, 0);

    int cs = 1;
    int d_sign = q.m_icurv;
    if (abs(r.m_direction[1]) >= abs(r.m_direction[0]) && abs(r.m_direction[1]) >= abs(r.m_direction[2])) {
        cs = 2;
    } else if (abs(r.m_direction[2]) >= abs(r.m_direction[0]) && abs(r.m_direction[2]) >= abs(r.m_direction[1])) {
        cs = 3;
    }

    double x = 0;
    double y = 0;
    double z = 0;
    double a = 0;
    double b = 0;
    double c = 0;

    if (cs == 1) {
        double aml = r.m_direction[1] / r.m_direction[0];
        double anl = r.m_direction[2] / r.m_direction[0];
        y = r.m_position[1] - aml * r.m_position[0];
        z = r.m_position[2] - anl * r.m_position[0];
        d_sign = int(sign(r.m_direction[0]) * q.m_icurv);

        a = q.m_a11 + 2 * q.m_a12 * aml + q.m_a22 * aml * aml + 2 * q.m_a13 * anl + 2 * q.m_a23 * aml * anl + q.m_a33 * anl * anl;
        b = q.m_a14 + q.m_a24 * aml + q.m_a34 * anl + (q.m_a12 + q.m_a22 * aml + q.m_a23 * anl) * y + (q.m_a13 + q.m_a23 * aml + q.m_a33 * anl) * z;
        c = q.m_a44 + q.m_a22 * y * y + 2 * q.m_a34 * z + q.m_a33 * z * z + 2 * y * (q.m_a24 + q.m_a23 * z);

        double bbac = b * b - a * c;
        if (bbac < 0) {
            col.found = false;
        } else {
            if (abs(a) > abs(c) * 1e-10) {
                x = (-b + d_sign * sqrt(bbac)) / a;
            } else {
                x = (-c / 2) / b;
            }
        }
        y = y + aml * x;
        z = z + anl * x;
    } else if (cs == 2) {
        double alm = r.m_direction[0] / r.m_direction[1];
        double anm = r.m_direction[2] / r.m_direction[1];
        x = r.m_position[0] - alm * r.m_position[1];
        z = r.m_position[2] - anm * r.m_position[1];
        d_sign = int(sign(r.m_direction[1]) * q.m_icurv);

        a = q.m_a22 + 2 * q.m_a12 * alm + q.m_a11 * alm * alm + 2 * q.m_a23 * anm + 2 * q.m_a13 * alm * anm + q.m_a33 * anm * anm;
        b = q.m_a24 + q.m_a14 * alm + q.m_a34 * anm + (q.m_a12 + q.m_a11 * alm + q.m_a13 * anm) * x + (q.m_a23 + q.m_a13 * alm + q.m_a33 * anm) * z;
        c = q.m_a44 + q.m_a11 * x * x + 2 * q.m_a34 * z + q.m_a33 * z * z + 2 * x * (q.m_a14 + q.m_a13 * z);

        double bbac = b * b - a * c;
        if (bbac < 0) {
            col.found = false;
        } else {
            if (abs(a) > abs(c) * 1e-10) {
                y = (-b + d_sign * sqrt(bbac)) / a;
            } else {
                y = (-c / 2) / b;
            }
        }
        x = x + alm * y;
        z = z + anm * y;

    } else {
        double aln = r.m_direction[0] / r.m_direction[2];
        double amn = r.m_direction[1] / r.m_direction[2];
        // firstParam = aln;
        // secondParam = amn;
        x = r.m_position[0] - aln * r.m_position[2];
        y = r.m_position[1] - amn * r.m_position[2];
        d_sign = int(sign(r.m_direction[2]) * q.m_icurv);

        a = q.m_a33 + 2 * q.m_a13 * aln + q.m_a11 * aln * aln + 2 * q.m_a23 * amn + 2 * q.m_a12 * aln * amn + q.m_a22 * amn * amn;
        b = q.m_a34 + q.m_a14 * aln + q.m_a24 * amn + (q.m_a13 + q.m_a11 * aln + q.m_a12 * amn) * x + (q.m_a23 + q.m_a12 * aln + q.m_a22 * amn) * y;
        c = q.m_a44 + q.m_a11 * x * x + 2 * q.m_a24 * y + q.m_a22 * y * y + 2 * x * (q.m_a14 + q.m_a12 * y);

        double bbac = b * b - a * c;
        if (bbac < 0) {
            col.found = false;
        } else {
            if (abs(a) > abs(c) * 1e-10) {  // pow(10, double(-10))) {
                z = (-b + d_sign * sqrt(bbac)) / a;
            } else {
                z = (-c / 2) / b;
            }
        }
        x = x + aln * z;
        y = y + amn * z;
        // r.m_position = dvec3(a, b, c);
    }

    // intersection point is in the negative direction (behind the position when the direction is followed forwards), set weight to 0
    if ((x - r.m_position.x) / r.m_direction.x < 0 || (y - r.m_position.y) / r.m_direction.y < 0 || (z - r.m_position.z) / r.m_direction.z < 0) {
        col.found = false;
    }

    col.hitpoint = dvec3(x, y, z);

    double fx = 2 * q.m_a14 + 2 * q.m_a11 * x + 2 * q.m_a12 * y + 2 * q.m_a13 * z;
    double fy = 2 * q.m_a24 + 2 * q.m_a12 * x + 2 * q.m_a22 * y + 2 * q.m_a23 * z;
    double fz = 2 * q.m_a34 + 2 * q.m_a13 * x + 2 * q.m_a23 * y + 2 * q.m_a33 * z;
    col.normal = normalize(dvec3(fx, fy, fz));
    return col;
}

/**************************************************************
 *                    Cubic collision
 **************************************************************/
/**
 * taken from RAY-UI Nov. 2023:
 *  new version of Th. Zeschke by Oct. 18, 2007
 *  corresponding exe-version is called ray_thomas1.exe (on VAX only)
 * 
 *  calculates intersection of a straight line  
 *  equation straight line: 
 *  (x-x1)/al = (y-y1)/am = (z-z1)/an 
 *  with a surface of 3. order
 * 
 *  method: the iterative Newtonmethod is used for zero point search
 *  result is X,Y,Z of intersection
*/
Collision getCubicCollision(Ray r, CubicSurface cu) {
    Collision col;
    col.found = true;
    col.hitpoint = dvec3(0, 0, 0);
    col.normal = dvec3(0, 0, 0);

    int cs = 1;
    if (abs(r.m_direction[1]) >= abs(r.m_direction[0]) && abs(r.m_direction[1]) >= abs(r.m_direction[2])) {
        cs = 2;
    } else if (abs(r.m_direction[2]) >= abs(r.m_direction[0]) && abs(r.m_direction[2]) >= abs(r.m_direction[1])) {
        cs = 3;
    }

    double x = 0;
    double y = 0;
    double z = 0;
    double x1 = 0;
    double xx = 0;
    double y1 = 0;
    double yy = 0;
    double z1 = 0;
    double zz = 0;
    double counter = 0;
    double dx = 0;
    double al = r.m_direction[0];
    double am = r.m_direction[1];
    double an = r.m_direction[2];

    if (cs == 1) {
        
        double aml = r.m_direction[1] / r.m_direction[0];
        double anl = r.m_direction[2] / r.m_direction[0];    


        do {
            x1 = xx;
            y1 = r.m_position[1] - aml * (r.m_position[0] - xx);
            z1 = r.m_position[2] - anl * (r.m_position[0] - xx);

            
            double func = (2*((x1-xx)*an-al*z1)*cu.m_a23-(2*cu.m_a24+cu.m_b12*pow(float(xx), 2.0))*al+((x1-xx)*am-al*y1)*(cu.m_a22+cu.m_b21*xx))*((x1-xx)*am-al*y1);
            func = func + pow(float(((x1-xx)*an-al*z1)), 2.0)*cu.m_a33;
            func = func - ((x1-xx)*an-al*z1)*(2*cu.m_a34+cu.m_b13*pow(float(xx), 2.0)*al + cu.m_a44*pow(float(al), 2.0));
            func = (func - (2*((x1-xx)*an-al*z1)*cu.m_a13-(cu.m_a11*xx+2*cu.m_a14)*al+2*((x1-xx)*am-al*y1)*cu.m_a12)*al*xx)*al;
            func = (func - (pow(float(((x1 - xx) * am - al * y1)), 2.0) * cu.m_b23 + ((x1 - xx) * am - al * y1) * ((x1 - xx) * an - al * z1) * cu.m_b32 - 
            ((x1 - xx) * an - al * z1) * al * cu.m_b31 * xx) * ((x1 - xx) * an - al * z1)/ pow(float(al), 3));
            
            double dfunc = (2*((x1-xx)*an-al*z1)*cu.m_a23-(2*cu.m_a24+cu.m_b12*pow(float(xx), 2))*al+((x1-xx)*am-al*y1)*(cu.m_a22+cu.m_b21*xx))*am;
            dfunc = dfunc - (2*(cu.m_a12*am+cu.m_a13*an)+cu.m_a11*al)*al*xx;
            dfunc = dfunc + (2*((x1-xx)*an-al*z1)*cu.m_a13-(cu.m_a11*xx+2*cu.m_a14)*al+2*((x1-xx)*am-al*y1)*cu.m_a12);
            dfunc = dfunc*al + ((cu.m_a22+cu.m_b21*xx)*am+2*(cu.m_a23*an+al*cu.m_b12*xx) - ((x1-xx)*am-al*y1)*cu.m_b21)*((x1-xx)*am-al*y1);
            dfunc = (dfunc + 2*((x1-xx)*an-al*z1)*(cu.m_a33*an+al*cu.m_b13*xx)-(2*cu.m_a34+cu.m_b13*pow(float(xx), 2))*al*an)*al;
            dfunc = (dfunc - ((((x1-xx)*an-al*z1)*(al*cu.m_b31+am*cu.m_b32)-al*an*cu.m_b31*xx+((x1-xx)*am-al*y1)*(2*am*cu.m_b23+an*cu.m_b32))*((x1-xx)*an-al*z1)+(pow(float(((x1-xx)*am-al*y1)), 2)*cu.m_b23+((x1-xx)*am-al*y1)*((x1-xx)*an-al*z1)*cu.m_b32-((x1-xx)*an-al*z1)*al*cu.m_b31*xx)*an));
            dfunc = dfunc / pow(float(al), 3);

            if (dfunc < 0.001) {dfunc = 0.001;}
            
            dx = func / dfunc;
            xx = xx - dx;

            x = x1;
            y = y1;
            z = z1;
            
            if (counter > 1000) {
                x = -2 * y1 / 2 / aml;
                y = y1 + aml * x;
                z = z1 + anl * x;
            }
            counter++;
        } while (dx > 0.001);
        

    } else if (cs == 2) {
        double alm = r.m_direction[0] / r.m_direction[1];
        double anm = r.m_direction[2] / r.m_direction[1];
        
        do {

            x1 = r.m_position[0] - alm * (r.m_position[1] - yy);
            y1 = yy;
            z1 = r.m_position[2] - anm * (r.m_position[1] - yy);
            
            double func = (2*(((y1-yy)*an-am*z1)*cu.m_a13-(cu.m_a12*yy+cu.m_a14)*am)+((y1-yy)*al-am*x1)*(cu.m_a11+cu.m_b12*yy))*((y1-yy)*al-am*x1);
            func = func + (((y1-yy)*an-am*z1)*cu.m_a33-2*(cu.m_a23*yy+cu.m_a34)*am)*((y1-yy)*an-am*z1)+(2*cu.m_a24*yy+cu.m_a44+cu.m_a22*pow(float(yy), 2)*pow(float(am), 2));
            func = func * am + ((((y1-yy)*an-am*z1)*cu.m_b32-am*cu.m_b23*yy)*am*yy-pow(float((y1-yy)*al-am*x1), 2)*cu.m_b13)*((y1-yy)*an-am*z1);
            func = func - (pow(float(((y1-yy)*an-am*z1)), 2)*cu.m_b31+pow(float(am), 2)*cu.m_b21*pow(float(yy), 2))*((y1-yy)*al-am*x1);
            func = func / pow(float(am), 3);

            double dfunc = (pow(float((y1-yy)*an-am*z1), 2)*cu.m_b31+pow(float(am), 2)*cu.m_b21*pow(float(yy), 2)*al+2*(((y1-yy)*an-am*z1)*an*cu.m_b31-pow(float(am), 2))*cu.m_b21*yy)*((y1-yy)*al-am*x1);
            dfunc = dfunc - ((((y1-yy)*an-am*z1)*cu.m_b32-am*cu.m_b23*yy)*am*yy-pow(float((y1-yy)*al-am*x1), 2)*cu.m_b13)*an;
            dfunc = dfunc + (2*((y1-yy)*al-am*x1)*al*cu.m_b13-(am*cu.m_b23+an*cu.m_b32)*am*yy+(((y1-yy)*an-am*z1)*cu.m_b32-am*cu.m_b23*yy)*am)*((y1-yy)*an-am*z1);
            dfunc = dfunc - (((cu.m_a11+cu.m_b12*yy)*al+2*(cu.m_a12*am+cu.m_a13*an)-((y1-yy)*al-am*x1)*cu.m_b12)*((y1-yy)*al-am*x1)-2*(cu.m_a22*pow(float(am), 2)*yy+cu.m_a23*pow(float(am), 2)*z1-cu.m_a23*am*an*y1+2*cu.m_a23*am*an*yy+cu.m_a24*pow(float(am), 2)+cu.m_a33*am*an*z1-cu.m_a33*pow(float(an), 2)*y1+cu.m_a33*pow(float(an), 2)*yy+cu.m_a34*am*an)+(2*(((y1-yy)*an-am*z1)*cu.m_a13-(cu.m_a12*yy+cu.m_a14)*am)+((y1-yy)*al-am*x1)*(cu.m_a11+cu.m_b12*yy))*al)*am;
            dfunc = dfunc / pow(float(am), 3);

            if (dfunc < 0.001) {dfunc = 0.001;}
            
            dx = func / dfunc;
            yy = yy - dx;

            x = x1;
            y = y1;
            z = z1;
            
            if (counter > 1000) {
                x = x1;
                y = 0;
                z = z1;
            }
            counter++;
        } while (dx > 0.001);
    } else {
        double aln = r.m_direction[0] / r.m_direction[2];
        double amn = r.m_direction[1] / r.m_direction[2];
        
        do {

            x1 = x - aln * (z - zz);
            y1 = r.m_position[1] - amn * (z - zz);
            z1 = zz;
            
            double func = ((2*(((z1-zz)*am-an*y1)*cu.m_a12-(cu.m_a13*zz+cu.m_a14)*an)+((z1-zz)*al-an*x1)*cu.m_a11)*((z1-zz)*al-an*x1)+(((z1-zz)*am-an*y1)*cu.m_a22-2*(cu.m_a23*zz+cu.m_a24)*an)*((z1-zz)*am-an*y1)+(2*cu.m_a34*zz+cu.m_a44+cu.m_a33*pow(float(zz), 2))*pow(float(an), 2))*an;
            func = func - ((((z1-zz)*am-an*y1)*cu.m_b12-an*cu.m_b13*zz)*pow(float(((z1-zz)*al-an*x1)), 2)-(((z1-zz)*am-an*y1)*cu.m_b23-an*cu.m_b32*zz)*((z1-zz)*am-an*y1)*an*zz+(pow(float(((z1-zz)*am-an*y1)), 2)*cu.m_b21+pow(float(an), 2)*cu.m_b31*pow(float(zz), 2))*((z1-zz)*al-an*x1));
            func = func / pow(float(an), 3);


            double dfunc = (((z1-zz)*am-an*y1)*cu.m_a22-2*(cu.m_a23*zz+cu.m_a24)*an)*am + (2*(cu.m_a12*am+cu.m_a13*an)+cu.m_a11*al)*((z1-zz)*al-an*x1);
            dfunc = dfunc + ((z1-zz)*am-an*y1)*(cu.m_a22*am+2*cu.m_a23*an) - 2*(cu.m_a33*zz+cu.m_a34)*pow(float(an), 2);
            dfunc = (dfunc + (2*(((z1-zz)*am-an*y1)*cu.m_a12-(cu.m_a13*zz+cu.m_a14)*an)+((z1-zz)*al-an*x1)*cu.m_a11)*al)*an;
            dfunc = dfunc - (2*(((z1-zz)*am-an*y1)*am*cu.m_b21-pow(float(an), 2)*cu.m_b31*zz+(((z1-zz)*am-an*y1)* cu.m_b12-an*cu.m_b13*zz)*al)*((z1-zz)*al-an*x1)+(pow(float(((z1-zz)*am-an*y1)), 2)*cu.m_b21+pow(float(an), 2)*cu.m_b31*pow(float(zz), 2))*al+pow(float(((z1-zz)*al-an*x1)), 2)*(am*cu.m_b12+an*cu.m_b13)-((z1-zz)*am-an*y1)*(am*cu.m_b23+an*cu.m_b32)*an*zz+(((z1-zz)*am-an*y1)*cu.m_b23-an*cu.m_b32*zz)*(am*z1-2*am*zz-an*y1)*an);
            dfunc = (-dfunc) / pow(float(an), 3);




            if (abs(dfunc) < 0.001) {dfunc = 0.001;}
            
            dx = func / dfunc;
            zz = zz - dx;

            x = x1;
            y = y1;
            z = z1;
            
            if (counter > 1000) {
                x = x1+aln*z;
                y = y1+amn*z;
                z = -2*y1/2/amn;
            }
            counter++;
        } while (dx > 0.001);        
        // r.m_position = dvec3(a, b, c);
    }

    // intersection point is in the negative direction (behind the position when the direction is followed forwards), set weight to 0
    if ((x - r.m_position.x) / r.m_direction.x < 0 || (y - r.m_position.y) / r.m_direction.y < 0 || (z - r.m_position.z) / r.m_direction.z < 0) {
        col.found = false;
    }

    col.hitpoint = dvec3(x, y, z);

    double fx = 2 * cu.m_a14 + 2 * cu.m_a11 * x + 2 * cu.m_a12 * y + 2 * cu.m_a13 * z;
    double fy = 2 * cu.m_a24 + 2 * cu.m_a12 * x + 2 * cu.m_a22 * y + 2 * cu.m_a23 * z;
    double fz = 2 * cu.m_a34 + 2 * cu.m_a13 * x + 2 * cu.m_a23 * y + 2 * cu.m_a33 * z;
    col.normal = normalize(dvec3(fx, fy, fz));
    return col;
}


/**************************************************************
 *                    Toroid Collision
 **************************************************************/
// this uses newton to approximate a solution.
Collision getToroidCollision(Ray r, ToroidSurface toroid, bool isTriangul) {
    // Constants
    const double NEW_TOLERANCE = 0.0001;
    const int NEW_MAX_ITERATIONS = 50;

    double longRad = toroid.m_longRadius;
    double shortRad = (toroid.m_toroidType == TOROID_TYPE_CONVEX) ? -toroid.m_shortRadius : toroid.m_shortRadius;

    Collision col;
    col.found = true;
    col.hitpoint = dvec3(0, 0, 0);
    col.normal = dvec3(0, 0, 0);

    // sign radius: +1 = concave, -1 = convex
    double isigro = sign(shortRad);

    // double aln = r.m_direction.x / r.m_direction.z;
    // double amn = r.m_direction.y / r.m_direction.z;

    dvec4 normal = dvec4(0, 0, 0, 0);
    double xx = 0.0;
    double zz = 0.0;
    double yy = 0.0;
    double dz = 0.0;
    dvec3 normalized_dir = dvec3(r.m_direction) / r.m_direction.z;

    int n = 0;
    // Newton's method iteration
    // While not converged...
    do {
        zz = zz + dz;
        xx = r.m_position.x + normalized_dir.x * (zz - r.m_position.z);
        if (xx * xx > shortRad * shortRad) {
            xx = xx / abs(xx) * 0.95 * shortRad;
        }
        yy = r.m_position.y + normalized_dir.y * (zz - r.m_position.z);
        double sq = sqrt(shortRad * shortRad - xx * xx);
        double rx = (longRad - shortRad + isigro * sq);

        // Calculate toroid normal
        normal.x = (-2 * xx * isigro / sq) * rx;
        normal.y = -2 * (yy - longRad);
        normal.z = -2 * zz;

        double func = -rx * rx + (yy - longRad) * (yy - longRad) + zz * zz;
        double df = normalized_dir.x * normal.x + normalized_dir.y * normal.y + normal.z;  // dot(normalized_dir, dvec3(normal));
        dz = func / df;
        n += 1;
        if (n >= NEW_MAX_ITERATIONS) {
            col.found = false;
            return col;
        }
    } while (abs(dz) > NEW_TOLERANCE);

    col.normal = normalize(dvec3(normal));
    col.hitpoint = dvec3(xx, yy, zz);

    if (isTriangul) { // TODO : Hack, Triangulation sensetive to direction apparently. Actual fix or func rework is needed!
        return col;
    }

    dvec3 rayToHitpoint = col.hitpoint - r.m_position;
    // edit: if ray points away from the hitpoint, no collision can be found.
    // Note that multiplying the rays direction with -1 SHOULD totally have an effect on the collision detection - in most cases this 180° rotation
    // will make the ray point away from the toroid, and hence preventing a Collision completely. The above code however, is unaffected when
    // multiplying the ray direction with -1. Due to it having no effect on `dvec3 normalized_dir = dvec3(r.m_direction) / r.m_direction.z;`
    col.found = dot(rayToHitpoint, r.m_direction) > 0.0;

    return col;
}

/**************************************************************
 *                    Collision Finder
 **************************************************************/

Collision RAYX_API findCollisionInElementCoords(Ray r, Surface surface, Cutout cutout, bool isTriangul) {
    double sty = surface.m_type;

    Collision col;
    if (sty == STYPE_PLANE_XZ) {
        col.normal = dvec3(0, -sign(r.m_direction.y), 0);

        // the `time` that it takes for the ray to hit the plane (if we understand the rays direction as its velocity).
        // velocity = distance/time <-> time = distance/velocity from school physics.
        // (We need to negate the position, as with positive velocity, you need a negative position to eventually reach the zero point (aka the plane).
        // Having positive position & positive velocity means that we never hit the plane as we move away from it.)
        double time = -r.m_position.y / r.m_direction.y;

        col.hitpoint.x = r.m_position.x + r.m_direction.x * time;
        col.hitpoint.z = r.m_position.z + r.m_direction.z * time;
        col.hitpoint.y = 0;

        // the ray should not face away from the plane (or equivalently, the ray should not come *from* the plane). If that is the case we set `found = false`.
        col.found = time >= 0;
    } else if (sty == STYPE_TOROID) {
        col = getToroidCollision(r, deserializeToroid(surface), isTriangul);
    } else if (sty == STYPE_QUADRIC) {
        col = getQuadricCollision(r, deserializeQuadric(surface));
    } else if (sty == STYPE_CUBIC) {
        col = getCubicCollision(r, deserializeCubic(surface));
    } else {
        col.found = false;

        _throw("invalid surfaceType!");
        return col;  // has found = false
    }

    // cutout is applied in the XZ plane.
    if (!inCutout(cutout, col.hitpoint.x, col.hitpoint.z)) {
        col.found = false;
    }

    // Both r.m_direction and col.normal are in element-coordinates.
    // The collision normal should point "out of the surface", i.e. in the direction that the ray came from.
    // In other words we want `dot(r.m_direction, col.normal) <= 0`.
    // Later on, we'd like to remove this hotfix, and each individual get*Collision function should already satisfy this constraint.
    if (dot(r.m_direction, col.normal) > 0.0) {
        col.normal = col.normal * -1.0;
    }
    return col;
}

// checks whether `r` collides with the element of the given `id`,
// and returns a Collision accordingly.
Collision findCollisionWith(Ray r, uint id) {
    // misalignment
    r = rayMatrixMult(r, inv_elements[id].m_inTrans);  // image plane is the x-y plane of the coordinate system
    Collision col = findCollisionInElementCoords(r, inv_elements[id].m_surface, inv_elements[id].m_cutout, false);
    if (col.found) {
        col.elementIndex = int(id);
    }

    SlopeError sE = inv_elements[id].m_slopeError;
    col.normal = applySlopeError(col.normal, sE, 0);

    return col;
}

// Returns the next collision for the ray `_ray`.
Collision findCollision() {
    if (inv_pushConstants.sequential == 1.0) {
        if (_ray.m_lastElement >= inv_elements.length() - 1) {
            Collision col;
            col.found = false;
            return col;
        }
        return findCollisionWith(_ray, uint(_ray.m_lastElement + 1));
    }

    // global coordinates of first intersection point of ray among all elements in beamline
    Collision best_col;
    best_col.found = false;

    // the distance the ray has to travel to reach `best_col`.
    double best_dist = infinity();

    // move ray slightly forward.
    // -> prevents hitting an element very close to the previous collision.
    // -> prevents self-intersection.
    Ray r = _ray;
    r.m_position += r.m_direction * COLLISION_EPSILON;

    // Find intersection points through all elements
    for (uint elementIndex = 0; elementIndex < uint(inv_elements.length()); elementIndex++) {
        Collision current_col = findCollisionWith(r, elementIndex);
        if (!current_col.found) {
            continue;
        }

        dvec3 global_hitpoint = dvec3(inv_elements[elementIndex].m_outTrans * dvec4(current_col.hitpoint, 1));
        double current_dist = length(global_hitpoint - _ray.m_position);

        if (current_dist < best_dist) {
            best_col = current_col;
            best_dist = current_dist;
        }
    }

    return best_col;
}
