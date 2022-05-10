#pragma once

#include "Core.h"
#include "Presenter/Presenter.h"

class Beamline;

namespace RAYX {
/**
 *	Abstract class to be used by applications built with the RayCore library.
 */
class RAYX_API Application {
  public:
    Application();
    virtual ~Application();

    void loadDummyBeamline();
    virtual void run() = 0;

  protected:
    std::shared_ptr<Beamline> m_Beamline;
    Presenter m_Presenter;
};

}  // namespace RAYX