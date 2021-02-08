#ifndef SPHEREMIRROR_H
#define SPHEREMIRROR_H
#include "Quadric.h"

namespace RAY
{

    class RAY_API SphereMirror : public Quadric {

    public:
        
        SphereMirror(double width, double height, double grazingIncidence, double azimuthal, double distanceToPreceedingElement, double entranceArmLength, double exitArmLength, std::vector<double> misalignmentParams);
        SphereMirror();
        ~SphereMirror();

        void calcRadius();
        double getWidth();
        double getHeight();
        double getRadius();
        
    private:
        double m_totalWidth;
        double m_totalHeight;
        double m_radius;
        double m_exitArmLength;
        double m_entranceArmLength;
        // grazing incidence, in rad
        double m_alpha;
        double m_beta;
        double m_chi;
        double m_distanceToPreceedingElement;
        //double m_Depth;
        //double m_verDivergence;
        //double m_horDivergence;
        
    };

} // namespace RAY
#endif