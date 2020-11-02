#include "Controller.h"


namespace RAY {

    /// static pointer to the only Controller instance
    Controller* Controller::m_Instance = 0;


    Controller::Controller()
    {
    }


    Controller::~Controller()
    {

    }



    /**
     * Get the current controller instance or create one if there is none (singleton)
     * @see Controller::Controller()
     * @return Returns the controller instance
     */
    Controller* Controller::getInstance()
    {
        if(!m_Instance)
            m_Instance = new Controller();
        return m_Instance;
    }

}
