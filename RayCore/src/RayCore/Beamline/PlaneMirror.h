#include "Quadric.h"

namespace RAY
{

    class RAY_API PlaneMirror : public Quadric {

    public:
        
        PlaneMirror(double width, double height, double grazingIncidence, double azimuthal, double distanceToPreceedingElement, std::vector<double> misalignmentParams);
        PlaneMirror();
        ~PlaneMirror();

        double getWidth();
        double getHeight();
        
    private:
        double m_totalWidth;
        double m_totalHeight;
        double m_alpha;
        double m_beta;
        double m_chi;
        double m_distanceToPreceedingElement;
        //double m_Depth;
        //double m_verDivergence;
        //double m_horDivergence;
        
    };

} // namespace RAY